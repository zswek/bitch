// kernel/process/exec.c

#include <arch/x86/cpu.h>
#include <kernel/mm/kmm.h>
#include <kernel/mm/umm.h>
#include <kernel/dev/disk.h>
#include <kernel/dev/file.h>
#include <kernel/intr/isr.h>
#include <kernel/binfmt.h>
#include <kernel/process.h>
#include <debug.h>

static char init_exec[] = "init";

void exec_exit(void) {

    _pde_t* pde = current_process -> pde;
    current_process -> pde = kernel_mmap_pde;
    load_pde(kernel_mmap_pde);
    umm_free(pde);

    process_exit();
}

static void user_isr_handler(_intr_regs_t *regs) {

    if ((regs -> int_code == 0x0e ) && (umm_handler(current_process -> pde) != 0)) return;

    enable_intr();

    debug("Process %d: Fault 0x%x, Error Code 0x%x, 0x%x", current_process -> pid, regs -> int_code, regs -> err_code, get_page_fault_attr());

    exec_exit();
}

_size_t exec(const char* path) {

    _pde_t* pde = umm_alloc_pde();

    if (pde == NULL) return 0;

    _size_t pid = process_create(binfmt_exec, path, exec_exit, pde);

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
            debug("Disk %d: Found Init Process (%s)", id, init_exec);
            set_disk(id);
            exec(init_exec);
            return;
        }
    }

    debug("Exec: Not Found Init Process (%s)", init_exec);
}
