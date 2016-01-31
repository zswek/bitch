// kernel/kernel.c

#include <kernel/mm.h>
#include <kernel/init.h>
#include <kernel/intr.h>
#include <kernel/dev.h>
#include <kernel/syscall.h>
#include <kernel/process.h>
#include <debug.h>

void kernel_init(void) {

    console_init();

    debug("Kernel: Booting");

    debug("Kernel: Address 0x%08X~0x%08X, Used %d KB", _kernel_start, _kernel_end, (_kernel_end - _kernel_start + 1023) / 1024);

    mm_init();

    intr_init();

    dev_init();

    syscall_init();

    process_init();

}
