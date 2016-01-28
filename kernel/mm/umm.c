// kernel/mm/umm.c

#include <kernel/def.h>
#include <kernel/mm/kmm.h>
#include <kernel/mm/pmm.h>
#include <kernel/mm/vmm.h>
#include <lib/string.h>

_pde_t* umm_alloc_pde(void) {

    _pde_t* pde = vmm_alloc_page(1);

    if (pde == NULL) return NULL;

    memcpy(pde, &kernel_mmap_pde[0], PAGE_SIZE);

    return pde;

}

void* umm_mmap_page(void* attr, void* mmap_attr, _pde_t* pde) {

    if ((_u32_t) mmap_attr >= PAGE_OFFSET) return NULL;

    _pte_t* pte;
    if (pde[pde_index(mmap_attr)] == 0) {

        pte = vmm_alloc_page(1);

        if (pte == NULL) {
            return NULL;
        }

        int i;
        for (i = 0; i < PAGE_PTE_INDEX; i++) {
            pte[i] = set_page(0, 0);
        }

        pde[pde_index(mmap_attr)] = set_page(get_globl_phy_attr(pte), PAGE_PRESENT|PAGE_WRITE|PAGE_USER);

    }
    else {

        pte = vmm_mmap_page(get_page_attr(pde[pde_index(mmap_attr)]));

        if (pte == NULL) {
            return NULL;
        }

    }

    if (pte[pte_index(mmap_attr)] == 0) {

        pte[pte_index(mmap_attr)] = set_page(get_globl_phy_attr(attr), PAGE_PRESENT|PAGE_WRITE|PAGE_USER);

        vmm_ummap_page(pte, 1);

        return mmap_attr;

    }
    else {

        vmm_ummap_page(pte, 1);

        return NULL;

    }

}

void umm_free(_pde_t* pde) {

    _pte_t* pte;

    int m, n;
    for (m = 0; m < pde_index(PAGE_OFFSET); m++) {
        if (pde[m] != 0) {

            pte = vmm_mmap_page(get_page_attr(pde[m]));

            for (n = 0; n < PAGE_PTE_INDEX; n++) {
                if (get_page_attr(pte[n]) != 0) {
                    pmm_free_page(get_page_attr(pte[n]), 1);
                }
            }

            vmm_free_page(pte, 1);

        }
    }

    vmm_free_page(pde, 1);
}
