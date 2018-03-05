
#define MSR_EFER 0xc0000080
#define EFER_LME 0x00000100


/*
        PHY address view                   Virt addr view
     1G   0x40000000 +-----+ -------------->  +-----+ 0x60000000  1.5G
                     |     |                  |     |
                     |     |                  |     |
     0.5G 0x20000000 +-----+ = _ ---------->  +-----+ 0x40000000  1G
                     |     |    \
                     |     |      \-------->  +-----+ 0x20000000  0.5G
     0G   0x00000000 +-----+ _                |     |
                               \              |     |
                                \---------->  +-----+ 0x00000000  0G

*/

    .code16
    .global real_start
real_start:
    // load new gdt
    lgdt gdt_init

    // enable protect mode
    mov %cr0, %eax
    or $0x01, %eax
    mov %eax, %cr0

    ljmp $(1*8), $(protect_start)
    hlt

        .code32
protect_start:
    mov $(2*8), %ax
    mov %ax, %ds
    mov %ax, %ss

    mov $(9*8), %ax
    ltr %ax

    mov $0, %ax
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs


    // eable CR4.PAE and CR4.PGE
    mov %cr4, %eax
    or $0xa0, %eax
    mov %eax, %cr4

    // long mode enable
    movl $0xc0000080, %ecx
    rdmsr
    orl $0x00000100, %eax
    wrmsr

    // clear guest_pt
    movl guest_pt_size, %ecx
    shrl $2, %ecx
    movl $guest_plm4, %eax
1:
    movl $0, %eax
    addl $4, %eax
    loop 1b

    // guenst_plm4e -> guest_pdp, 1 entry
    movl $(guest_pdp), %eax
    orl $0x003, %eax
    mov %eax, guest_plm4

    // guest_pdp[0] -> guest_pd, 1 entry
    movl $(guest_pd), %eax
    orl $0x013, %eax
    mov %eax, guest_pdp


    // guest_pde[0~255] -> guest_pt, 256 entries
    movl $guest_pd, %eax
    movl $guest_pt, %esi
    movl $256, %ecx
2:
    orl $0x013, %esi
    movl %esi, (%eax)
    addl $8, %eax
    addl $4096, %esi
    loop 2b


    // guest_pdp[1] -> guest_pd + 0x1000
    movl $(guest_pdp), %eax
    addl $8, %eax
    movl $(guest_pd), %ebx
    addl $4096, %ebx
    orl $0x013, %ebx
    mov %ebx, (%eax)

    // guenst_pde[512~767] -> guest_pt, 256 entries
    // phy [512M ~ 1G]
    mov $(guest_pd), %eax
    addl $4096, %eax
    mov $guest_pt, %esi
    addl $0x100000, %esi #1M
    movl $256, %ecx
6:
    orl $0x013, %esi
    mov %esi, (%eax)
    addl $8, %eax
    addl $4096, %esi
    loop 6b

    // guest_pte -> phy addr, 512 * 512 entry
    movl $guest_pt, %eax
    movl $0, %esi
    movl $0x40000, %ecx
3:
    orl $0x013, %esi
    movl %esi, (%eax)
    addl $8, %eax
    addl $4096, %esi
    loop 3b

    // page table is done. load pml4 phy addr to cr3
    movl $guest_plm4, %eax
    mov %eax, %cr3

    // enable paging
    mov %cr0, %eax
    btsl $31, %eax
    mov %eax, %cr0


    ljmpl $(5*8), $(long_start)
    hlt

    .global guest_main
    .code64
long_start:
    mov $(6*8), %ax
    mov %ax, %ds
    mov %ax, %ss

    movq $stack_bottom, %rax
    mov %rax, %rsp
    mov %rax, %rbp

    push %rax
    push %rbx

    // reload gdt, i don't know why
    //lgdt gdt_init

    call guest_main

    # vm exit
    movq $0x123456789fedcba, %rax
    hlt


    .align 16
    .global gdt
gdt:
    .word    0,0,0,0				# seg 0 - null
    .word    0xffff, 0x0000, 0x9a00, 0x00cf	# seg 1 - kernel flat 4GB code
    .word    0xffff, 0x0000, 0x9200, 0x00cf	# seg 2 - kernel flat 4GB data
    .word    0xffff, 0x0000, 0xfa00, 0x00cf	# seg 3 - user flat 4GB code
    .word    0xffff, 0x0000, 0xf200, 0x00cf	# seg 4 - user flat 4GB data
    .word    0xffff, 0x0000, 0x9a00, 0x00af  # seg 5 - kernel 64bit flat 4G code
    .word    0xffff, 0x0000, 0x9200, 0x00cf  # seg 6 - kernel 64bit flat 4G data
    .word    0xffff, 0x0000, 0xfa00, 0x00af  # seg 7 - user 64bit flat 4GB code
    .word    0xffff, 0x0000, 0xf200, 0x00cf  # seg 8 - user 64bit flat 4GB data
    .word    0x0068, (tss-real_start),0x8901, 0x00cf  # seg 9 - TSS

gdt_init:
	.word	gdt_init - gdt
	.long	gdt


    .align 16
    .global tss
tss:
    .long  0

    .data
arr:
    .int 0x12345678
    .int 0x1
    .int 0x2
    .skip 0x1024

    .global guest_plm4
    .align 4096
guest_plm4:
    .skip 0x8

    .global guest_pdp
    .align 4096
guest_pdp:
    .skip 0x16

    .global guest_pd
    .align 4096
guest_pd:
    // 8K byte, 1024 * 8byte entries
    .skip 0x2000

    .global guest_pt
    .align 4096
guest_pt:
    // 2M byte, 512 * 512 8byte entries
    .skip 0x200000
guest_pt_size:
    .long guest_pt_size - guest_plm4

    .align 4096
stack_top:
    .skip 0x200000
stack_bottom:

