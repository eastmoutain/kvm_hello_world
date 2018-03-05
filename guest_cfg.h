#ifndef GUEST_CFG_H
#define GUEST_CFG_H

#define phy2virt(x) (x)

#ifndef PAGE_SIZE
#define PAGE_SIZE (1 << 12)
#endif

#define GUEST_PHY_SIZE (1 << 30)

#define GUEST_MSG_BUF_OFFSET 0x200000 //(GUEST_PHY_SIZE - PAGE_SIZE)
#define GUEST_MSG_BUF_SIZE (PAGE_SIZE)

#define HLT_INSN_LEN 1

static inline void *guest_msg_buf_ptr(void) {
    return (void *)phy2virt(GUEST_MSG_BUF_OFFSET);
}

static inline int guest_msg_buf_size(void) { return GUEST_PHY_SIZE; }

#endif // GUEST_CFG_H
