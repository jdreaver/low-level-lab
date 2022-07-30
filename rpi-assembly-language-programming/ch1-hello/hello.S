.text

.global _start
_start: mov     R0, #1          @ 1 = stdout
        ldr     R1, =helloworld @ string to print
        mov     R2, #13         @ length of our string
        mov     R7, #4          @ write syscall number
        svc     0               @ call syscall

        mov     R0, #0          @ Use 0 for return code
        mov     R7, #1          @ Exit syscall number
        svc     0

.data
helloworld:     .ascii  "Hello World!\n"
