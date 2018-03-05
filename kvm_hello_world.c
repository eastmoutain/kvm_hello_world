#include "guest_cfg.h"
#include "page_table.h"
#include "printf.h"
#include <errno.h>
#include <fcntl.h>
#include <linux/kvm.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>

/* CR0 bits */
#define CR0_PE 1
#define CR0_MP (1 << 1)
#define CR0_EM (1 << 2)
#define CR0_TS (1 << 3)
#define CR0_ET (1 << 4)
#define CR0_NE (1 << 5)
#define CR0_WP (1 << 16)
#define CR0_AM (1 << 18)
#define CR0_NW (1 << 29)
#define CR0_CD (1 << 30)
#define CR0_PG (1 << 31)

/* CR4 bits */
#define CR4_VME 1
#define CR4_PVI (1 << 1)
#define CR4_TSD (1 << 2)
#define CR4_DE (1 << 3)
#define CR4_PSE (1 << 4)
#define CR4_PAE (1 << 5)
#define CR4_MCE (1 << 6)
#define CR4_PGE (1 << 7)
#define CR4_PCE (1 << 8)
#define CR4_OSFXSR (1 << 8)
#define CR4_OSXMMEXCPT (1 << 10)
#define CR4_UMIP (1 << 11)
#define CR4_VMXE (1 << 13)
#define CR4_SMXE (1 << 14)
#define CR4_FSGSBASE (1 << 16)
#define CR4_PCIDE (1 << 17)
#define CR4_OSXSAVE (1 << 18)
#define CR4_SMEP (1 << 20)
#define CR4_SMAP (1 << 21)

#define EFER_SCE 1
#define EFER_LME (1 << 8)
#define EFER_LMA (1 << 10)
#define EFER_NXE (1 << 11)

/* 32-bit page directory entry bits */
#define PDE32_PRESENT 1
#define PDE32_RW (1 << 1)
#define PDE32_USER (1 << 2)
#define PDE32_PS (1 << 7)

/* 64-bit page * entry bits */
#define PDE64_PRESENT 1
#define PDE64_RW (1 << 1)
#define PDE64_USER (1 << 2)
#define PDE64_ACCESSED (1 << 5)
#define PDE64_DIRTY (1 << 6)
#define PDE64_PS (1 << 7)
#define PDE64_G (1 << 8)

extern unsigned char __mem_start[];
extern unsigned char __mem_end[];
extern void dump_register(struct kvm_regs *regs, struct kvm_sregs *sregs);

struct vm {
    int sys_fd;
    int fd;
    char *mem;
    char *guest_msg_buf;
};

void vm_init(struct vm *vm, size_t mem_size) {
    int api_ver;
    struct kvm_userspace_memory_region memreg;

    vm->sys_fd = open("/dev/kvm", O_RDWR);
    if (vm->sys_fd < 0) {
        perror("open /dev/kvm");
        exit(1);
    }

    api_ver = ioctl(vm->sys_fd, KVM_GET_API_VERSION, 0);
    if (api_ver < 0) {
        perror("KVM_GET_API_VERSION");
        exit(1);
    }

    if (api_ver != KVM_API_VERSION) {
        fprintf(stderr, "Got KVM api version %d, expected %d\n", api_ver,
                KVM_API_VERSION);
        exit(1);
    }

    vm->fd = ioctl(vm->sys_fd, KVM_CREATE_VM, 0);
    if (vm->fd < 0) {
        perror("KVM_CREATE_VM");
        exit(1);
    }

    if (ioctl(vm->fd, KVM_SET_TSS_ADDR, 0xfffbd000) < 0) {
        perror("KVM_SET_TSS_ADDR");
        exit(1);
    }

    vm->mem = mmap(NULL, mem_size, PROT_READ | PROT_WRITE,
                   MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE, -1, 0);
    if (vm->mem == MAP_FAILED) {
        perror("mmap mem");
        exit(1);
    }

    madvise(vm->mem, mem_size, MADV_MERGEABLE);
    vm->guest_msg_buf = vm->mem + GUEST_MSG_BUF_OFFSET;

    memreg.slot = 0;
    memreg.flags = 0;
    memreg.guest_phys_addr = 0;
    memreg.memory_size = mem_size;
    memreg.userspace_addr = (unsigned long)vm->mem;

    if (ioctl(vm->fd, KVM_SET_USER_MEMORY_REGION, &memreg) < 0) {
        perror("KVM_SET_USER_MEMORY_REGION");
        exit(1);
    }
}

