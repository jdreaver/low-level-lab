.PHONY: all
all: hello
	file hello
	aarch64-unknown-linux-gnu-readelf -a hello

# Cross-compile using toolchain
hello: hello.c
	aarch64-unknown-linux-gnu-gcc -static -o $@ $<

.PHONY: clean
clean:
	rm -f hello
