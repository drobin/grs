#include <sys/errno.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>

#include "grsd.h"
#include "log.h"
#include "types.h"

static void stdout2channel(evutil_socket_t fd, short what, void* arg) {
  session_t session = (session_t)arg;
  size_t nread;
  char buf[512];
  
  log_debug("Incoming data from stdout");
  
  nread = read(fd, buf, sizeof(buf));
  log_debug("%i bytes read from stdout", nread);
  
  if (nread > 0) {
    // Write data into channel
    int nwritten = ssh_channel_write(session->channel, buf, nread);
    log_debug("%i bytes written into channel", nwritten);
  } else if (nread == 0) {
    int result, stat_loc;

    log_debug("EOF on stdout, wait for process-end");
    
    if ((result = waitpid(session->pid, &stat_loc, 0)) == -1) {
      log_err("wait_pid: %s", strerror(errno));
    } else if (WIFEXITED(stat_loc)) {
      log_debug("Process terminated with %i", WEXITSTATUS(stat_loc));
    } else {
      log_err("Abnormal termination of process");
    }
    
    session_destroy(session);
  } else {
    log_err("Failed to read from pipe: %s", strerror(errno));
    session_destroy(session);
  }
}

static int session_exec(session_t session, ssh_message msg) {
  int pipe_out[2];
  
  if (pipe(pipe_out) == -1) {
    log_err("Failed to create a pipe: %s", strerror(errno));
    return -1;
  }
  
  if ((session->pid = fork()) == -1) {
    log_err("Failed to fork: %s", strerror(errno));
    return -1;
  }
  
  if (session->pid == 0) { // The child executes the command
    char* cmd = ssh_message_channel_request_command(msg);
    
    log_debug("Executing '%s'", cmd);
    
    dup2(pipe_out[1], 1);
    close(pipe_out[0]);
    close(pipe_out[1]);
    
    execl("/bin/ls", "-1", NULL);
    log_err("Failed to exec: %s", strerror(errno));
    _exit(1);
  } else {
    close(pipe_out[1]);
    session->stdout_ev = event_new(session->handle->event_base,
                                   pipe_out[0],
                                   EV_READ|EV_PERSIST,
                                   stdout2channel,
                                   session);
    event_add(session->stdout_ev, NULL);
  }
  
  return 0;
}

static int session_handle_auth(session_t session, ssh_message msg) {
  char* user;
  char* password;
  
  log_debug("Handle authentication for session");
  
  if (ssh_message_type(msg) != SSH_REQUEST_AUTH) {
    log_debug("Ignoring message of type %i", ssh_message_type(msg));
    
    ssh_message_reply_default(msg);
    
    return 0;
  }
  
  if (ssh_message_subtype(msg) != SSH_AUTH_METHOD_PASSWORD) {
    log_debug("Currently only password-authentication is supported.");
    log_debug("Rejecting auth-type %i", ssh_message_subtype(msg));
    
    ssh_message_auth_set_methods(msg, SSH_AUTH_METHOD_PASSWORD);
    ssh_message_reply_default(msg);
    
    return 0;
  }
  
  user = ssh_message_auth_user(msg);
  password = ssh_message_auth_password(msg);
  log_debug("Authentication requested with %s/%s", user, password);
  
  if (strcmp(user, password)) {
    log_debug("Authentication rejected");
    ssh_message_reply_default(msg);
    return 0;
  }
  
  log_debug("Authentication succeeded");
  ssh_message_auth_reply_success(msg, 0);
  session->state = CHANNEL_OPEN;
  
  return 0;
}

static int session_handle_channel_open(session_t session, ssh_message msg) {
  log_debug("Handle open-request for a channel");
  
  if (ssh_message_type(msg) != SSH_REQUEST_CHANNEL_OPEN) {
    log_debug("Ignoring message of type %i", ssh_message_type(msg));
    
    ssh_message_reply_default(msg);
    
    return 0;
  }
  
  session->channel = ssh_message_channel_request_open_reply_accept(msg);
  log_debug("Channel is open");
  
  session->state = REQUEST_CHANNEL;
  
  return 0;
}

static int session_handle_request_channel(session_t session, ssh_message msg) {
  log_debug("Handle channel-request");
  
  if (ssh_message_type(msg) != SSH_REQUEST_CHANNEL) {
    log_debug("Ignoring message of type %i", ssh_message_type(msg));
    
    ssh_message_reply_default(msg);
    
    return 0;
  }
  
  if (ssh_message_subtype(msg) != SSH_CHANNEL_REQUEST_EXEC) {
    log_debug("Ignoring channel-type %i", ssh_message_subtype(msg));
    
    ssh_message_reply_default(msg);
    
    return -1;
  }
  
  ssh_message_channel_request_reply_success(msg);
  log_debug("Channel request accepted");
  
  session_exec(session, msg);
  
  session->state = NOP; // Finished
  
  return 0;
}

void grsd_handle_session(evutil_socket_t fd, short what, void* arg) {
  session_t session = (session_t)arg;
  ssh_message msg = ssh_message_get(session->session);
  int result;
  
  log_debug("Handle incoming data from session");
  
  if (ssh_message_type(msg) == -1) {
    log_debug("ssh_message_type of -1 received. Abort...");
    session_destroy(session);
    return;
  }
  
  // Depending on the session-state handle the ssh-data.
  // If the return-code is less than 0, the the handler asks to remove the
  // related event from the event_base_loop and destroy the session
  switch (session->state) {
    case AUTH: 
      result = session_handle_auth(session, msg); break;
    case CHANNEL_OPEN:
      result = session_handle_channel_open(session, msg); break;
    case REQUEST_CHANNEL:
      result = session_handle_request_channel(session, msg); break;
    case NOP:
      result = -1; break;
  }
  
  
  ssh_message_free(msg);
  
  if (result < 0) {
    session_destroy(session);
  }
}
