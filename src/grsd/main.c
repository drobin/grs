#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libssh/libssh.h>
#include <libssh/server.h>

#include <log.h>
#include <session.h>

static void leave_handler(int sig) {
  log_debug("Ask to leave grsd_listen");
}

static void usage() {
  printf("USAGE\n");
  exit(1);
}

int main(int argc, char** argv) {
  struct sigaction sa;
  char* hostkey;
  int port;
  int ch;

  memset(&sa, 0, sizeof(struct sigaction));
  sa.sa_handler = leave_handler;

  sigaction(SIGINT, &sa, NULL);

  while ((ch = getopt(argc, argv, "p:k:?")) != -1) {
    switch (ch) {
    case 'p':
      port = atoi(optarg);
      break;
    case 'k':
      hostkey = optarg;
      break;
    default:
      usage();
    }
  }

  return 0;
}
