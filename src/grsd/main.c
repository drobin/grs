#include <signal.h>
#include <stdio.h>
#include <string.h>

#include <grsd.h>

static grsd_t handle;

static void leave_handler(int sig) {
  grsd_listen_exit(handle);
}

int main(int argc, char** argv) {
  struct sigaction sa;

  memset(&sa, 0, sizeof(struct sigaction));
  sa.sa_handler = leave_handler;

  sigaction(SIGINT, &sa, NULL);

  handle = grsd_init();

  grsd_listen(handle);
  grsd_destroy(handle);

  return 0;
}
