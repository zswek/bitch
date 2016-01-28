// kernel/dev/disk.c

#include <driver/ata.h>
#include <kernel/dev/disk.h>
#include <kernel/dev/part.h>
#include <kernel/mm/alloc.h>
#include <debug.h>

typedef struct disk_t {
    _size_t id;
    _disk_dev_type_t dev_type;
    _size_t dev_id;
    _u64_t start_sector;
    _u64_t sector_count;
    _fs_t fs;
    _size_t part_id;
    _size_t part_count;
    struct disk_t* pdisk;
    struct disk_t* last;
    struct disk_t* next;
} _disk_t;

static _size_t new_id = 0;
static _disk_t* header = NULL;
static _disk_t* pos = NULL;

static _disk_t* get_disk(_size_t id) {

    _disk_t* disk = header;

    while (disk != NULL) {
        if (disk -> id == id) {
            return disk;
        }
        disk = disk -> next;
    }

    return NULL;
}

static void free_disk(_disk_t *disk) {

    if (disk == NULL) return;

    if (disk -> last == NULL)
        header = disk -> next;
    else
        disk -> last -> next = disk -> next;
    if (disk -> next == NULL)
        pos = disk -> last;
    else
        disk -> next -> last = disk -> last;

    if (disk -> pdisk != NULL) disk -> pdisk -> part_count--;

    fs_free(disk -> fs);

    kfree(disk);
}

_ssize_t get_disk_id(_size_t n) {

    _disk_t* disk = header;

    int i;
    for (i = 0; i < n; i++) {
        if (disk == NULL)
            return -1;
        disk = disk -> next;
    }

    if (disk == NULL)
        return -1;
    else
        return disk -> id;

}

_ssize_t get_disk_sector_size(_size_t id) {

    _disk_t* disk;

    if ((disk = get_disk(id)) == NULL) {
        return -1;
    }

    switch (disk -> dev_type) {
        case DISK_ATA:
            return ATA_SECTOR_SIZE;
        default:
            return -1;
    }

}

_ssize_t get_disk_sectors(_size_t id) {

    _disk_t* disk;

    if ((disk = get_disk(id)) == NULL) {
        return -1;
    }

    return disk -> sector_count;

}

_ssize_t disk_read(_size_t id, void* buf, _size_t count, _u64_t offect) {

    _disk_t* disk;

    if ((disk = get_disk(id)) == NULL) {
        return -1;
    }

    switch (disk -> dev_type) {
        case DISK_ATA:
            if (count + offect > disk -> sector_count * ATA_SECTOR_SIZE) {
                count = disk -> sector_count * ATA_SECTOR_SIZE - offect;
            }
            return ata_read(disk -> dev_id, buf, count, disk -> start_sector * ATA_SECTOR_SIZE + offect);
        default:
            return -1;
    }

}

_ssize_t disk_write(_size_t id, void* buf, _size_t count, _u64_t offect) {

    _disk_t* disk;

    if ((disk = get_disk(id)) == NULL) {
        return -1;
    }

    switch (disk -> dev_type) {
        case DISK_ATA:
            if (count + offect > disk -> sector_count * ATA_SECTOR_SIZE) {
                count = disk -> sector_count * ATA_SECTOR_SIZE - offect;
            }
            return ata_write(disk -> dev_id, buf, count, disk -> start_sector * ATA_SECTOR_SIZE + offect);
        default:
            return -1;
    }

}

_ssize_t disk_create(_disk_dev_type_t dev_type, _size_t dev_id, _u64_t sector_count) {

    _disk_t* disk = kmalloc(sizeof(_disk_t));

    if (disk == NULL) return -1;

    disk -> id = new_id++;

    disk -> dev_type = dev_type;
    disk -> dev_id = dev_id;
    disk -> start_sector = 0;
    disk -> sector_count = sector_count;

    disk -> part_id = 0;
    disk -> part_count = 0;

    disk -> pdisk = NULL;
    disk -> last = pos;
    disk -> next = NULL;
    if (pos != NULL) pos -> next = disk;
    pos = disk;
    if (header == NULL) header = disk;

    switch(dev_type) {
        case DISK_ATA:
            debug("Disk %d: ATA %d, %d sectors", disk -> id, dev_id, sector_count);
            break;
    }

    disk_part(disk -> id);

    disk -> fs = disk_fs_init(disk -> id);

    return disk -> id;
}

_ssize_t disk_mmap(_size_t id, _u64_t start_sector, _u64_t sector_count) {

    _disk_t *pdisk, *disk;

    if ((pdisk = get_disk(id)) == NULL) {
        return -1;
    }

    if (start_sector + sector_count > pdisk -> sector_count) {
        return -1;
    }

    if ((disk = kmalloc(sizeof(_disk_t))) == NULL) {
        return -1;
    }

    disk -> id = new_id++;

    disk -> dev_type = pdisk -> dev_type;
    disk -> dev_id = pdisk -> dev_id;
    disk -> start_sector = pdisk -> start_sector + start_sector;
    disk -> sector_count = sector_count;

    disk -> part_id = pdisk -> part_count ++;
    disk -> part_count = 0;

    disk -> pdisk = pdisk;
    disk -> last = pos;
    disk -> next = NULL;
    if (pos != NULL) pos -> next = disk;
    pos = disk;

    debug("Disk %d: Disk %d Part %d, %d sectors, start %d", disk -> id, pdisk -> id, disk -> part_id, (_u32_t) sector_count, (_u32_t) start_sector);

    disk -> fs = disk_fs_init(disk -> id);

    return disk -> id;
}

void disk_remove(_size_t id) {

    _disk_t *disk;

    if ((disk = get_disk(id)) != NULL) {

        disk_remove_part(id);

        free_disk(disk);
    }

}

void disk_remove_part(_size_t id) {

    _disk_t* disk = header;

    while (disk != NULL) {

        if ((disk -> pdisk != NULL) && (disk -> pdisk -> id == id)) {
            disk_remove_part(disk -> id);

            free_disk(disk);
        }

        disk = disk -> next;
    }

}

_file_t disk_readdir(_size_t id, const char* path, _size_t n) {

    _disk_t* disk;
    _file_t file;

    if ((disk = get_disk(id)) == NULL) {
        file.name_len = 0;
    }
    else {
        file = fs_readdir(disk -> fs, path, n);
    }

    return file;
}

_ssize_t disk_fread(_size_t id, const char* path, void* buf, _size_t count, _u64_t offect) {

    _disk_t* disk;

    if ((disk = get_disk(id)) == NULL) {
        return -1;
    }

    return fs_read(disk -> fs, path, buf, count, offect);
}

_ssize_t disk_fwrite(_size_t id, const char* path, void* buf, _size_t count, _u64_t offect) {

    _disk_t* disk;

    if ((disk = get_disk(id)) == NULL) {
        return -1;
    }

    return fs_write(disk -> fs, path, buf, count, offect);
}

void disk_init(void) {

    init_ata();

}
