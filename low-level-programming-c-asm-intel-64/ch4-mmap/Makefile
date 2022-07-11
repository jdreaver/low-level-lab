YASM_TARGETS = mmap
YASM_OBJ_FILES = $(patsubst %,%.o,$(YASM_TARGETS))

GAS_TARGETS = mmap-gas
GAS_OBJ_FILES = $(patsubst %,%.o,$(GAS_TARGETS))

.PHONY: all
all: $(YASM_TARGETS) $(GAS_TARGETS)

$(YASM_TARGETS): %: %.o
	ld -o $@ $^

$(YASM_OBJ_FILES): %.o: %.asm
	yasm -f elf64 -g dwarf2 $? -l $?.lst -o $@

$(GAS_TARGETS): %: %.o
	gcc -nostdlib -o $@ $^

$(GAS_OBJ_FILES): %.o: %.S
	gcc -ggdb3 -c $? -o $@

.PHONY: clean
clean:
	rm -f $(YASM_TARGETS) $(YASM_OBJ_FILES) $(GAS_TARGETS) $(GAS_OBJ_FILES) *.lst
