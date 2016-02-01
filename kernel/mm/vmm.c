// kernel/mm/vmm.c

#include <kernel/def.h>
#include <kernel/mm/kmm.h>
#include <kernel/mm/pmm.h>
#include <kernel/mm/umm.h>
#include <kernel/mm/mmap.h>
#include <kernel/process/lock.h>
#include <debug.h>

static _size_t dmmap_page_count = DMMAP_PAGE_MAX;
static _size_t vmmap_page_count;
static _size_t vmm_free_page_count;

static _size_t vmm_pos;

static _lock_t lock;

static _size_t get_globl_page_index(_size_t index) {
    return dmmap_page_count + index;
}

static void* get_vmm_page(_size_t index) {
    if (index < vmmap_page_count) {
        return get_globl_page_attr(get_globl_page_index(index));
    }
    return NULL;
}

_size_t get_vmm_free_count(void) {
    return vmm_free_page_count;
}

void* vmm_alloc_page(_size_t count) {
    _size_t p;
    process_lock(&lock);
    for (p = vmm_pos; p > 0; p--) {
        _size_t i;
        for (i = 0; i < count; i++) {
            if (p - i < 0 || get_vmm_page(p - i) != 0)
                break;
        }
        if (i == count) {
            _size_t head = p - count + 1;
            for (i = 0; i < count; i++) {
                void* attr = pmm_alloc_page();
                if (attr != NULL) {
                    globl_page_mmap((_size_t) attr / PAGE_SIZE, get_globl_page_index(head + i));
                }
                else {
                    for (; i > 0; i--) {
                        globl_page_remove(get_globl_page_index(head + i));
                        pmm_free_page(get_vmm_page(head + i), 1);
                    }
                    process_unlock(&lock);
                    return NULL;
                }
            }
            if (p == vmm_pos) {
                vmm_pos -= i;
            }
            vmm_free_page_count -= count;
            process_unlock(&lock);
            return (void*) (get_globl_page_index(head) * PAGE_SIZE);
        }
        else {
            p -= i;
        }
    }
    process_unlock(&lock);
    debug("Kernel Mapp: Dynamic Pages Insufficient");
    return NULL;
}

void vmm_free_page(void* attr, _size_t count) {
    _size_t i, index = (_size_t) attr / PAGE_SIZE;
    if (index < dmmap_page_count) {
        pmm_free_page(attr, count);
    }
    else {
        index -= dmmap_page_count;
        if (index + count <= vmmap_page_count) {
            for (i = 0; i < count; i++) {
                pmm_free_page(get_vmm_page(index + i), 1);
                globl_page_remove(get_globl_page_index(index + i));
            }
            if (index + count - 1 > vmm_pos)
                vmm_pos = index + count - 1;
        }
        vmm_free_page_count += count;
    }
}

void* vmm_mmap_page(void* phy_attr) {
    if ((_size_t) phy_attr / PAGE_SIZE < dmmap_page_count) {
        return phy_attr;
    }
    _size_t p;
    process_lock(&lock);
    for (p = vmm_pos; p > 0; p--) {
        if (get_vmm_page(p) == 0) {
            globl_page_mmap((_size_t) phy_attr / PAGE_SIZE, get_globl_page_index(p));
            vmm_free_page_count--;
            vmm_pos = p;
            process_unlock(&lock);
            return (void*) (get_globl_page_index(p) * PAGE_SIZE);
        }
    }
    process_unlock(&lock);
    debug("Kernel Mapp: Dynamic Pages Insufficient");
    return NULL;
}

void* vmm_mmap_pde_page(void* attr, _size_t count, _pde_t* pde) {
    _size_t p;
    process_lock(&lock);
    for (p = vmm_pos; p > 0; p--) {
        _size_t i;
        for (i = 0; i < count; i++) {
            if (p - i < 0 || get_vmm_page(p - i) != 0)
                break;
        }
        if (i == count) {
            _size_t head = p - count + 1;
            for (i = 0; i < count; i++) {
                void* phy_attr = get_page_attr(get_umm_value((void*) ((_size_t) attr + i * PAGE_SIZE), pde));
                if (phy_attr == NULL) {
                    phy_attr = pmm_alloc_page();
                    if (phy_attr != NULL) set_umm_value((void*) ((_size_t) attr + i * PAGE_SIZE), set_page(get_globl_phy_attr(phy_attr), PAGE_PRESENT|PAGE_WRITE|PAGE_USER), pde);
                }
                if (phy_attr != NULL) {
                    globl_page_mmap((_size_t) phy_attr / PAGE_SIZE, get_globl_page_index(head + i));
                }
                else {
                    for (; i > 0; i--) {
                        globl_page_remove(get_globl_page_index(head + i));
                    }
                    process_unlock(&lock);
                    return NULL;
                }
            }
            if (p == vmm_pos) {
                vmm_pos -= i;
            }
            vmm_free_page_count -= count;
            process_unlock(&lock);
            return (void*) (get_globl_page_index(head) * PAGE_SIZE);
        }
        else {
            p -= i;
        }
    }
    process_unlock(&lock);
    debug("Kernel Mapp: Dynamic Pages Insufficient");
    return NULL;
}

void vmm_ummap_page(void* attr, _size_t count) {
    _size_t i, index = (_size_t) attr / PAGE_SIZE;
    if (index >= dmmap_page_count) {
        index -= dmmap_page_count;
        if (index + count <= vmmap_page_count) {
            for (i = 0; i < count; i++) {
                globl_page_remove(get_globl_page_index(index + i));
            }
            if (index + count - 1 > vmm_pos)
                vmm_pos = index + count - 1;
        }
        vmm_free_page_count += count;
    }
}

void* dmm_alloc_page(_size_t count) {
    process_lock(&lock);
    void* attr = pmm_up_alloc_page(count);
    process_unlock(&lock);
    if ((_size_t) attr / PAGE_SIZE + count > dmmap_page_count) {
        pmm_free_page(attr, count);
        debug("Kernel Mapp: Direct Pages Insufficient");
        return NULL;
    }
    return attr;
}

void dmm_free_page(void* attr, _size_t count) {
    pmm_free_page(attr, count);
}

void vmm_init(void) {

    if (mem_page_count < DMMAP_PAGE_MAX) {
        dmmap_page_count = mem_page_count;
    }
    vmmap_page_count = GLOBL_PAGE_NUM - dmmap_page_count;

    int i;
    for (i = 0; i < dmmap_page_count; i++) {
        globl_page_mmap(i, i);
    }

    vmm_pos = vmmap_page_count - 1;

    vmm_free_page_count = vmmap_page_count;

    debug("Kernel Mapp: Direct %d pages, Dynamic %d pages", dmmap_page_count, vmmap_page_count);

    init_process_lock(&lock);
}
