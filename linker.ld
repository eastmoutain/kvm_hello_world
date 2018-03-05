

SECTIONS
{
        . = 0;
        __mem_start = .;
        .payload16 : {
                 start.o(.text .data .bss .rodata)
                 *(.text)
                 *(.data)
                 *(.bss)
                 *(.rodata)
        }
        __mem_end = .;
}
