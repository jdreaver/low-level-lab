        .data
msg1:   .asciz  "Hello, World!"
msg2:   .asciz  "Alive and Kicking!"
radius: .long   357
pi:     .double 3.14
fmtstr: .asciz  "%s\n"
fmtflt: .asciz  "%lf\n"
fmtint: .asciz  "%d\n"

        .text

        .global main
main:
        ## print msg1
        mov     $0,%rax         # no floating point
        mov     $fmtstr,%rdi
        mov     $msg1,%rsi
        call    printf

        ## print msg2
        mov     $0,%rax         # no floating point
        mov     $fmtstr,%rdi
        mov     $msg2,%rsi
        call    printf

        ## print radius
        mov     $0,%rax         # no floating point
        mov     $fmtint,%rdi
        mov     (radius),%rsi
        call    printf

        ## print pi
        mov     $1,%rax         # 1 xmm register used
        movq    (pi),%xmm0
        mov     $fmtflt,%rdi
        call    printf

        mov     $60, %rax       # 60 = exit
        mov     $0, %rdi        # 0 = success
        syscall                 # quit
