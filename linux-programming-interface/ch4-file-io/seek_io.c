#include<ctype.h>
#include<errno.h>
#include<fcntl.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>

#include "error_functions.h"

static long getNum(const char *arg) {
  // Buffer for errors
  #define BUF_SIZE 500
  char buf[BUF_SIZE];

  if (arg == NULL || *arg == '\0') {
    fatal("getNum error: null or empty string");
  }

  errno = 0;
  int base = 10;
  char *endptr;
  long res = strtol(arg, &endptr, base);

  if (errno != 0) {
    snprintf(buf, BUF_SIZE, "strtol() failed on text %s", arg);
    errExit(buf);
  }

  if (*endptr != '\0') {
    snprintf(buf, BUF_SIZE, "ERROR nonnumeric characters in text %s", arg);
    fatal(buf);
  }

  return res;
}

int main(int argc, char *argv[]) {
  if (argc < 3 || strcmp(argv[1], "--help") == 0) {
    usageErr("%s file {r<length>|R<length>|w<string>|s<offset>}...\n", argv[0]);
  }

  char *path = argv[1];
  int openFlags = O_RDWR | O_CREAT;
  mode_t filePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH; /* rw-rw-rw- */
  int file = open(path, openFlags, filePerms);
  if (file == -1) {
    char buf[500];
    snprintf(buf, 500, "failed to open %s", path);
    errExit(buf);
  }

  char errorBuf[500]; // For printing errors
  long len;
  char *buf;
  for (int argi = 2; argi < argc; argi++) {
    char *op = argv[argi];
    char opType = op[0];
    char *opArg = &op[1];

    switch (opType) {
    case 'r': /* Display bytes at current offset as text */
    case 'R': /* Display bytes at current offset as hex */
      len = getNum(opArg);

      buf = malloc(len);
      if (buf == NULL) {
	errExit("malloc error");
      }

      int numRead = read(file, buf, len);
      if (numRead == -1) {
	errExit("error reading file into buffer");
      } else if (numRead == 0) {
	printf("%s: end-of-file\n", op);
      } else {
	printf("%s: ", op);
	for (int j = 0; j < numRead; j++) {
	  if (opType == 'r') {
	    if (buf[j] == '\n') {
	      printf("\\n");
	    } else {
	      printf("%c", isprint((unsigned char) buf[j]) ? buf[j] : '?');
	    }
	  } else {
	    printf("%02x ", (unsigned char) buf[j]);
	  }
	}
	printf("\n");
      }
      free(buf);
      break;
    default:
      snprintf(errorBuf, 500, "failed to open %s", path);
      fatal(errorBuf);
    }
  }

  if (close(file) == -1) {
    errExit("closing input file");
  }
}
