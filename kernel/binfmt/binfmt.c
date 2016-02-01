// kernel/binfmt/binfmt.c

#include <kernel/binfmt/a.out.h>
#include <kernel/binfmt/elf.h>
#include <kernel/mm/alloc.h>
#include <kernel/process.h>
#include <debug.h>

void binfmt_exec(const char* path) {

    debug("Process %d: Binary Format (%s)", current_process -> pid, path);

    elf_exec(path);

    a_out_exec(path);

    debug("Process %d: Format Error (%s)", current_process -> pid, path);

    kfree((void*) path);

}
