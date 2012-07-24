#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "hostkey.h"

static char hostkey_path[L_tmpnam];

const char* hostkey_get_path() {
  return hostkey_path;
}

int hostkey_generate() {
  char* cmd;
  int result;

  if (tmpnam(hostkey_path) == NULL) {
    return -1;
  }

  asprintf(&cmd, "ssh-keygen -N \"\" -q -f %s", hostkey_path);
  printf("Generating hostkey (%s)\n", cmd);
  result = system(cmd);
  free(cmd);

  return result;
}

int hostkey_remove() {
  if (unlink(hostkey_path) == -1) {
    perror(hostkey_path);
    return -1;
  } else {
    return 0;
  }
}
