// kernel/syscall/process.c

#include <kernel/intr/stub.h>
#include <kernel/process/exec.h>

void sys_exit(void) {
    exec_exit();
}
