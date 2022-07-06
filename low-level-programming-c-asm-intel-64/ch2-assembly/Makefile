TARGETS = hello
TARGETS_O = $(patsubst %,%.o,$(TARGETS))

.PHONY: all
all: $(TARGETS)

$(TARGETS): %: %.o
	ld -o $@ $?

$(TARGETS_O): %.o: %.asm
	yasm -f elf64 -g dwarf2 $? -l $?.lst -o $@

.PHONY: clean
clean:
	rm -f $(TARGETS) $(TARGETS_O) *.lst
