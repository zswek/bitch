// kernel/binfmt/binfmt.c

#include <kernel/binfmt/a.out.h>
#include <kernel/binfmt/elf.h>
#include <kernel/process.h>
#include <debug.h>

void binfmt_exec(const char* path) {

    debug("Process %d: binfmt(%s)", current_process -> pid, path);

    elf_exec(path);

    a_out_exec(path);

}
