section .data
codes:
        db	'0123456789ABCDEF'
newline:
        db	0xA
section .text
global _start
_start:
        ; Store input in rax
        mov	rax, 0x8C12345349ABFEF

        ; Store some arguments to write() syscall in proper registers ahead of
        ; time so we don't have to repeat them in the loop.
        mov	rdi, 1          ; arg #1 in rdi: where to write? 1 for stdout
        mov	rdx, 1          ; arg #3 in rdx: how many bytes to write?

        mov	rcx, 64         ; Loop counter. We will count down from 64 to 0,
                                ; and use cl (sub-register for rcx) for
                                ; bit-shifting the input.

.loop:
        ; In this loop, we will shift our input rax by one hexadecimal digit (4
        ; bits) every iteration and then bitwise AND the value so we can index
        ; into the codes array to print the value.
        push	rax             ; Save rax. We need to modify it later for the
                                ; syscall
        sub	rcx, 4
        ; cl is the smallest part of rcx
        ; rcx -- ecx -- cx -- ch + cl
        sar	rax, cl
        and	rax, 0xf        ; Isolate last 4 bits

        ; Set up write() syscall (remember rdi and rdx set above)
        lea	rsi, [codes + rax] ; write() arg #2: what to write
        mov	rax, 1             ; write syscall number

        push	rcx             ; write changes rcx and r11, so save rcx
        syscall
        pop	rcx

        pop	rax             ; Restore our original input rax
        test	rcx, rcx        ; test is a fast "is zero?" method
        jnz	.loop

        ; Print a newline (rdi/rdx set above)
        mov	rax, 1
        mov	rsi, newline
        syscall

        ; Invoke exit syscall
        mov	rax, 60
        xor	rdi, rdi
        syscall
