// kernel/syscall/process.h

#ifndef _KERNEL_SYSCALL_PROCESS_H_
#define _KERNEL_SYSCALL_PROCESS_H_

void sys_exit(void);

_size_t sys_exec(char *path, _size_t len);

#endif
