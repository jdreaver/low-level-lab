        .data
bNum:   .byte   123
wNum:   .word   12345
warray: .word   0,0,0,0,0

dNum:   .long   12345
qNum1:  .quad   12345
text1:  .asciz  "abc"
qNum2:  .float  3.141592654
text2:  .asciz  "cde"
        .bss
.lcomm  bvar    1
.lcomm  dvar    4
.lcomm  wvar    10 * 2
        .text
        .global main
main:
        lea     (bNum), %rax    # load address of bNum into rax
        mov     $bNum, %rax     # load address of bNum into rax
        mov     (bNum), %rax    # load value of bNum into rax, but this writes 4 bytes! (includes wNum and a 0x00)

        xor     %rax, %rax      # properly clear rax first
        mov     (bNum), %al     # load bNum into rax

        mov     %rax, (bvar)    # load rax into bvar
        lea     (bvar), %rax    # load address of bvar into rax

        lea     (wNum), %rax    # load address of wNum into rax
        mov     (wNum), %rax    # load value of wNum into rax

        mov     $60, %rax       # 60 = exit
        mov     $0, %rdi        # 0 = success
        syscall                 # quit
