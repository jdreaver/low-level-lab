TARGET = main
SRC = $(wildcard *.c)
OBJS = $(patsubst %.c,%.o,$(SRC))
INCLUDES = $(wildcard *.h)

TEST_INPUTS = $(wildcard tests/*.in)

CC = gcc
LD = gcc
CFLAGS = -Wall -Wextra -pedantic -no-pie -g3 -ggdb3 -DDEBUG
LDFLAGS = -no-pie

.PHONY: all
all: $(TARGET)

$(TARGET): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^

$(OBJS): %.o: %.c $(INCLUDES) # N.B. Depending on all includes sucks, but whatever
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: test
test: $(TARGET)
	@./run-tests.sh

.PHONY: clean
clean:
	rm -f $(TARGET) $(OBJS)
