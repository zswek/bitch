// kernel/fs/fs.c

#include <kernel/fs.h>
#include <kernel/fs/fat.h>

_file_t fs_readdir(_fs_t fs, const char* path, _size_t n) {
    
    _file_t file;
    switch (fs.type) {
        case FS_FAT:
            file = fat_readdir(fs.info, path, n);
            break;
        default:
            file.name_len = 0;
    }
    
    return file;
}

_ssize_t fs_read(_fs_t fs, const char* path, void* buf, _size_t count, _u64_t offect) {
    
    switch (fs.type) {
        case FS_FAT:
            return fat_read(fs.info, path, buf, count, offect);
        default:
            return -1;
    }
}

_ssize_t fs_write(_fs_t fs, const char* path, void* buf, _size_t count, _u64_t offect) {
    
    switch (fs.type) {
        case FS_FAT:
            return fat_write(fs.info, path, buf, count, offect);
        default:
            return -1;
    }
}

void fs_free(_fs_t fs) {
    
    switch (fs.type) {
        default:
            return;
    }
}

_fs_t disk_fs_init(_size_t id) {
    
    _fs_t fs;
    void* info = NULL;
    
    if ((info = fat_init(id)) != NULL) {
        fs.type = FS_FAT;
    }
    else {
        fs.type = FS_RAW;
    }
    
    return fs;
    
}