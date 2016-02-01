// kernel/mm/pmm.c

#include <kernel/init.h>
#include <kernel/def.h>
#include <kernel/mm/mmap.h>
#include <kernel/process/lock.h>
#include <debug.h>

static _u8_t* pmm_bitmap;
static _size_t pmm_bitmap_length;

static _size_t pmm_free_page_count;

static _size_t pmm_head_pos;
static _size_t pmm_tail_pos;

//static _lock_t lock;

static _i8_t set_bit_mask(_size_t index, _u8_t mask) {
    if (index < pmm_bitmap_length * 8) {
        if (mask == 0) {
            pmm_bitmap[index / 8] &= ~(0x1 << (index % 8));
        }
        else {
            pmm_bitmap[index / 8] |= (0x1 << (index % 8));
        }
        return 0;
    }
    else {
        return -1;
    }
}

static _i8_t get_bit_mask(_size_t index) {
    if (index < pmm_bitmap_length * 8) {
        return ((pmm_bitmap[index / 8] >> (index % 8)) & 0x1);
    }
    else {
        return -1;
    }
}

_size_t get_pmm_free_count(void) {
    return pmm_free_page_count;
}

void* pmm_alloc_page(void) {
    //process_lock(&lock);
    for (; pmm_head_pos < pmm_tail_pos; pmm_tail_pos--) {
        if (get_bit_mask(pmm_tail_pos) == 0) {
            set_bit_mask(pmm_tail_pos, 1);
            pmm_tail_pos--;
            pmm_free_page_count--;
            //process_unlock(&lock);
            return (void*) ((pmm_tail_pos + 1) * PAGE_SIZE);
        }
    }
    //process_unlock(&lock);
    debug("Memory: Physical Memory Insufficient");
    return NULL;
}

void* pmm_up_alloc_page(_size_t count) {
    _size_t p;
    //process_lock(&lock);
    for (p = pmm_head_pos; p < pmm_tail_pos; p++) {
        _size_t i;
        for (i = 0; i < count; i++) {
            if (get_bit_mask(p + i) != 0)
                break;
        }
        if (i == count) {
            for (i = 0; i < count; i++) {
                set_bit_mask(p + i, 1);
            }
            if (p == pmm_head_pos) {
                pmm_head_pos += i;
            }
            pmm_free_page_count -= count;
            //process_unlock(&lock);
            return (void*) (p * PAGE_SIZE);
        }
        else {
            p += i;
        }
    }
    //process_unlock(&lock);
    debug("Memory: Physical Memory Insufficient");
    return NULL;
}

void pmm_free_page(void* attr, _size_t count) {
    _size_t i, index = (_size_t) attr / PAGE_SIZE;
    for (i = 0; i < count; i++) {
        set_bit_mask(index + i, 0);
    }
    if (index < pmm_head_pos) pmm_head_pos = index;
    if (index + count - 1 > pmm_tail_pos)
        pmm_tail_pos = index + count - 1;
    pmm_free_page_count += count;
}

void pmm_init(void) {

    pmm_bitmap = _mem_base_addr;

    pmm_bitmap_length =  (mem_page_count + 7) / 8;

    _mem_base_addr += pmm_bitmap_length;

    pmm_free_page_count = 0;

    _size_t i, mem_base_page = ((_size_t) _mem_base_addr + PAGE_SIZE - 1) / PAGE_SIZE;
    for (i = 0; i < pmm_bitmap_length * 8; i++) {
        if (i < mem_base_page || i >= mem_page_count) {
            set_bit_mask(i, 1);
        }
        else {
            set_bit_mask(i, 0);
            pmm_free_page_count++;
        }
    }

    pmm_head_pos = mem_base_page;
    pmm_tail_pos = mem_page_count - 1;

    debug("Memory: %d MB, Total %d pages, Free %d pages", (_mem_length + 1023 * 1024) / (1024 * 1024), mem_page_count, pmm_free_page_count);

    //init_process_lock(&lock);

}
