#!/usr/bin/env bash

# See Chapter 3 of "Learning eBPF"

set -eux

NETWORK_INTERFACE=wlp5s0

# Inspect the new file
file hello.bpf.o
llvm-objdump -S hello.bpf.o

# Load into the kernel
bpftool prog load hello.bpf.o /sys/fs/bpf/hello
ls /sys/fs/bpf/
bpftool prog list | grep -A3 'name hello'
bpftool prog show name hello --pretty

# Dump bytecode
bpftool prog dump xlated name hello
bpftool prog dump jited name hello

# Attach to network interface
bpftool net attach xdp name hello dev "$NETWORK_INTERFACE"
bpftool net list
ip link show "$NETWORK_INTERFACE"

# View output (cat is better but doesn't terminate, just use head)
sleep 3
head /sys/kernel/debug/tracing/trace_pipe

# More stuff to look at
bpftool map list | grep -A2 hello
bpftool map dump name hello.bss
bpftool map dump name hello.rodata

# Shut it down
bpftool net detach xdp dev "$NETWORK_INTERFACE"
rm /sys/fs/bpf/hello
