; These macrodefinitions are copied from linux sources Linux is written in C, so
; the definitions looked a bit different there. We could have just looked up
; their values and use them directly in right places However it would have made
; the code much less legible

%define O_RDONLY 0
%define PROT_READ 0x1
%define MAP_PRIVATE 0x2

section .data
fname:
        db	"test.txt", 0

section .text

global _start
_start:
        ; Open the test file
        mov	rax, 2          ; open syscall number
        mov	rdi, fname      ; arg #1: file path
        mov	rsi, O_RDONLY   ; arg #2: flags
        mov	rdx, 0          ; arg #3: file mode (not creating, so not used)
        syscall

        ; mmap test file
        mov	r8, rax         ; arg #4: file descriptor
        mov	rax, 9          ; mmap syscall number
        mov	rdi, 0          ; Address (0 means let OS choose)
        mov	rsi, 4096       ; Page size
        mov	rdx, PROT_READ  ; memory protection (PROT_READ is read-only)
        mov	r10, MAP_PRIVATE ; pages will not be shared
        mov	r9, 0            ; offset within file
        syscall

        ; Print string
        mov	rdi, rax
        call	print_string

        ; Exit
        mov	rax, 60
        xor	rdx, rdx
        syscall

; These functions are used to print a null terminated string
print_string:
        push rdi
        call string_length
        pop rsi
        mov rdx, rax
        mov rax, 1
        mov rdi, 1
        syscall
        ret
string_length:
        xor rax, rax
.loop:
        cmp byte [rdi+rax], 0
        je .end
        inc rax
        jmp .loop
.end:
        ret
