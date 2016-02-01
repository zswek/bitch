// kernel/process/exec.h

#ifndef	_KERNEL_PROCESS_EXEC_H_
#define	_KERNEL_PROCESS_EXEC_H_

_size_t exec(const char* path);

void exec_exit(void);

void exec_init(void);

#endif
