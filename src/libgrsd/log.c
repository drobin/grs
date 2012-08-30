#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "log.h"

enum LOG_LEVEL {
  LOG_INFO,
  LOG_WARN,
  LOG_ERR,
  LOG_FATAL,
  LOG_DEBUG
};

static void do_log(enum LOG_LEVEL level, const char* format, va_list ap) {
  char *fmt;

  switch (level) {
  case LOG_INFO:  asprintf(&fmt, "[INFO] %s\n", format); break;
  case LOG_WARN:  asprintf(&fmt, "[WARN] %s\n", format); break;
  case LOG_ERR:   asprintf(&fmt, "[ERR] %s\n", format); break;
  case LOG_FATAL: asprintf(&fmt, "[FATAL] %s\n", format); break;
  case LOG_DEBUG: asprintf(&fmt, "[DEBUG] %s\n", format);; break;
  }

  if (level == LOG_ERR || level == LOG_FATAL) {
    vfprintf(stderr, fmt, ap);
  } else {
    vfprintf(stdout, fmt, ap);
  }

  free(fmt);
}

void log_info(const char* format, ...) {
  va_list ap;

  va_start(ap, format);
  do_log(LOG_INFO, format, ap);
  va_end(ap);
}

void log_warn(const char* format, ...) {
  va_list ap;

  va_start(ap, format);
  do_log(LOG_WARN, format, ap);
  va_end(ap);
}

void log_err(const char* format, ...) {
  va_list ap;

  va_start(ap, format);
  do_log(LOG_ERR, format, ap);
  va_end(ap);
}

void log_fatal(const char* format, ...) {
  va_list ap;

  va_start(ap, format);
  do_log(LOG_FATAL, format, ap);
  va_end(ap);
}

void log_debug(const char* format, ...) {
  va_list ap;

  va_start(ap, format);
  do_log(LOG_DEBUG, format, ap);
  va_end(ap);
}
