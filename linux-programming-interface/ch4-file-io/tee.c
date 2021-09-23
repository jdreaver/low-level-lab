#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "error_functions.h"

#ifndef BUF_SIZE
#define BUF_SIZE 1024
#endif

int main(int argc, char *argv[]) {
  opterr = 0; // Global variable that tells getopt not to print error messages

  // Parse options
  int c;
  int helpFlag = 0;
  while ((c = getopt (argc, argv, "h")) != -1) {
    switch (c) {
    case 'h':
      helpFlag = 1;
      break;
    case '?':
      if (isprint (optopt)) {
	fprintf (stderr, "Unknown option `-%c'.\n", optopt);
      } else {
	fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
        exit(EXIT_FAILURE);
      default:
        abort();
      }
    }
  }

  // Parse positional argument
  int numPositionalArgs = argc - optind;
  if (helpFlag || numPositionalArgs != 1) {
    usageErr("%s FILE\n", argv[0]);
  }

  char *path = argv[1];
  int openFlags = O_CREAT | O_RDWR | O_TRUNC;
  mode_t filePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH; /* rw-rw-rw- */
  int outputFile = open(path, openFlags, filePerms);
  if (outputFile == -1) {
    char errorBuf[500];
    snprintf(errorBuf, 500, "failed to open %s", path);
    errExit(errorBuf);
  }

  int numRead;
  char buf[BUF_SIZE];
  while ((numRead = read(STDIN_FILENO, buf, BUF_SIZE)) > 0) {
    if (numRead == -1) {
      errExit("failed to read from stdin");
    }

    int numWritten;
    numWritten = write(outputFile, buf, numRead);
    if (numWritten == -1) {
      errExit("failed to write to output file");
    }

    numWritten = write(STDOUT_FILENO, buf, numRead);
    if (numWritten == -1) {
      errExit("failed to write to stdout");
    }
  }

  if (close(outputFile) == -1) {
    errExit("failed to close output file");
  }
}
