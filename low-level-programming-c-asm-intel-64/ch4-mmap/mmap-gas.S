# These macrodefinitions are copied from linux sources Linux is written in C, so
# the definitions looked a bit different there. We could have just looked up
# their values and use them directly in right places However it would have made
# the code much less legible

.set O_RDONLY, 0
.set PROT_READ, 0x1
.set MAP_PRIVATE, 0x2

.data
fname:  .asciz  "test.txt"

.text

.global _start
_start:
        # Open the test file
        mov     $2, %rax        # open syscall number
        mov     $fname, %rdi    # arg #1: file path
        mov     $O_RDONLY, %rsi # arg #2: flags
        mov     $0, %rdx        # arg #3: file mode (not creating, so not used)
        syscall

        # mmap test file
        mov     %rax, %r8       # arg #4: file descriptor
        mov     $9, %rax        # mmap syscall number
        mov     $0, %rdi        # Address (0 means let OS choose)
        mov     $4096, %rsi     # Page size
        mov     $PROT_READ, %rdx # memory protection (PROT_READ is read-only)
        mov     $MAP_PRIVATE, %r10 # pages will not be shared
        mov     $0, %r9         # offset within file
        syscall

        # Print string
        mov     %rax, %rdi
        call    print_string

        # Exit
        mov     $60, %rax
        xor     %rdx, %rdx
        syscall

# These functions are used to print a null terminated string
print_string:
        push    %rdi
        call    string_length
        pop     %rsi
        mov     %rax, %rdx
        mov     $1, %rax
        mov     $1, %rdi
        syscall
        ret

string_length:
        xor     %rax, %rax
string_length.loop:
        cmpb    $0, (%rdi, %rax)
        je      string_length.end
        inc     %rax
        jmp     string_length.loop
string_length.end:
        ret
