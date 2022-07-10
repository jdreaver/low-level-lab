TARGET = main
SRC = $(wildcard *.c)
OBJS = $(patsubst %.c,%.o,$(SRC))
INCLUDES = $(wildcard *.h)

CC = gcc
LD = gcc
CFLAGS = -Wall -Wextra -pedantic -no-pie -g3 -ggdb3 -DDEBUG -O0
LDFLAGS = -no-pie

.PHONY: all
all: $(TARGET)

$(TARGET): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^

$(OBJS): %.o: %.c $(INCLUDES) # N.B. Depending on all includes sucks, but whatever
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: clean
clean:
	rm -f $(TARGET) $(OBJS)
