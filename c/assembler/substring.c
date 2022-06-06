#include <string.h>

#include "substring.h"

struct substring substring_create(const char* source, size_t start, size_t end)
{
	struct substring s = {.source = source, .start = start, .end = end};
	return s;
}

int substring_cmp(struct substring s, const char* other)
{
	size_t substring_len = s.end - s.start;

	// Empty substrings are weird and don't really work in strcmp. If our
	// substring has a length of 0, and the other string is just "", then
	// return 0.
	if (substring_len == 0) {
		return other[0] != '\0';
	}

	const char *substring_start = s.source + s.start;
	return strncmp(substring_start, other, substring_len);
}
