TARGET = main
SRC = $(wildcard *.c)
OBJS = $(patsubst %.c,%.o,$(SRC))
DEPS = $(patsubst %.c,%.d,$(SRC))
INCLUDES = $(wildcard *.h)

TEST_INPUTS = $(wildcard tests/*.in)

CC = gcc
LD = gcc
CFLAGS = -Wall -Wextra -pedantic -no-pie -g3 -ggdb3 -DDEBUG
LDFLAGS = -no-pie

# Automatically generate dependencies. -MMD generates non-system header
# dependencies. -MP creates an empty rule for generating each dependency. See
# http://www.microhowto.info/howto/automatically_generate_makefile_dependencies.html
CPPFLAGS+=-MMD -MP

COMPILE_COMMANDS = compile_commands.json

.PHONY: all
all: $(TARGET)

$(TARGET): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^

$(OBJS): %.o: %.c # N.B. other deps specified because of -MMD -MP flags
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

.PHONY: test
test: $(TARGET)
	@./run-tests.sh

$(COMPILE_COMMANDS):
	$(MAKE) clean
	bear -- $(MAKE)

.PHONY: clean
clean:
	rm -f $(TARGET) $(OBJS) $(DEPS) $(COMPILE_COMMANDS)

# Include all of the dependency files generated from -MD and -MP. Purposely at
# the bottom of this Makefile so these don't supersede the default target on
# accident.
-include $(DEPS)
