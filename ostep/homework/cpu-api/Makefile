TARGETS=p4 fork-shared fork-write-shared
TARGETS_O=$(patsubst %,%.o,$(TARGETS))

CFLAGS=-Wall -Werror -pedantic

.PHONY: all
all: $(TARGETS)

$(TARGETS): %: %.o
	gcc $(CFLAGS) -o $@ $?

$(TARGETS_O): %.o: %.c
	gcc $(CFLAGS) -c $? -o $@ -ggdb3

.PHONY: clean
clean:
	rm -f $(TARGETS) $(TARGETS_O)
