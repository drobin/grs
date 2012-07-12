#include <sys/errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libssh/libssh.h>

#include "itypes.h"
#include "log.h"
#include "session.h"

session_t session_create(grsd_t handle) {
  struct _session* session;
  
  if (handle == NULL) {
    return NULL;
  }
  
  if ((session = malloc(sizeof(struct _session))) == NULL) {
    return NULL;
  }
  
  if ((session->session = ssh_new()) == NULL) {
    free(session);
    return NULL;
  }
  
  session->channel = NULL;
  session->session_ev = NULL;
  session->handle = handle;
  session->state = AUTH;
  
  return session;
}

int session_destroy(session_t session) {
  if (session == NULL) {
    return -1;
  }
  
  if (session->channel != NULL) {
    ssh_channel_free(session->channel);
  }
  
  if (session->session_ev != NULL) {
    event_free(session->session_ev);
  }
  
  ssh_free(session->session);
  free(session);
  
  return 0;
}

grsd_t session_get_grsd(session_t session) {
  if (session == NULL) {
    return NULL;
  }
  
  return session->handle;
}

int session_accept(session_t session) {
  ssh_bind sshbind;
  
  if (session == NULL) {
    return -1;
  }
  
  sshbind = session->handle->bind;
  
  if (ssh_bind_accept(sshbind, session->session) != SSH_OK) {
    log_err("Error accepting connection: %s\n", ssh_get_error(sshbind));
    return -1;
  }
  
  if (ssh_handle_key_exchange(session->session) != SSH_OK) {
    log_err("Error in key exchange: %s\n", ssh_get_error(session->session));
    return -1;
  }
  
  log_info("Connection established");
  // TODO Who is connected?
  
  return 0;
}

static int session_exec(session_t session, ssh_message msg) {
  pid_t pid;
  int pipe_out[2];
  
  if (pipe(pipe_out) == -1) {
    log_err("Failed to create a pipe: %s", strerror(errno));
    return -1;
  }
  
  if ((pid = fork()) == -1) {
    log_err("Failed to fork: %s", strerror(errno));
    return -1;
  }
  
  if (pid == 0) { // The child executes the command
    char* cmd = ssh_message_channel_request_command(msg);

    log_debug("Executing '%s'", cmd);

    dup2(pipe_out[1], 1);
    close(pipe_out[0]);
    close(pipe_out[1]);

    execl("/bin/ls", "-1", NULL);
    log_err("Failed to exec: %s", strerror(errno));
    _exit(1);
  } else {
    int stat_loc;
    char buf[512];
    size_t nread;
    
    close(pipe_out[1]);
    
    while ((nread = read(pipe_out[0], buf, sizeof(buf))) > 0) {
      ssh_channel_write(session->channel, buf, nread);
    }
    
    if (waitpid(pid, &stat_loc, 0) == -1) {
      log_err("wait_pid: %s", strerror(errno));
      return -1;
    }
    
    if (WIFEXITED(stat_loc)) {
      log_debug("Process terminated with %i", WEXITSTATUS(stat_loc));
      return WEXITSTATUS(stat_loc);
    } else {
      log_err("Abnormal termination of process");
      return -1;
    }
  }
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
  
  return -1;
}

int session_handle(session_t session) {
  ssh_message msg;
  int result;
  
  msg = ssh_message_get(session->session);
  
  if (ssh_message_type(msg) == -1) {
    log_debug("ssh_message_type of -1 received. Abort...");
    return -1;
  }
  
  switch (session->state) {
  case AUTH: result = session_handle_auth(session, msg); break;
  case CHANNEL_OPEN: result = session_handle_channel_open(session, msg); break;
  case REQUEST_CHANNEL: result = session_handle_request_channel(session, msg); break;
  case NOP: result = -1;
  }
  
  ssh_message_free(msg);
  
  return result;
}
