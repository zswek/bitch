// kernel/process/exec.c

#include <arch/x86/cpu.h>
#include <kernel/mm/kmm.h>
#include <kernel/mm/umm.h>
#include <kernel/dev/disk.h>
#include <kernel/dev/file.h>
#include <kernel/binfmt.h>
#include <kernel/process.h>
#include <debug.h>

static char init_exec[] = "init";

static void exit(void) {

    current_process -> pde_phy = kernel_mmap_pde;
    load_pde(kernel_mmap_pde);
    umm_free(current_process -> pde);

    process_exit();
}

static void user_isr_handler(_intr_regs_t *regs) {

    enable_intr();

    exit();
}

_size_t exec(const char* path) {

    _pde_t* pde = umm_alloc_pde();

    if (pde == NULL) return 0;

    _size_t pid = process_create(binfmt_exec, path, exit, pde);

    if (pid == 0) umm_free(pde);

    return pid;
}

void exec_init(void) {

    reg_user_isr_handler(user_isr_handler);

    _size_t i;
    _ssize_t id;
    for (i = 0; ; i++) {
        id = get_disk_id(i);
        if (id == -1) break;
        if (disk_fread(id, init_exec, NULL, 0, 0) != -1) {
            debug("Disk %d: Found Init Process(%s)", id, init_exec);
            set_disk(id);
            exec(init_exec);
            return;
        }
    }

    debug("Exec: Not Found Init Process(%s)", init_exec);
}
