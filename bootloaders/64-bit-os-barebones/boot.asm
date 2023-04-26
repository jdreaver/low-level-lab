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

; Print some stuff
mov bx, hello
call print_bios

mov dx, 0x8934
call print_hex_bios

; Load the next sector
mov bx, 0x7C00 + 512            ; Destination is boot sector destination + 512 bytes
mov cl, 0x02                    ; Start at sector 2 (sectors are 1-based)
mov dl, 0x01                    ; Load 1 sector
call bios_load

mov bx, about_to_jump_to_protected
call print_bios

call elevate_bios

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
        jmp $

bios_load_sectors_error:
        mov bx, bios_sector_error_string
        call print_bios
        jmp $

;
; GDT: Define the Flat Mode Configuration Global Descriptor Table (GDT). The
; flat mode table allows us to read and write code anywhere, without
; restriction. We will set up paging later in 64 bit (long) mode, so we just
; want the simplest possible GDT here to get into 32 bit (protected) mode. This
; structure is loaded with the LGDT instruction before we switch to 32 bit mode.
;
; See https://wiki.osdev.org/Global_Descriptor_Table
;

gdt_32_start:

; Define the null sector for the 32 bit gdt, which is 8 bytes of nulls. Null
; sector is required for memory integrity check.
gdt_32_null:
        ; All values in null entry are 0
        dd 0x00000000
        dd 0x00000000

; After the null section comes sector entries, each 8 bytes long.
gdt_32_code:
        ; Base:     0x00000
        ; Limit:    0xFFFFF
        ; 1st Flags:        0b1001
        ;   Present:        1  (1 = a valid segment)
        ;   Privilege:      00 (0 = highest)
        ;   Descriptor:     1  (1 = code or data segment)
        ; Type Flags:       0b1010
        ;   Code:           1  (1 = executable, or code segment)
        ;   Conforming:     0  (0 = can only be executed from the privilege level set above, which is 0)
        ;   Readable:       1  (1 = readable. Write access is never allow for code segments)
        ;   Accessed:       0  (CPU sets this to 1 when accessed. We keep is clear for now)
        ; 2nd Flags:        0b1100
        ;   Granularity:    1  (1 = limit is in 4KB blocks, not bytes)
        ;   32-bit Default: 1  (1 = 32 bit mode)
        ;   64-bit Segment: 0  (0 because we are in 32 bit mode. Mutually exclusive with previous flag)
        ;   Reserved:       0  (reserved, always 0)

        dw 0xFFFF           ; Limit (bits 0-15)
        dw 0x0000           ; Base  (bits 0-15)
        db 0x00             ; Base  (bits 16-23)
        db 0b10011010       ; 1st Flags, Type flags
        db 0b11001111       ; 2nd Flags, Limit (bits 16-19)
        db 0x00             ; Base  (bits 24-31)

; Define the data sector for the 32 bit gdt
gdt_32_data:
        ; Base:     0x00000
        ; Limit:    0xFFFFF
        ; 1st Flags:        0b1001
        ;   Present:        1  (1 = a valid segment)
        ;   Privilege:      00 (0 = highest)
        ;   Descriptor:     1  (1 = code or data segment)
        ; Type Flags:       0b0010
        ;   Code:           0  (0 = data segment, not code)
        ;   Expand Down:    0  (0 = data segment grows up, not down)
        ;   Writeable:      1  (1 = writeable, not read only)
        ;   Accessed:       0  (CPU sets this to 1 when accessed. We keep is clear for now)
        ; 2nd Flags:        0b1100
        ;   Granularity:    1  (1 = limit is in 4KB blocks, not bytes)
        ;   32-bit Default: 1  (1 = 32 bit mode)
        ;   64-bit Segment: 0  (0 because we are in 32 bit mode. Mutually exclusive with previous flag)
        ;   Reserved:       0  (reserved, always 0)

        dw 0xFFFF           ; Limit (bits 0-15)
        dw 0x0000           ; Base  (bits 0-15)
        db 0x00             ; Base  (bits 16-23)
        db 0b10010010       ; 1st Flags, Type flags
        db 0b11001111       ; 2nd Flags, Limit (bits 16-19)
        db 0x00             ; Base  (bits 24-31)

gdt_32_end:

; The GDT descriptor data structure gives the CPU the length and start address
; of gdt. We will feed this structure to the CPU in order to set the protected
; mode GDT.
gdt_32_descriptor:
        dw gdt_32_end - gdt_32_start - 1        ; Size of GDT, one byte less than true size
        dd gdt_32_start                         ; Start of the 32 bit gdt

