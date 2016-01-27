// kernel/fs/fat.h

#ifndef	_KERNEL_FS_FAT_H_
#define	_KERNEL_FS_FAT_H_

#include <kernel/fs.h>

typedef struct fat_t {
    _size_t disk_id;
    enum {
        FAT16,
        FAT32
    } fat_type;
    _size_t sector_size;
    _size_t cluster_size;
    _u64_t start_cluster;
    _u64_t fat_sector;
    _u64_t root_dir_cluster;
    _u64_t root_dir_sector;
    _size_t root_dir_count;
} _fat_t;

_file_t fat_readdir(_fat_t* fat, const char* path, _size_t n);
_ssize_t fat_read(_fat_t* fat, const char* path, void* buf, _size_t count, _u64_t offect);
_ssize_t fat_write(_fat_t* fat, const char* path, void* buf, _size_t count, _u64_t offect);

void fat_free(_fat_t* fat);

_fat_t* fat_init(_size_t id);

#endif