// kernel/mm/alloc.c

#include <asm/types.h>
#include <kernel/def.h>
#include <kernel/mm/vmm.h>

typedef struct alloc_header_t {
    struct alloc_header_t* last;
    struct alloc_header_t* next;
    _size_t length;
} _alloc_header_t;

static _alloc_header_t* pos = NULL;

void* kmalloc(_size_t len) {

    _alloc_header_t* mem;

    len += sizeof(_alloc_header_t);

    if ((pos != NULL) && (((((_size_t) pos + pos -> length + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1)) - ((_size_t) pos + pos -> length)) >= len)) {
        mem = (_alloc_header_t*) ((_size_t) pos + pos -> length);
    }
    else {
        if ((mem = vmm_alloc_page((len + PAGE_SIZE - 1) / PAGE_SIZE)) == NULL) {
            return NULL;
        }
    }

    mem -> length = len;
    mem -> last = pos;
    mem -> next = NULL;

    if (pos != NULL) pos -> next = mem;

    pos = mem;

    return (void *) ((_size_t) mem + sizeof(_alloc_header_t));

}

void kfree(void *p) {

    _alloc_header_t* mem = (_alloc_header_t*) ((_size_t) p - sizeof(_alloc_header_t));

    void* attr = (void *) ((_size_t) mem & ~(PAGE_SIZE - 1));
    _ssize_t num = (mem -> length + PAGE_SIZE - 1) / PAGE_SIZE;

    if (mem -> last != NULL) {
        mem -> last -> next = mem -> next;
        if ((((_size_t) mem -> last + mem -> last -> length - 1) & ~(PAGE_SIZE - 1)) == (_size_t) attr) {
            attr += PAGE_SIZE;
            num--;
        }
    }
    if (mem -> next != NULL) {
        mem -> next -> last = mem -> last;
        if ((((_size_t) mem + mem -> length -1) & ~(PAGE_SIZE - 1)) == ((_size_t) mem -> next & ~(PAGE_SIZE - 1))) {
            num--;
        }
    }
    else {
        pos = mem -> last;
    }

    if (num > 0) vmm_free_page(attr, num);

}
