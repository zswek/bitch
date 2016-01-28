// kernel/dev/disk.h

#ifndef _KERNEL_DEV_DISK_H_
#define _KERNEL_DEV_DISK_H_

#include <bits/types.h>
#include <kernel/fs.h>

typedef enum disk_dev_type_t {
    DISK_ATA
} _disk_dev_type_t;

_ssize_t get_disk_id(_size_t n);

_ssize_t get_disk_sector_size(_size_t id);
_ssize_t get_disk_sectors(_size_t id);

_ssize_t disk_read(_size_t id, void* buf, _size_t count, _u64_t offect);
_ssize_t disk_write(_size_t id, void* buf, _size_t count, _u64_t offect);

_ssize_t disk_create(_disk_dev_type_t dev_type, _size_t dev_id, _u64_t sector_num);

_ssize_t disk_mmap(_size_t id, _u64_t start_sector, _u64_t sector_num);

void disk_remove(_size_t id);
void disk_remove_part(_size_t id);

_file_t disk_readdir(_size_t id, const char* path, _size_t n);
_ssize_t disk_fread(_size_t id, const char* path, void* buf, _size_t count, _u64_t offect);
_ssize_t disk_fwrite(_size_t id, const char* path, void* buf, _size_t count, _u64_t offect);

void disk_init(void);

#endif
