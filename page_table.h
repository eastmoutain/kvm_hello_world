
#ifndef PAGE_TABLE_H
#define PAGE_TABLE_H

#include <stdint.h>

typedef unsigned long long pt_addr_t;
typedef unsigned long long arch_flags_t;

#define PLM4_SHIFT 39
#define PDP_SHIFT 30
#define PD_SHIFT 21
#define PT_SHIFT 12

#define X86_PAGE_FRAME (0xfffffffffffff000UL)

#define X86_PHY_ADDR_MASK ((1UL << 48) - 1)
#define X86_PHY_ADDR_PLM4_MASK (((1UL << 9) - 1) << 39)
#define X86_PHY_ADDR_PDP_MASK (((1UL << 9) - 1) << 30)
#define X86_PHY_ADDR_PD_MASK (((1UL << 9) - 1) << 21)
#define X86_PHY_ADDR_PT_MASK (((1UL << 9) - 1) << 12)
#define X86_PHY_ADDR_OFF_MASK ((1UL << 12) - 1)
#define X86_PT_FLAG_MASK ((1UL << 12) - 1)

#define PLM4E_PG_OFFSET(addr) ((addr & X86_PHY_ADDR_PLM4_MASK) >> PLM4_SHIFT)
#define PDPE_PG_OFFSET(addr) ((addr & X86_PHY_ADDR_PDP_MASK) >> PDP_SHIFT)
#define PDE_PG_OFFSET(addr) ((addr & X86_PHY_ADDR_PD_MASK) >> PD_SHIFT)
#define PTE_PG_OFFSET(addr) ((addr & X86_PHY_ADDR_PT_MASK) >> PT_SHIFT)
#define PG_OFFSET(addr) (addr & X86_PHY_ADDR_OFF_MASK)
#define PG_SIZE (1 << 12)

#define PAGE_ALIGN(x) ((x + X86_PT_FLAG_MASK) & X86_PAGE_FRAME)

#define PLM4_PG_ENTRY_CNT (512)
#define PDP_PG_ENTRY_CNT (512)
#define PD_PG_ENTRY_CNT (512)
#define PT_PG_ENTRY_CNT (512)

#define PLM4_PG_VALID_ENTRY_CNT (512)
#define PDP_PG_VALID_ENTRY_CNT (512)
#define PD_PG_VALID_ENTRY_CNT (256)
#define PT_PG_VALID_ENTRY_CNT (512)

extern pt_addr_t guest_plm4[];
extern pt_addr_t guest_pdp[];
extern pt_addr_t guest_pd[];
extern pt_addr_t guest_pt[];

#define PLM4E_NO 1
#define PDPE_NO 1
#define PDE_NO 512
#define PTE_NO 512 * 512

#define X86_PG_P (0x001)
#define X86_PG_RW (0x002)
#define X86_PG_U (0x004)
#define X86_PG_PS (0x080)
#define X86_PTE_PAT (0x080)
#define X86_PG_G (0x100)
#define X86_MMU_CACHE_DISABLE (0x010)
#define X86_PD_FLAGS (X86_PG_RW | X86_PG_P) // 0x003
#define X86_PG_C_BIT (1ul << 47)

extern unsigned char __mem_end[];

unsigned int pg_offset(pt_addr_t addr);
pt_addr_t *get_plm4_table_phy_addr(void);
pt_addr_t *get_pdp_table_phy_addr(void);
pt_addr_t *get_pd_table_phy_addr(void);
pt_addr_t *get_pt_table_phy_addr(void);

unsigned int plm4_table_entry_index(pt_addr_t addr);
unsigned int pdp_table_entry_index(pt_addr_t addr);
unsigned int pd_table_entry_index(pt_addr_t addr);
unsigned int pt_table_entry_index(pt_addr_t addr);

uint64_t read_plm4_entry(unsigned int index);
uint64_t read_pdp_entry(unsigned int index);
uint64_t read_pd_entry(unsigned int index);
uint64_t read_pt_entry(unsigned int index);

void set_plm4_entry(unsigned int index, uint64_t val);
void set_pdp_entry(unsigned int index, uint64_t val);
void set_pd_entry(unsigned int index, uint64_t val);
void set_pt_entry(unsigned int index, uint64_t val);

/* invalid TLB
 * @addr virtual address
 */
static inline void invalid_tlb(void *addr) {
    asm volatile("invlpg (%0)" ::"b"(addr) : "memory");
}

#endif // PAGE_TABLE_H
