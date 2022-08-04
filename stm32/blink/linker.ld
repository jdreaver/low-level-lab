/* ENTRY is really only needed for gdb to understand the entry point. The STM32
bootloader uses the address stored in 0x00000004 (the reset interrupt handler)
to decide where to start. */
ENTRY(Reset_Handler)

/* Memory layout for an STM32F401RE. See Figure 15. Memory map on page 51 of the
datasheet. */
MEMORY
{
  FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 512K
  SRAM (xrw) : ORIGIN = 0x20000000, LENGTH = 96K
}

/* output sections */
SECTIONS
{
    /* Program code into FLASH. The boot loader uses the address stored in
    0x00000000 to decide where to put the end of the stack pointer, and then it
    starts execution from the address at 0x00000004 (4 bytes after). The default
    boot pin configuration has 0x00000000 aliased to the flash memory at
    0x08000000, so we store this stuff in FLASH. */
    .text :
    {
        LONG(ORIGIN(SRAM) + LENGTH(SRAM)); /* Initial stack pointer, located at end of SRAM */
        *(.vector_table)    /* Vector table */
        *(.text)            /* Program code */
        . = ALIGN(4);       /* Always good practice to align here */
    } >FLASH

    /* uninitialized global and static variables (which
       we don't have any in this example) into SRAM */
    .data :
    {
        *(.data)
        . = ALIGN(4);       /* Always good practice to align here */
    } >SRAM
}
