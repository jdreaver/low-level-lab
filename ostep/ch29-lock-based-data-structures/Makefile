TARGET = main
SRC = $(wildcard *.c)
OBJS = $(patsubst %.c,%.o,$(SRC))
DEPS = $(patsubst %.c,%.d,$(SRC))
INCLUDES = $(wildcard *.h)

CC = gcc
LD = gcc
CFLAGS = -Wall -Wextra -pedantic -Wshadow -fanalyzer
# Enabling link-time optimization speeds up the nonatomic_counter _massively_.
# GCC basically inlines everything and precomputes what the counter should be,
# so we even get an accurate count most of the time.
# CFLAGS += -flto
LDFLAGS = -no-pie -pthread

# Run like "make DEBUG=0 ..." to disable debugging
DEBUG ?= 1
ifeq ($(DEBUG), 1)
  CFLAGS += -g3 -ggdb3 -DDEBUG -O0

  # N.B. fsanitize options need to be present in both compile flags and linker
  # flags
  CFLAGS += -fsanitize=address,leak,undefined -fsanitize-undefined-trap-on-error
  LDFLAGS += -fsanitize=address,leak,undefined -fsanitize-undefined-trap-on-error

  # thread sanitizer is fun, but can't be combined with some other sanitizers
  # CFLAGS += -fsanitize=thread
  # LDFLAGS += -fsanitize=thread
else
  CFLAGS += -O2
endif

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
