// kernel/syscall/io.c

#include <kernel/intr/stub.h>
#include <kernel/dev/console.h>
#include <kernel/mm/umm.h>
#include <kernel/process.h>

_size_t sys_put(char *c, _size_t len) {
    c = umm_mmap_kmm(c, len, current_process -> pde);
    if (c == NULL) return 0;
    _size_t i;
    for (i = 0; i < len; i++)
        console_putchar(c[i]);
    umm_ummap_kmm(c, len);
    return len;
}
