#include <fcntl.h>  /* open */
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>     /* Prototypes of commonly used library functions,
                           plus EXIT_SUCCESS and EXIT_FAILURE constants */
#include <string.h>     /* Commonly used string-handling functions */

#ifndef BUF_SIZE
#define BUF_SIZE 1024
#endif

#ifdef TRUE
#undef TRUE
#endif

#ifdef FALSE
#undef FALSE
#endif

typedef enum { FALSE, TRUE } Boolean;

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

void errExit(const char *format, ...) {
  va_list argList;

  va_start(argList, format);
  outputError(TRUE, errno, TRUE, format, argList);
  va_end(argList);

  terminate(TRUE);
}

int main(int argc, char *argv[]) {
  int inputFile;

  /* for (int i = 0; i < argc; i++) { */
  /*   printf("arg %d: %s\n", i, argv[i]); */
  /* } */

  if (argc != 3 || strcmp(argv[1], "--help") == 0) {
    usageErr("%s old-file new-file\n", argv[0]);
  }

  inputFile = open(argv[1], O_RDONLY);
  if (inputFile == -1) {

  }
}
