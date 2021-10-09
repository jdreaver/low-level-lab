#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/param.h>
#include <unistd.h>

#include "lib.h"
#include "lib.h"

#define BUF_SIZE 4096

// TODO:
// - Test on file exactly BUF_SIZE
// - If the file ends in a newline, ignore that first newline

int main(int argc, char *argv[]) {
	opterr = 0; // Global variable that tells getopt not to print error messages

	// Parse options
	char c;
	int num_lines = 10; // Default to 10 lines
	int helpFlag = 0;
	while ((c = getopt (argc, argv, "hn:")) != -1) {
		switch (c) {
		case 'h':
			helpFlag = 1;
			break;
		case 'n':
			num_lines = getInt(optarg);
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
		usageErr("%s FILE [-n lines]\n", argv[0]);
	}

	// Open input file
	char *path = argv[optind];
	int file = open(path, O_RDONLY);
	if (file == -1) {
		errExit("open() on input file");
	}

	// Seek to end of file
	off_t current_offset = lseek(file, 0, SEEK_END);
	if (current_offset == -1) {
		errExit("lseek to end of file");
	}

	// Continuously read starting from the end of the file until
	// we either hit the given number of lines, or until we read
	// the whole file.
	int lines_left = num_lines;
	char buf[BUF_SIZE];
	while (lines_left > 0 && current_offset > 0) {
		// Compute where to lseek to
		off_t read_start = MAX(0, current_offset - BUF_SIZE);
		int bytes_to_read = current_offset - read_start;
		current_offset -= bytes_to_read;
		off_t lseek_ret = lseek(file, current_offset, SEEK_SET);
		if (lseek_ret == -1) {
			errExit("lseek in loop");
		}

		// Read next chunk of file into buffer
		int num_read = read(file, buf, bytes_to_read);
		if (num_read != bytes_to_read) {
			char err_buf[1000];
			snprintf(err_buf, 1000, "reading from %d bytes starting at %ld", bytes_to_read, read_start);
			errExit(err_buf);
		}

		// Scan buffer until we see the desired number of
		// newlines, or until we scan the whole buffer.
		int write_start;
		for (write_start = num_read - 1; write_start >= 0; write_start--) {
			if (buf[write_start] == '\n') {
				lines_left--;
			}
			if (lines_left == 0) {
				write_start++; // Don't write that last newline
				current_offset += write_start; // Reset current_offset to write_start (relative)
				break;
			}
		}
	}

	// Reset file offset to undo last read
	off_t lseek_ret = lseek(file, current_offset, SEEK_SET);
	if (lseek_ret == -1) {
		errExit("lseek to reset current offset");
	}

	// Write from current_offset to the end of the file
	int num_read;
	while ((num_read = read(file, buf, BUF_SIZE)) > 0) {
		if (num_read == -1) {
			errExit("error reading from input file");
		}
		int num_written = write(STDOUT_FILENO, buf, num_read);
		if (num_written == -1) {
			errExit("error writing to file");
		} else if (num_written != num_read) {
			fatal("failed to write whole buffer");
		}
	}

	if (close(file) == -1) {
		errExit("failed to close output file");
	}
}
