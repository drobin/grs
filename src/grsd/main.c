#include <sys/errno.h>
#include <errno.h>
#include <event.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <grsd.h>
#include <log.h>
#include <session.h>

/**
 * The event_base of the event-loop.
 */
struct event_base* event_base;

/**
 * Event is used to receive information from the ctrl_pipe.
 */
struct event* pipe_ev;

/**
 * Event is used to receive information from the SSH-server-bind.
 */
struct event* sshbind_ev;

/**
 * Event is used to receive information from the session.
 */
struct event* session_ev;

/**
 * The handle to the grs-library
 */
static grsd_t handle;

/**
 * The control-pipe is used to send commands into the event_base_loop.
 */
int ctrl_pipe[2];

static void leave_handler(int sig) {
  ssize_t nwritten;

  log_debug("Ask to leave grsd_listen");
  nwritten = write(ctrl_pipe[1], "q", 1);

  if (nwritten == 1) {
    log_debug("Leave request issued");
  } else {
    log_err("Failed to issue leave-requedt");
  }
}

static void usage() {
  printf("USAGE\n");
  exit(1);
}

static void pipe_handler(evutil_socket_t fd, short what, void* arg) {
  char c;
  size_t nread;

  log_debug("Handling incoming data from pipe");

  if ((nread = read(fd, &c, 1)) != 1) {
    log_fatal("Failed to read from pipe: %s", strerror(errno));
    event_base_loopexit(event_base, NULL);
    return;
  }

  if (c == 'q') {
    log_debug("Quit-request received");
    event_base_loopexit(event_base, NULL);
  } else {
    log_warn("Unsupported char received from pipe: %c", c); }
}

static void session_handler(evutil_socket_t fd, short what, void* arg) {
  session_t session = (session_t)arg;

  if (session_handle(session) != 0) {
    session_destroy(session);
    log_debug("Session is destroyed");
    event_free(session_ev);
    session_ev = NULL;
  }
}

static void sshbind_handler(evutil_socket_t fd, short what, void* arg) {
  grsd_t handle = (grsd_t)arg;
  session_t session;

  log_debug("New incoming connection-attempt");

  // Create a new session for the requested connection
  if ((session = session_create(handle)) == NULL) {
    // Fatal error: leave the event_base_loop
    log_fatal("Failed to create a session");
    event_base_loopexit(event_base, NULL);
    return;
  }

  // Accept the incoming connection
  if (session_accept(session, handle) != 0) {
    // Failed to accept connection, stay in event_base_loop for further
    // login-attempts
    log_err("Failed to accept incoming connection");
    session_destroy(session);
    return;
  }

  // Register the session-fd at event_base_loop
  session_ev = event_new(event_base, session_get_fd(session),
                         EV_READ|EV_PERSIST, session_handler, session);
  event_add(session_ev, NULL);

  // Initial handle the ssh-data
  event_active(session_ev, EV_READ, 1);
}

int main(int argc, char** argv) {
  struct sigaction sa;
  int ch;

  memset(&sa, 0, sizeof(struct sigaction));
  sa.sa_handler = leave_handler;

  sigaction(SIGINT, &sa, NULL);

  handle = grsd_init();

  while ((ch = getopt(argc, argv, "p:k:?")) != -1) {
    switch (ch) {
    case 'p':
      grsd_set_listen_port(handle, atoi(optarg));
      break;
    case 'k':
      grsd_set_hostkey(handle, optarg);
      break;
    default:
      usage();
    }
  }

  event_base = event_base_new();

  // Prepare the control-pipe
  if (pipe(ctrl_pipe) != 0) {
    free(handle);
    return -1;
  }

  pipe_ev = event_new(event_base, ctrl_pipe[0], EV_READ|EV_PERSIST,
                      pipe_handler, handle);
  event_add(pipe_ev, NULL);

  // Prepare grsd-library
  grsd_listen(handle);
  sshbind_ev = event_new(event_base, grsd_listen_get_fd(handle),
                         EV_READ|EV_PERSIST, sshbind_handler, handle);
  event_add(sshbind_ev, NULL);

  // Run the event-loop
  event_base_loop(event_base, 0);

  // Destroy everything
  grsd_destroy(handle);

  close(ctrl_pipe[0]);
  close(ctrl_pipe[1]);

  event_free(pipe_ev);
  event_free(sshbind_ev);

  if (session_ev != NULL) {
    event_free(session_ev);
  }

  event_base_free(event_base);

  return 0;
}
