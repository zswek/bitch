// kernel/syscall/process.c

#include <kernel/intr/stub.h>
#include <kernel/mm/umm.h>
#include <kernel/process.h>
#include <kernel/process/exec.h>

void sys_exit(void) {
    exec_exit();
}

_size_t sys_exec(char *path, _size_t len) {
    path = umm_mmap_kmm(path, len, current_process -> pde);
    if (path == NULL) return 0;
    _size_t pid = exec(path);
    umm_ummap_kmm(path, len);
    return pid;
}
