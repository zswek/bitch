// kernel/fs.h

#ifndef	_KERNEL_FS_H_
#define	_KERNEL_FS_H_

#include <bits/types.h>

typedef enum file_type_t {
    FILE_REG,
    FILE_DIR
} _file_type_t;

typedef struct file_t {
    char name[256];
    _size_t name_len;
    _file_type_t type;
} _file_t;

typedef struct fs_t {
    enum {
        FS_RAW,
        FS_FAT
    } type;
    void* info;
} _fs_t;

_file_t fs_readdir(_fs_t fs, const char* path, _size_t n);
_ssize_t fs_read(_fs_t fs, const char* path, void* buf, _size_t count, _u64_t offect);
_ssize_t fs_write(_fs_t fs, const char* path, void* buf, _size_t count, _u64_t offect);

void fs_free(_fs_t fs);

_fs_t disk_fs_init(_size_t id);

#endif