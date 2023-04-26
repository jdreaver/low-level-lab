; The BIOS loads the first 512 bytes of the disk into memory at address 0x7C00.
; We have to tell NASM about this so it knows to offset any addresses by 0x7C00.
[org 0x7C00]

; All x86-type chips start in 16-bit "real mode", which
; enables backwards compatibility all the way to the Intel 8086 chip.
; Yes, even the 64-bit variants have this ability, it's just not
; commonly used. So, we'll need to tell NASM to generate 16 bit instructions.
[BITS 16]

; Initialize the base pointer and the stack pointer. Set to 0x7C00, which is
; where our BIOS is loaded. The space below this should be free, and the stack
; grows down.
mov bp, 0x07C00
mov sp, bp

; Before we do anything else, we want to save the ID of the boot
; drive, which the BIOS stores in register dl. We can offload this
; to a specific location in memory
mov byte[boot_drive], dl

; Load the next sector
mov bx, 0x7C00 + 512            ; Destination is boot sector destination + 512 bytes
mov cl, 0x02                    ; Start at sector 2 (sectors are 1-based)
mov dl, 0x01                    ; Load 1 sector
call bios_load

; Print some stuff
mov bx, hello
call print_bios

mov bx, bootsector_extended_msg
call print_bios

mov dx, 0x8934
call print_hex_bios

; Loop forever so we can see output
loop:
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

        ; When in BIOS (https://wiki.osdev.org/BIOS), we can BIOS functions to
        ; perform certain actions. These functions are activated with
        ; interrupts. You set the values of certain registers to select which
        ; function to use and to pass args.
        ;
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


; Define print_hex_bios
; The number to print is in the dx register
print_hex_bios:
        ; Save registers we will use
        push ax
        push bx
        push cx

        ; The BIOS function to print a character is 0x10. The subfunction to
        ; print a character is 0x0e. The character to print is passed in the al
        ; register.
        mov ah, 0x0e

        ; Print the "0x" prefix
        mov al, '0'
        int 0x10
        mov al, 'x'
        int 0x10

        ; Set number of digits to print
        mov cx, 4

print_hex_bios_loop:
        cmp cx, 0               ; Check if we've printed all the digits
        je print_hex_bios_end   ; If we have, we're done
        dec cx                  ; Decrement the digit counter

        ; Rotate the number left so the most significant nibble is in the
        ; least significant position
        rol dx, 4
        mov bx, dx              ; Copy least significant nibble to bx
        and bx, 0x0f            ; Mask out all but the least significant nibble
        add bx, '0'             ; Convert to ASCII
        cmp bx, '9'             ; Check if the nibble is greater than 9
        jle print_hex_bios_print_char ; If it's not, it's a number
        add bx, 'A' - '9'       ; Otherwise, it's a letter ('A' is 0x40 in ASCII and '9' is 0x39)

print_hex_bios_print_char:
        mov al, bl              ; Move the char to print into al
        int 0x10                ; Print the char
        jmp print_hex_bios_loop ; Loop back to the beginning

print_hex_bios_end:
        ; Restore registers
        pop cx
        pop bx
        pop ax

        ret

; BIOS load function to read more sectors than just the boot sector into memory.
; See:
; - https://web.archive.org/web/20201021171813/https://wiki.osdev.org/ATA_in_x86_RealMode_%28BIOS%29
; - https://github.com/gmarino2048/64bit-os-tutorial/tree/master/Chapter%201/1.3%20-%20Load%20More%20Code
;
; Args:
;   bx: destination buffer (only uses bl)
;   cl: start sector (ch is overwritten). Also remember sectors are 1-based
;   dl: number of sectors to load
bios_load:
        push dx                 ; Save number of sectors to read

        ; For now, we only need to load a few sectors, so we can assume we are
        ; on the 0th cylinder and the 0th head.
        mov ah, 0x02            ; BIOS function to read sectors
        mov al, dl              ; Number of sectors to read
        ; bl contains the destination address
        mov ch, 0x00            ; Cylinder number
        ; cl contains the sector number
        mov dh, 0x00            ; Head number
        mov dl, [boot_drive]    ; Boot drive number
        int 0x13                ; Call BIOS interrupt

        ; If there's a read error, then the BIOS function will set the 'carry'
        ; bit in the 8086 special register. We can use the 'jc' command to 'jump
        ; if the carry bit is set'. We'll jump to the error handling function
        ; defined below
        jc bios_load_disk_error

        pop dx                ; get back original number of sectors to read
        cmp al, dl            ; BIOS sets 'al' to the # of sectors actually read
        jne bios_load_sectors_error   ; compare it to 'dl' and error out if they are !=

        ; We already popped dx above
        ret

bios_load_disk_error:
        mov bx, bios_disk_error_string
        call print_bios
        jmp loop

bios_load_sectors_error:
        mov bx, bios_sector_error_string
        call print_bios
        jmp loop

; Data

; Boot drive storage. The BIOS stores the boot drive in register dl. The first
; thing we do is save it to this location in memory.
boot_drive:
        db 0x00

hello:
        db `Hello, World!\r\n`, 0

bios_disk_error_string:
        db `Error reading disk\r\n`, 0

bios_sector_error_string:
        db `Error reading disk: not enough sectors loaded\r\n`, 0


; The magic number must come at the end of the boot
; sector. We need to fill the space with zeros to ensure that
; the magic number comes at the end. Remember, since the magic
; number is 2 bytes, the rest of the boot sector is 510.

; We use the 'times' command to repeat the 'db' or "define byte" command
; to pad the rest. Remeber, we will use the '$' and '$$' commands to
; determine how much space is left between the final instruction and
; the magic number.
times 510 - ($ - $$) db 0x00

; The 'dw' command stands for "define word", and puts
; a 16-bit literal integer at that specific location in the file.
; We use 'dw' here to define the magic number, 'AA55'
dw 0xaa55

; This will be loaded into the sector _after_ the boot sector.
bootsector_extended:

bootsector_extended_msg:
        db `Now reading from the next sector!\r\n`, 0

; Fill with zeros to the end of the sector
times 512 - ($ - bootsector_extended) db 0x00
