// kernel/process.h

#ifndef	_KERNEL_PROCESS_H_
#define	_KERNEL_PROCESS_H_

#include <bits/types.h>
#include <kernel/intr/stub.h>
#include <arch/x86/page.h>
#include <kernel/process/lock.h>

typedef
struct context_t {
        _u32_t esp;
        _u32_t ebp;
        _u32_t eflags;
} _context_t;

typedef
struct process_t {
    enum {
        RUN = 0,
        EXIT = 1,
        LOCK = 2,
        WAIT = 3
    } state;
    _size_t pid;
    _u32_t* stack_top;
    _pde_t* pde;
    _context_t context;
    _lock_t* lock;
    _size_t tick;
    struct process_t* next;
} _process_t;

extern _process_t* current_process;

_size_t process_create(void (*fn)(void*), void *arg, _pde_t* pde);
void process_exit(void);

void process_init(void);

#endif