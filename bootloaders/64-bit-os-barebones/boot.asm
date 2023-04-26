; The BIOS loads the first 512 bytes of the disk into memory at address 0x7C00.
; We have to tell NASM about this so it knows to offset any addresses by 0x7C00.
[org 0x7C00]

; All x86-type chips start in 16-bit "real mode", which
; enables backwards compatibility all the way to the Intel 8086 chip.
; Yes, even the 64-bit variants have this ability, it's just not
; commonly used. So, we'll need to tell NASM to generate 16 bit instructions.
[BITS 16]

; Print 'Hello'
;
; When in BIOS (https://wiki.osdev.org/BIOS), we can BIOS functions to perform
; certain actions. These functions are activated with interrupts. You set the
; values of certain registers to select which function to use and to pass args.
;
; The BIOS function to print a character is 0x10. The subfunction to print a
; character is 0x0e. The character to print is passed in the al register.

mov bx, hello
call print_bios

mov bx, goodbye
call print_bios

jmp loop

; Define function print_bios
; Input pointer to string in bx
print_bios:
        ; We'll be using registers ax and bx, so we'll need to save their
        ; current values on the stack so we don't mess anything up once the
        ; function is complete. We use the 'push' and 'pop' instructions for
        ; this.
        push ax
        push bx

        ; The BIOS function to print a character is 0x10. The subfunction to
        ; print a character is 0x0e. The character to print is passed in the al
        ; register.
        mov ah, 0x0e

print_bios_loop:
        mov al, [bx]            ; Load the next char to print in al
        cmp al, 0               ; Check if the char is null
        je print_bios_done      ; If it is, we're done
        int 0x10                ; Otherwise, print the char
        inc bx                  ; Increment the pointer to the next char
        jmp print_bios_loop     ; Loop back to the beginning

print_bios_done:
        ; Restore the values of ax and bx registers.
        ; order.
        pop bx
        pop ax

        ; This instruction returns to the instruction after the initial 'call'
        ; instruction that got us here. It works almost the same as 'return' in
        ; C, but you can't give it a value.
        ret

; A simple boot sector program that loops forever
loop:
    jmp loop

; Data
hello:
    db `Hello, World!\r\n`, 0

goodbye:
    db `Goodbye!\r\n`, 0

; The magic number must come at the end of the boot
; sector. We need to fill the space with zeros to ensure that
; the magic number comes at the end. Remember, since the magic
; number is 2 bytes, the rest of the boot sector is 510.

; We use the 'times' command to repeat the 'db' or "define byte" command
; to pad the rest. Remeber, we will use the '$' and '$$' commands to
; determine how much space is left between the final instruction and
; the magic number.
times 510-($-$$) db 0

; The 'dw' command stands for "define word", and puts
; a 16-bit literal integer at that specific location in the file.
; We use 'dw' here to define the magic number, 'AA55'
dw 0xaa55
