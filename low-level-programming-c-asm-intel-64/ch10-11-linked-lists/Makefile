TARGET = main
SRC = $(wildcard *.c)
OBJS = $(patsubst %.c,%.o,$(SRC))
DEPS = $(patsubst %.c,%.d,$(SRC))
INCLUDES = $(wildcard *.h)

TEST_INPUTS = $(wildcard tests/*.in)

CC = gcc
LD = gcc
CFLAGS = -Wall -Wextra -pedantic -no-pie -g3 -ggdb3 -DDEBUG -O0
LDFLAGS = -no-pie

# Automatically generate dependencies. -MMD generates non-system header
# dependencies. -MP creates an empty rule for generating each dependency. See
# http://www.microhowto.info/howto/automatically_generate_makefile_dependencies.html
CPPFLAGS+=-MMD -MP

.PHONY: all
all: $(TARGET)

$(TARGET): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^

$(OBJS): %.o: %.c # N.B. other deps specified because of -MMD -MP flags
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

.PHONY: test
test: $(TARGET)
	@./run-tests.sh

.PHONY: compile_commands
compile_commands:
	$(MAKE) clean
	bear -- $(MAKE)
  # We need to tell bear to exclude compiler wrappers, or we'll have duplicate,
  # incorrect entries in compile_commands.json. See:
  # - https://github.com/rizsotto/Bear/wiki/Troubleshooting#the-output-has-duplicate-entries
  # - https://github.com/NixOS/nixpkgs/issues/20056
  # - https://github.com/rizsotto/Bear/issues/321
  #
  # An alternative solution is to use the config file to pass in the compiler
  # wrapper under compilation.compilers_to_exclude.
	jq 'map(select(.arguments[0] | contains("wrapper") | not))' compile_commands.json > tmp_compile_commands.json
	mv tmp_compile_commands.json compile_commands.json

.PHONY: clean
clean:
	rm -f $(TARGET) $(OBJS) $(DEPS) $(COMPILE_COMMANDS)

# Include all of the dependency files generated from -MD and -MP. Purposely at
# the bottom of this Makefile so these don't supersede the default target on
# accident.
-include $(DEPS)
