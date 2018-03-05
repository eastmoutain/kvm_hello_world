#include <stdio.h>
#include <linux/kvm.h>

void dump_register(struct kvm_regs *regs, struct kvm_sregs *sregs)
{
    printf("=================  dump register  =====================\n");
    printf("eax: 0x%08llx, ebx: 0x%08llx, ecx: 0x%08llx\n"
           "edx: 0x%08llx, esi: 0x%08llx, edi: 0x%08llx\n"
           "ip: 0x%08llx, sp: 0x%08llx, flags: 0x%08llx\n"
           "cs: 0x%04x, ss: 0x%04x, ds: 0x%04x, es: 0x%04x, fs: 0x%04x\n",
           regs->rax, regs->rbx, regs->rcx, regs->rdx, regs->rsi, regs->rdi,
           regs->rip, regs->rsp, regs->rflags,
           sregs->cs.selector, sregs->ss.selector, sregs->ds.selector,
           sregs->es.selector, sregs->fs.selector);

}
