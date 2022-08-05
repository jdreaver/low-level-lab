/* ENTRY is really only needed for gdb to understand the entry point. The STM32
bootloader uses the address stored in 0x00000004 (the reset interrupt handler)
to decide where to start. */
ENTRY(Reset_Handler)

/* Set minimum size for stack and dynamic memory. */
/* (The linker will generate an error if there is
 * less than this much RAM leftover.) */
/* (1KB) */
_Min_Leftover_RAM = 0x400;

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

  /* The vector table goes to the start of flash. The boot loader uses the
    address stored in 0x00000000 to decide where to put the end of the stack
    pointer, and then it starts execution from the address at 0x00000004 (4
    bytes after). The default boot pin configuration has 0x00000000 aliased to
    the flash memory at 0x08000000, so we store this stuff in FLASH. */
  .vector_table :
  {
    . = ALIGN(4);
    LONG(ORIGIN(SRAM) + LENGTH(SRAM)); /* Initial stack pointer, located at end of SRAM */
    KEEP (*(.vector_table))
    . = ALIGN(4);
  } >FLASH

  .text :
  {
    . = ALIGN(4);
    *(.text)
    *(.text*)
    . = ALIGN(4);
  } >FLASH

  /* The 'rodata' section contains read-only data,
   * constants, strings, information that won't change. */
  .rodata :
  {
    . = ALIGN(4);
    *(.rodata)
    *(.rodata*)
    . = ALIGN(4);
  } >FLASH

  /* The 'data' section is space set aside in RAM for
   * things like variables, which can change. */
  _sidata = .;
  .data : AT(_sidata)
  {
    . = ALIGN(4);
    /* Mark start/end locations for the 'data' section. */
    _sdata = .;
    *(.data)
    *(.data*)
    _edata = .;
    . = ALIGN(4);
  } >SRAM

  /* The 'bss' section is similar to the 'data' section,
   * but its space is initialized to all 0s at the
   * start of the program. */
  .bss :
  {
    . = ALIGN(4);
    /* Also mark the start/end of the BSS section. */
    _sbss = .;
    *(.bss)
    *(.bss*)
    *(COMMON)
    . = ALIGN(4);
    _ebss = .;
  } >SRAM

  /* Space set aside for the application's heap/stack. */
  .dynamic_allocations :
  {
    . = ALIGN(4);
    _ssystem_ram = .;
    . = . + _Min_Leftover_RAM;
    . = ALIGN(4);
    _esystem_ram = .;
  } >SRAM
}
