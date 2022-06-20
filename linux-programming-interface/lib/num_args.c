#include<errno.h>
#include <limits.h>
#include<stdio.h>
#include<stdlib.h>

#include "error_functions.h"

long getLong(const char *arg) {
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

int getInt(const char *arg) {
	long num = getLong(arg);
	if (num > INT_MAX || num < INT_MIN) {
		char buf[100];
		snprintf(buf, 100, "getInt integer out of range: %ld", num);
		fatal(buf);
	}

	return (int) num;
}
