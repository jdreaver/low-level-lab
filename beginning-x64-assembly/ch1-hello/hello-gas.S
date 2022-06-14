# Version of hello.asm using gcc GAS syntax, to hopefully get around gdb
# weirdness.
        .data
msg:    .ascii  "Hello, world"

        .global main

        .text
main:
        mov     $1, %rax        # 1 = write
        mov     $1, %rdi        # 1 = to stdout
        mov     $msg, %rsi      # string to display in rsi
        mov     $12, %rdx       # length of the string, without 0
        syscall                 # display the string
        mov     $60, %rax       # 60 = exit
        mov     $0, %rdi        # 0 = success
        syscall                 # quit
