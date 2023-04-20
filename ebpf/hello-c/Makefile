.DEFAULT_GOAL := run
.PHONY: run
run: hello.bpf.o
	sudo ./run.sh

hello.bpf.o: %.o: %.c
	clang \
	  -target bpf \
	  -g \
	  -O2 -c $< -o $@

.PHONY: clean
clean:
	rm -f hello.bpf.o
