OUTPUT_FORMAT(elf64-x86-64)
ENTRY(_start)

SECTIONS
{
    . = 0x800000000000;
    _KernelStart = .;
    .text ALIGN(0x1000) : AT (ADDR (.text) - 0x800000000000){
        *(.text*)
    }

    .data ALIGN(0x1000) : AT (ADDR (.data) - 0x800000000000){
        *(.data*)
    }

    .rodata ALIGN(0x1000) : AT (ADDR (.rodata) - 0x800000000000){
        *(.rodata*)
    }

    .bss ALIGN(0x1000) : AT (ADDR (.bss) - 0x800000000000){
        *(COMMON*)
        *(.bss*)
    }
    _KernelEnd = .;
}
