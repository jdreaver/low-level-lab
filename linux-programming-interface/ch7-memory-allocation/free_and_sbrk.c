#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "lib.h"

#define MAX_ALLOCS 1000000

int main(int argc, char *argv[])
{
	if (argc < 3 || strcmp(argv[1], "--help") == 0) {
		usageErr("%s num-allocs block-size [step [min [max]]]\n", argv[0]);
	}

	int numAllocs = getInt(argv[1]);
	int blockSize = getInt(argv[2]);
	int freeStep = (argc > 3) ? getInt(argv[3]) : 1;
        int freeMin = (argc > 4) ? getInt(argv[4]) : 1;
        int freeMax = (argc > 5) ? getInt(argv[5]) : numAllocs;

	if (numAllocs > MAX_ALLOCS) {
		char buf[100];
		snprintf(buf, 100, "num-allocs > MAX_ALLOCS (%d > %d)", numAllocs, MAX_ALLOCS);
		fatal(buf);
	}

	if (freeMax > numAllocs) {
		char buf[100];
		snprintf(buf, 100, "max > num-allocs (%d > %d)", freeMax, numAllocs);
		fatal(buf);
	}

	printf("Initial program break: %10p\n", sbrk(0));

	printf("Allocating %d*%d bytes\n", numAllocs, blockSize);
	char *allocs[MAX_ALLOCS];
	for (int i = 0; i < numAllocs; i++) {
		allocs[i] = malloc(blockSize);
		if (allocs[i] == NULL) {
			errExit("malloc error");
		}
	}

	printf("Break is now:          %10p\n", sbrk(0));

	printf("Freeing blocks from %d to %d in steps of %d\n", freeMin, freeMax, freeStep);
	for (int i = freeMin - 1; i < freeMax; i += freeStep) {
		free(allocs[i]);
	}
	printf("After free(), program break is: %10p\n", sbrk(0));

	exit(EXIT_SUCCESS);
}
