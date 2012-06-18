#include <grsd.h>

int main(int argc, char** argv) {
  grsd_t handle;

  handle = grsd_init();
  grsd_destroy(handle);

  return 0;
}
