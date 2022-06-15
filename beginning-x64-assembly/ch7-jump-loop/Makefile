TARGETS=jump
TARGETS_O=$(patsubst %,%.o,$(TARGETS))

.PHONY: all
all: $(TARGETS)

$(TARGETS): %: %.o
	gcc -o $@ -nostartfiles --entry main $?

$(TARGETS_O): %.o: %.S
	gcc -c $? -o $@ -ggdb

.PHONY: clean
clean:
	rm -f $(TARGETS) $(TARGETS_O)
