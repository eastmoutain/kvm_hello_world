
#include "page_table.h"

unsigned int pg_offset(pt_addr_t addr)
{
    return PG_OFFSET(addr);
}

pt_addr_t* get_plm4_table_phy_addr(void)
{
    return (pt_addr_t*)guest_plm4;
}

pt_addr_t* get_pdp_table_phy_addr(void)
{
    return (pt_addr_t*)guest_pdp;
}

pt_addr_t* get_pd_table_phy_addr(void)
{
    return (pt_addr_t*)guest_pd;
}

pt_addr_t* get_pt_table_phy_addr(void)
{
    return (pt_addr_t*)guest_pt;
}

unsigned int plm4_table_entry_index(pt_addr_t addr)
{
    return PLM4E_PG_OFFSET(addr);
}

unsigned int pdp_table_entry_index(pt_addr_t addr)
{
    return plm4_table_entry_index(addr) * PDP_PG_ENTRY_CNT
           + PDPE_PG_OFFSET(addr) ;
}

unsigned int pd_table_entry_index(pt_addr_t addr)
{
    return pdp_table_entry_index(addr) * PD_PG_ENTRY_CNT
           + PDE_PG_OFFSET(addr);
}

unsigned int pt_table_entry_index(pt_addr_t addr)
{
    return pdp_table_entry_index(addr) * PD_PG_VALID_ENTRY_CNT * PT_PG_ENTRY_CNT
           + PTE_PG_OFFSET(addr);
}

uint64_t read_plm4_entry(unsigned int index)
{
    pt_addr_t *plm4_table = get_plm4_table_phy_addr();
    return (uint64_t)plm4_table[index];
}

uint64_t read_pdp_entry(unsigned int index)
{
    pt_addr_t *pdp_table = get_pdp_table_phy_addr();
    return (uint64_t)pdp_table[index];
}

uint64_t read_pd_entry(unsigned int index)
{
    pt_addr_t *pd_table = get_pd_table_phy_addr();
    return (uint64_t)pd_table[index];
}

uint64_t read_pt_entry(unsigned int index)
{
    pt_addr_t *pt_table = get_pt_table_phy_addr();
    return (uint64_t)pt_table[index];
}

void set_plm4_entry(unsigned int index, uint64_t val)
{
    pt_addr_t *plm4_table = get_plm4_table_phy_addr();
    plm4_table[index] = val;
}

void set_pdp_entry(unsigned int index, uint64_t val)
{
    pt_addr_t *pdp_table = get_pdp_table_phy_addr();
    pdp_table[index] = val;
}

void set_pd_entry(unsigned int index, uint64_t val)
{
    pt_addr_t *pd_table = get_pd_table_phy_addr();
    pd_table[index] = val;
}

void set_pt_entry(unsigned int index, uint64_t val)
{
    pt_addr_t *pt_table = get_pt_table_phy_addr();
    pt_table[index] = val;
}


