#include "gprintf.h"
#include "page_table.h"

static unsigned char *heap_base;
unsigned char __mem_end[];
int guest_main(void) {
    heap_base = (unsigned char *)PAGE_ALIGN((unsigned long)__mem_end);

    gprintf("hello, world!\n");
    gprintf("try to add your own code, bye!\n");

    return 0x0;
}
