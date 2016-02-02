// kernel/syscall/io.c

#include <arch/x86/cpu.h>
#include <kernel/intr/stub.h>
#include <kernel/dev/console.h>
#include <kernel/dev/keyboard.h>
#include <kernel/mm/umm.h>
#include <kernel/process.h>
#include <kernel/process/lock.h>

static char *key_c = NULL;
static _size_t key_len = 0;

static _lock_t *key_lock = NULL;

static void key_get(_u16_t key) {
    if ((key_len > 0) && (key_c != NULL)) {
        *(key_c++) = (char) key;
        key_len--;
    }
    if ((key_len == 0) && (key_lock != NULL)) {
        intr_unlock(key_lock);
    }
}

_size_t sys_get(char *c, _size_t len) {
    c = umm_mmap_kmm(c, len, current_process -> pde);
    if (c == NULL) return 0;

    while (key_len > 0) {
        halt();
    }
    _lock_t lock;
    key_lock = &lock;
    key_c = c;
    key_len = len;
    init_intr_lock(&lock);
    set_input_mode(2);
    reg_key_callback(key_get);
    intr_lock(&lock);

    if (key_lock == &lock) key_lock = NULL;

    umm_ummap_kmm(c, len);
    return len;
}

_size_t sys_put(char *c, _size_t len) {
    c = umm_mmap_kmm(c, len, current_process -> pde);
    if (c == NULL) return 0;
    _size_t i;
    for (i = 0; i < len; i++)
        console_putchar(c[i]);
    umm_ummap_kmm(c, len);
    return len;
}
