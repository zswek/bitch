// kernel/mm/umm.c

#include <arch/x86/page.h>
#include <kernel/def.h>
#include <kernel/mm/kmm.h>
#include <kernel/mm/pmm.h>
#include <kernel/mm/vmm.h>
#include <lib/string.h>

_pte_t get_umm_value(void* attr, _pde_t* pde) {
    if (pde[pde_index(attr)] == 0) {
        return 0;
    }
    else {
        return ((_pte_t*) get_page_attr(pde[pde_index(attr)]))[pte_index(attr)];
    }
}

_size_t set_umm_value(void* attr, _pte_t value, _pde_t* pde) {

    if ((_size_t) attr >= PAGE_OFFSET) return 0;

    _pte_t* pte;
    if (get_page_attr(pde[pde_index(attr)]) == 0) {

        pte = dmm_alloc_page(1);

        if (pte == NULL) {
            return 0;
        }

        int i;
        for (i = 0; i < PAGE_PTE_INDEX; i++) {
            pte[i] = 0;
        }

        pde[pde_index(attr)] = set_page(pte, PAGE_PRESENT|PAGE_WRITE|PAGE_USER);

    }
    else {

        pte = get_page_attr(pde[pde_index(attr)]);

    }

    pte[pte_index(attr)] = value;

    return 1;
}

_pde_t* umm_alloc_pde(void) {

    _pde_t* pde = dmm_alloc_page(1);

    if (pde == NULL) return NULL;

    memcpy(pde, &kernel_mmap_pde[0], PAGE_SIZE);

    return pde;

}

_size_t umm_mmap_page(void* attr, void* mmap_attr, _pde_t* pde) {

    if (get_umm_value(mmap_attr, pde) == 0) {

        return set_umm_value(mmap_attr, set_page(get_globl_phy_attr(attr), PAGE_PRESENT|PAGE_WRITE|PAGE_USER), pde);

    }
    else {

        return 0;

    }

}

/*void* umm_alloc_page(_size_t count, _pde_t* pde) {

    _size_t p;

    for (p = (pde_index(PAGE_OFFSET) - 1) * PAGE_PTE_INDEX; p > 1; p--) {
        _size_t i;
        for (i = 0; i < count; i++) {
            if (p - i < 1 || get_umm_value(p * PAGE_SIZE, pde) != 0)
                break;
        }
        if (i == count) {
            _size_t head = p - count + 1;
            for (i = 0; i < count; i++) {
                void* attr = set_umm_value((head + i) * PAGE_SIZE, 0x100, pde);
                if (attr == NULL) {
                    for (; i > 0; i--) {
                        set_umm_value((head + i) * PAGE_SIZE, 0x0, pde);
                    }
                    return NULL;
                }
            }
            return (void*) (head * PAGE_SIZE);
        }
        else {
            p -= i;
        }
    }

    return NULL;
}*/

void* umm_mmap_kmm(void* attr, _size_t len, _pde_t* pde) {
    if ((_size_t) attr + len > PAGE_OFFSET) return NULL;
    _size_t mmap_attr = (_size_t) attr & ~(PAGE_SIZE - 1);
    _size_t page_conut = ((_size_t) attr + len - mmap_attr + PAGE_SIZE - 1) / PAGE_SIZE;
    void* p;
    if (page_conut == 1) {
        void* phy_attr = get_page_attr(get_umm_value((void*) mmap_attr, pde));
        if (phy_attr == NULL) {
            p = vmm_alloc_page(1);
            if (p != NULL) set_umm_value((void*) mmap_attr, set_page(get_globl_phy_attr(p), PAGE_PRESENT|PAGE_WRITE|PAGE_USER), pde);
        }
        else {
            p = vmm_mmap_page(phy_attr);
        }
    }
    else {
        p = vmm_mmap_pde_page((void*) mmap_attr, page_conut, pde);
    }
    if (p == NULL) return NULL;
    return (void *) ((_size_t) p + (_size_t) attr - mmap_attr);
}

void umm_ummap_kmm(void* attr, _size_t len) {
    _size_t mmap_attr = (_size_t) attr & ~(PAGE_SIZE - 1);
    _size_t page_conut = ((_size_t) attr + len - mmap_attr + PAGE_SIZE - 1) / PAGE_SIZE;
    vmm_ummap_page((void*) mmap_attr, page_conut);
}

_size_t umm_handler(_pde_t* pde) {
    void* attr = (void*) get_page_fault_attr(), *phy = pmm_alloc_page();

    if (phy == NULL) {
        return 0;
    }

    if (set_umm_value(attr, set_page(phy, PAGE_PRESENT|PAGE_WRITE|PAGE_USER), pde) == 0) {
        pmm_free_page(phy, 1);
        return 0;
    }

    return 1;
}

void umm_free(_pde_t* pde) {

    _pte_t* pte;

    int m, n;
    for (m = 0; m < pde_index(PAGE_OFFSET); m++) {
        if (get_page_attr(pde[m]) != 0) {

            pte = get_page_attr(pde[m]);

            for (n = 0; n < PAGE_PTE_INDEX; n++) {
                if (get_page_attr(pte[n]) != 0) {
                    pmm_free_page(get_page_attr(pte[n]), 1);
                }
            }

            dmm_free_page(pte, 1);

        }
    }

    dmm_free_page(pde, 1);
}
