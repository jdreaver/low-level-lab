#include <fcntl.h>  /* open */
#include <stdio.h>
#include <stdlib.h>     /* Prototypes of commonly used library functions,
                           plus EXIT_SUCCESS and EXIT_FAILURE constants */
#include <string.h>     /* Commonly used string-handling functions */
#include <unistd.h>	/* read */

#include "lib.h"

#ifndef BUF_SIZE
#define BUF_SIZE 1024
#endif

int main(int argc, char *argv[]) {
  if (argc != 3 || strcmp(argv[1], "--help") == 0) {
    usageErr("%s old-file new-file\n", argv[0]);
  }

  char buf[BUF_SIZE];
  int inputFile = open(argv[1], O_RDONLY);
  if (inputFile == -1) {
    snprintf(buf, BUF_SIZE, "Failed to open %s", argv[1]);
    errExit(buf);
  }

  int openFlags = O_CREAT | O_WRONLY | O_TRUNC;
  mode_t filePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH; /* rw-rw-rw- */
  int outputFile = open(argv[2], openFlags, filePerms);
  if (outputFile == -1) {
    snprintf(buf, BUF_SIZE, "Failed to open %s", argv[1]);
    errExit(buf);
  }

  /* Transfer data until we encounter end of input or an error */
  int numRead;

  while ((numRead = read(inputFile, buf, BUF_SIZE)) > 0) {
    if (numRead == -1) {
      errExit("error reading from input file");
    }
    int numWrite = write(outputFile, buf, numRead);
    if (numWrite == -1) {
      errExit("error writing to file");
    } else if (numWrite != numRead) {
      fatal("failed to write whole buffer");
    }
  }

  if (close(inputFile) == -1) {
    errExit("closing input");
  }

  if (close(outputFile) == -1) {
    errExit("closing output");
  }

  exit(EXIT_SUCCESS);
}