; Define helpers to find pointers to Code and Data segments
gdt_32_code_seg: equ gdt_32_code - gdt_32_start
gdt_32_data_seg: equ gdt_32_data - gdt_32_start

; This function will raise our CPU to the 32-bit protected mode. This is done
; with the following steps:
;   1. Disable interrupts
;   2. Load the GDT into the CPU
;   3. Set 32-bit mode in the control register
;   4. Clear the processor pipeline

elevate_bios:
        ; We need to disable interrupts because elevating to 32-bit mode causes
        ; the CPU to go a little crazy. We do this with the 'cli' command.
        cli

        ; 32-bit protected mode requires the GDT, so we tell the CPU where it is
        ; with the 'lgdt' command
        lgdt [gdt_32_descriptor]

        ; Enable 32-bit mode by setting bit 0 of the original control register.
        ; We cannot set this directly, so we need to copy the contents into eax
        ; (32-bit version of ax) and back again
        mov eax, cr0
        or eax, 0x00000001
        mov cr0, eax

        ; Now we need to clear the pipeline of all 16-bit instructions, which we
        ; do with a far jump. The address doesn't actually need to be far away,
        ; but the type of jump needs to be specified as 'far'
        jmp gdt_32_code_seg:init_protected_mode

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

about_to_jump_to_protected:
        db `\r\nAbout to jump to protected mode...\r\n`, 0

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



;
; Next sector. This sector will only hold 32 bit code.
;

[bits 32]

; This will be loaded into the sector _after_ the boot sector.
bootsector_extended:
begin_protected:

call clear_vga_protected

mov esi, protected_alert
call print_vga_protected

; Loop forever so we can see output
jmp $

init_protected_mode:
    ; Congratulations! You're now in 32-bit mode!
    ; There's just a bit more setup we need to do before we're ready
    ; to actually execute instructions

    ; We need to tell all segment registers to point to our flat-mode data
    ; segment. If you're curious about what all of these do, you might want
    ; to look on the OSDev Wiki. We will not be using them enough to matter.
    mov ax, gdt_32_data_seg
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; Since the stack pointers got messed up in the elevation process, and we
    ; want a fresh stack, we need to reset them now.
    mov ebp, 0x90000
    mov esp, ebp

    ; Go to the second sector with 32-bit code
    jmp begin_protected

;
; VGA stuff. See https://wiki.osdev.org/Printing_To_Screen
;

; Define VGA constants
vga_start:       equ 0x000B8000
vga_extent:      equ 80 * 25 * 2  ; VGA Memory is 80 chars wide by 25 chars tall (one char is 2 bytes)
vga_end:         equ vga_start + vga_extent
vga_white_text:  equ 0x0F       ; White text on black background

; clear_vga_protected clears the VGA memory by writing spaces to it.
clear_vga_protected:
        push edx

        mov edx, vga_start

clear_vga_protected_loop:
        ; Loop condition
        cmp edx, vga_end     ; Have we reached the end of the VGA memory?
        jge clear_vga_protected_done

        ; Lower part of al is the character to print and ah is the background
        ; color.
        mov al, ' '
        mov ah, vga_white_text

        ; Write ax (2 bytes) to next spot in VGA
        mov [edx], ax

        ; Increment edx by 2 (since we're writing 2 bytes)
        add edx, 2

        jmp clear_vga_protected_loop

clear_vga_protected_done:
        pop edx
        ret


; print_vga_protected prints a string to the VGA memory.
; Input: esi = address of string to print
print_vga_protected:
        push edx

        mov edx, vga_start

print_vga_protected_loop:
        ; Loop condition
        cmp byte[esi], 0        ; Have we reached the end of the string?
        je print_vga_protected_done

        ; Lower part of al is the character to print and ah is the background
        ; color.
        mov al, byte[esi]
        mov ah, vga_white_text

        ; Write ax (2 bytes) to next spot in VGA
        mov [edx], ax

        add esi, 1
        ; Increment edx by 2 (since we're writing 2 bytes)
        add edx, 2

        jmp print_vga_protected_loop

print_vga_protected_done:
        pop edx
        ret


protected_alert:
        db `Now in 32-bit protected mode!`, 0

; Fill with zeros to the end of the sector
times 512 - ($ - bootsector_extended) db 0x00
