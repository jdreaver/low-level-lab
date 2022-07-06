TARGETS = hello print_rax print_call endianness strlen test_lib
OBJ_FILES = $(patsubst %,%.o,$(TARGETS))
OBJ_FILES += lib.o

.PHONY: all
all: $(TARGETS)

$(TARGETS): %: %.o lib.o
	ld -o $@ $^

$(OBJ_FILES): %.o: %.asm
	yasm -f elf64 -g dwarf2 $? -l $?.lst -o $@

.PHONY: clean
clean:
	rm -f $(TARGETS) $(OBJ_FILES) *.lst
