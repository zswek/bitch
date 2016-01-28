// kernel/dev/file.h

#ifndef _KERNEL_DEV_FILE_H_
#define _KERNEL_DEV_FILE_H_

#include <bits/types.h>
#include <kernel/fs.h>

void set_disk(_size_t id);
_file_t file_readdir(const char* path, _size_t n);
_ssize_t file_read(const char* path, void* buf, _size_t count, _u64_t offect);
_ssize_t file_write(const char* path, void* buf, _size_t count, _u64_t offect);

#endif
