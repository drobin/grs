#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <grsd.h>

static grsd_t handle;

static void leave_handler(int sig) {
  grsd_listen_exit(handle);
}

static void usage() {
  printf("USAGE\n");
  exit(1);
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

  grsd_listen(handle);
  grsd_destroy(handle);

  return 0;
}
