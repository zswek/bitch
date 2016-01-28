// kernel/dev/file.c

#include <kernel/dev/disk.h>

static _size_t disk_id = 0;

void set_disk(_size_t id) {
    disk_id = id;
}

_file_t file_readdir(const char* path, _size_t n) {
    return disk_readdir(disk_id, path, n);
}

_ssize_t file_read(const char* path, void* buf, _size_t count, _u64_t offect) {
    return disk_fread(disk_id, path, buf, count, offect);
}

_ssize_t file_write(const char* path, void* buf, _size_t count, _u64_t offect) {
    return disk_fwrite(disk_id, path, buf, count, offect);
}
