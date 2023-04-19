#!/usr/bin/env python

# N.B. To run on NixOS with a shell, we need to run with sudo via:
# $ sudo --preserve-env env PATH="$PATH" PYTHONPATH="$PYTHONPATH" ./hello.py

from bcc import BPF

program = r"""
int hello(void *ctx) {
    bpf_trace_printk("Hello World!");
    return 0;
}
"""

b = BPF(text=program)
syscall = b.get_syscall_fnname("execve")
b.attach_kprobe(event=syscall, fn_name="hello")
b.trace_print()
