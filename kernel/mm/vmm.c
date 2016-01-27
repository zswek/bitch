// kernel/mm/vmm.c

#include <kernel/def.h>
#include <kernel/mm/kmm.h>
#include <kernel/mm/pmm.h>
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
                    globl_page_mmap((_u32_t) attr / PAGE_SIZE, get_globl_page_index(head + i));
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
    return NULL;
}

void vmm_free_page(void* attr, _size_t count) {
    _size_t i, index = (_u32_t) attr / PAGE_SIZE - dmmap_page_count;
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

void* vmm_mmap_page(void* phy_attr) {
    _size_t p;
    process_lock(&lock);
    for (p = vmm_pos; p > 0; p--) {
        if (get_vmm_page(p) == 0) {
            globl_page_mmap((_u32_t) phy_attr / PAGE_SIZE, get_globl_page_index(p));
            vmm_free_page_count--;
            process_unlock(&lock);
            return (void*) (get_globl_page_index(p) * PAGE_SIZE);
        }
    }
    process_unlock(&lock);
    return NULL;
}

void vmm_unmamp_page(void* attr, _size_t count) {
    _size_t i, index = (_u32_t) attr / PAGE_SIZE - dmmap_page_count;
    if (index + count <= vmmap_page_count) {
        for (i = 0; i < count; i++) {
            globl_page_remove(get_globl_page_index(index + i));
        }
        if (index + count - 1 > vmm_pos) 
            vmm_pos = index + count - 1;
    }
    vmm_free_page_count += count;
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