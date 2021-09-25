#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void usageErr(const char *format, ...) {
  va_list argList;

  fflush(stdout);           /* Flush any pending stdout */

  fprintf(stderr, "Usage: ");
  va_start(argList, format);
  vfprintf(stderr, format, argList);
  va_end(argList);

  fflush(stderr);           /* In case stderr is not line-buffered */
  exit(EXIT_FAILURE);
}

void errExit(const char *userText) {
  char *errorStr;
  errorStr = strerror(errno);

  fprintf(stderr, "ERROR %s (%s)\n", userText, errorStr);
  fflush(stderr);           /* In case stderr is not line-buffered */
  exit(EXIT_FAILURE);
}

void fatal(const char *userText) {
  fprintf(stderr, "%s\n", userText);
  fflush(stderr);           /* In case stderr is not line-buffered */
  exit(EXIT_FAILURE);
}
