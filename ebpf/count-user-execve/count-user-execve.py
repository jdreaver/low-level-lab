#!/usr/bin/env python

# N.B. To run on NixOS with a shell, we need to run with sudo via:
# $ sudo --preserve-env env PATH="$PATH" PYTHONPATH="$PYTHONPATH" ./count-user-execve.py

from bcc import BPF
import time

# This program counts the number of execve() calls per user ID. It uses the
# BPF_HASH() table to store the counts, and the Python wrapper prints them.
program = r"""
BPF_HASH(counter_table);

int hello(void *ctx) {
  u64 uid;
  u64 counter = 0;
  u64 *p;

  uid = bpf_get_current_uid_gid() & 0xFFFFFFFF;
  p = counter_table.lookup(&uid);
  if (p != 0) {
    counter = *p;
  }
  counter++;
  counter_table.update(&uid, &counter);
  return 0;
}
"""

b = BPF(text=program)
syscall = b.get_syscall_fnname("execve")
b.attach_kprobe(event=syscall, fn_name="hello")

while True:
    time.sleep(2)
    for k, v in b["counter_table"].items():
        print("ID: %d, %d\t" % (k.value, v.value), end='')
    print("")
