#pragma once

#include <stdbool.h>
#include <stdlib.h>

struct substring {
	const char* source;
	size_t start;
	size_t end;
};

struct substring substring_create(const char*, size_t start, size_t end);
int substring_cmp(struct substring, const char*);
