#include<ctype.h>
#include<errno.h>
#include<fcntl.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>

#include "error_functions.h"
#include "num_args.h"

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
  long len, offset;
  char *buf;
  int numWritten;
  for (int argi = 2; argi < argc; argi++) {
    char *op = argv[argi];
    char opType = op[0];
    char *opArg = &op[1];

    switch (opType) {
    case 'r': /* Display bytes at current offset as text */
    case 'R': /* Display bytes at current offset as hex */
      len = getLong(opArg);

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

    case 'w':
      numWritten = write(file, opArg, strlen(opArg));
      if (numWritten == -1) {
	errExit("error writing to file");
      }
      printf("%s: wrote %ld bytes\n", op, (long) numWritten);
      break;

    case 's':
      offset = getLong(opArg);
      if (lseek(file, offset, SEEK_SET) == -1) {
	errExit("error seeking in file");
      }
      printf("%s: seek succeeded\n", op);
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
