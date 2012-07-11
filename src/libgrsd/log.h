#ifndef LOG_H
#define LOG_H

void log_info(const char* format, ...);
void log_warn(const char* format, ...);
void log_err(const char* format, ...);
void log_fatal(const char* format, ...);
void log_debug(const char* format, ...);

#endif  /* LOG_H */
