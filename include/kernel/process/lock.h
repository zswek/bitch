// kernel/process/lock.h

#ifndef	_KERNEL_PROCESS_LOCK_H_
#define	_KERNEL_PROCESS_LOCK_H_

#include <bits/types.h>

#define init_process_lock(lock)     (*(lock) = 0)
#define init_intr_lock(lock)        (*(lock) = 1)
#define process_unlock(lock)        (*(lock) = 0)
#define intr_unlock(lock)           (*(lock) = 0)

typedef _u8_t   _lock_t;

void process_lock(_lock_t* lock);

void intr_lock(_lock_t* lock);

#endif