struct vcpu {
    int fd;
    struct kvm_run *kvm_run;
};

void vcpu_init(struct vm *vm, struct vcpu *vcpu) {
    int vcpu_mmap_size;

    vcpu->fd = ioctl(vm->fd, KVM_CREATE_VCPU, 0);
    if (vcpu->fd < 0) {
        perror("KVM_CREATE_VCPU");
        exit(1);
    }

    vcpu_mmap_size = ioctl(vm->sys_fd, KVM_GET_VCPU_MMAP_SIZE, 0);
    if (vcpu_mmap_size <= 0) {
        perror("KVM_GET_VCPU_MMAP_SIZE");
        exit(1);
    }

    vcpu->kvm_run = mmap(NULL, vcpu_mmap_size, PROT_READ | PROT_WRITE,
                         MAP_SHARED, vcpu->fd, 0);
    if (vcpu->kvm_run == MAP_FAILED) {
        perror("mmap kvm_run");
        exit(1);
    }
}

int check(struct vm *vm, struct vcpu *vcpu, size_t sz) {
    struct kvm_regs regs;
    struct kvm_sregs sregs;

    vm = vm;
    sz = sz;

    if (ioctl(vcpu->fd, KVM_GET_REGS, &regs) < 0) {
        perror("KVM_GET_REGS");
        exit(1);
    }

    if (ioctl(vcpu->fd, KVM_GET_SREGS, &sregs) < 0) {
        perror("KVM_GET_SREGS");
        exit(1);
    }

    dump_register(&regs, &sregs);

    return 1;
}

static void print_guest_msg(char *buf) {

    if (NULL == buf)
        return;
    else {
        printf("%s", buf);
    }
    memset(buf, 0, 1);
}

int run_real_mode(struct vm *vm, struct vcpu *vcpu) {
    struct kvm_sregs sregs;
    struct kvm_regs regs;

    if (ioctl(vcpu->fd, KVM_GET_SREGS, &sregs) < 0) {
        perror("KVM_GET_SREGS");
        exit(1);
    }

    sregs.cs.selector = 0;
    sregs.cs.base = 0;

    if (ioctl(vcpu->fd, KVM_SET_SREGS, &sregs) < 0) {
        perror("KVM_SET_SREGS");
        exit(1);
    }

    memset(&regs, 0, sizeof(regs));
    /* Clear all FLAGS bits, except bit 1 which is always set. */
    regs.rflags = 2;
    regs.rip = 0;

    if (ioctl(vcpu->fd, KVM_SET_REGS, &regs) < 0) {
        perror("KVM_SET_REGS");
        exit(1);
    }

    printf("copy [%p~%p], size 0x%lx to kvm space\n", __mem_start, __mem_end,
           __mem_end - __mem_start);

    memcpy(vm->mem, __mem_start, __mem_end - __mem_start);

    while (1) {
        if (ioctl(vcpu->fd, KVM_RUN, 0) < 0) {
            perror("KVM_RUN");
            exit(1);
        }

        if (vcpu->kvm_run->exit_reason == KVM_EXIT_HLT) {
            print_guest_msg(vm->guest_msg_buf);

			if (ioctl(vcpu->fd, KVM_GET_REGS, &regs) < 0) {
                perror("KVM_GET_REGS");
                exit(1);
            }

			if (regs.rax == 0x123456789fedcba) {
                printf("vm exit successfully!\n");
                exit(1);
            }

			regs.rip += HLT_INSN_LEN;
            if (ioctl(vcpu->fd, KVM_SET_REGS, &regs) < 0) {
                perror("KVM_SET_REGS");
                exit(1);
            }
        } else {
            fprintf(stderr, "Got exit_reason %d, expected KVM_EXIT_HLT (%d)\n",
                    vcpu->kvm_run->exit_reason, KVM_EXIT_HLT);
            exit(1);
        }
    }

    return check(vm, vcpu, 2);
}

int main(int argc, char **argv) {
    struct vm vm;
    struct vcpu vcpu;

    argc = argc;
    argv = argv;

    printf("alloc 0x%x physical address space for guest\n", GUEST_PHY_SIZE);
    vm_init(&vm, GUEST_PHY_SIZE);
    vcpu_init(&vm, &vcpu);

    return !run_real_mode(&vm, &vcpu);
}
