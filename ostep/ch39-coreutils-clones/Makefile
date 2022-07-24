TARGETS = mystat myls
SRC = $(wildcard *.c)
OBJS = $(patsubst %.c,%.o,$(SRC))
DEPS = $(patsubst %.c,%.d,$(SRC))
INCLUDES = $(wildcard *.h)

CC = gcc
LD = gcc
CFLAGS = -Wall -Wextra -pedantic -Wshadow -fanalyzer -flto
LDFLAGS =

# Run like "make DEBUG=0 ..." to disable debugging
DEBUG ?= 1
ifeq ($(DEBUG), 1)
  CFLAGS += -g3 -ggdb3 -DDEBUG -O0

  # N.B. This undefines the _FORTIFY_SOURCE macro, which triggers a warning deep
  # in some header file because we are using -O0
  CFLAGS += -U_FORTIFY_SOURCE

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
all: $(TARGETS)

$(TARGETS): %: %.o
	$(LD) $(LDFLAGS) -o $@ $^

$(OBJS): %.o: %.c # N.B. other deps specified because of -MMD -MP flags
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

.PHONY: compile_commands
compile_commands:
	$(MAKE) clean
	bear -- $(MAKE)

.PHONY: clean
clean:
	rm -f $(TARGETS) $(OBJS) $(DEPS) $(COMPILE_COMMANDS)

# Include all of the dependency files generated from -MD and -MP. Purposely at
# the bottom of this Makefile so these don't supersede the default target on
# accident.
-include $(DEPS)
