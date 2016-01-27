// kernel/fs/fat.c

#include <kernel/fs/fat.h>
#include <kernel/dev/disk.h>
#include <kernel/mm/alloc.h>
#include <lib/string.h>
#include <debug.h>

#define ATTR_READ_ONLY  0x01
#define ATTR_HIDDEN     0x02
#define ATTR_SYSTEM     0x04
#define ATTR_VOLUME_ID  0x08
#define ATTR_DIR        0x10
#define ATTR_ARCHIVE    0x20
#define ATTR_DEVICE     0x40
#define ATTR_LFN        0x0f

#define FLAG_DEL        0xe5
#define FLAG_LAST       0x40

typedef struct fat_ext_t {
    _u8_t dev_num;
    _u8_t reserved;
    _u8_t sign;
    _u32_t vol_id;
    _u8_t vol_label[11];
    _u8_t fat_type[8];
} __attribute__((packed)) _fat_ext_t;

typedef struct fat32_ext_t {
    _u32_t table_sectors;
    _u16_t flags;
    _u16_t version;
    _u32_t root_dir_cluster;
    _u16_t fsinfo_sector;
    _u16_t backup_boot_sector;
    _u8_t reserved[12];
    _fat_ext_t fat;
} __attribute__((packed)) _fat32_ext_t;

typedef struct fat_header_t {
    _u8_t oem_name[8];
    _u16_t sector_size;
    _u8_t cluster_sectors;
    _u16_t reserved_sectors;
    _u8_t table_count;
    _u16_t root_dir_count;
    _u16_t total_sectors;
    _u8_t media_type;
    _u16_t table_sectors;
    _u16_t track_sectors;
    _u16_t heads;
    _u32_t hidden_sectors;
    _u32_t large_sectors;
    union {
        _fat_ext_t fat_ext;
        _fat32_ext_t fat32_ext;
    } ext;
} __attribute__((packed)) _fat_header_t;

typedef struct fat_std_dir_t {
    _u8_t dos_name[8];
    _u8_t dos_ext_name[3];
    _u8_t attribute;
    _u8_t reserved;
    _u8_t creat_time_sec;
    _u16_t creat_time;
    _u16_t creat_date;
    _u16_t access_date;
    _u16_t first_cluster_high;
    _u16_t modify_time;
    _u16_t modify_date;
    _u16_t first_cluster_low;
    _u32_t file_size;
} __attribute__((packed)) _fat_std_dir_t;

typedef struct fat_long_name_t {
    _u8_t order;
    _u16_t first_char[5];
    _u8_t attribute;
    _u8_t reserved;
    _u8_t checksum;
    _u16_t next_char[6];
    _u16_t always0;
    _u16_t final_char[2];
} __attribute__((packed)) _fat_long_name_t;

typedef struct fat_dir_t {
    union {
        _fat_std_dir_t dir;
        _fat_long_name_t name;
    } dir;
} __attribute__((packed)) _fat_dir_t;

static _u32_t next_cluster(_fat_t* fat, _u32_t cluster) {
    
    _u32_t next = 0;
    
    switch (fat -> fat_type) {
        case FAT16:
            disk_read(fat -> disk_id, &next, sizeof(_u16_t), fat -> fat_sector * fat -> sector_size + cluster * sizeof(_u16_t));
            if (next < 0x2 || next > 0xffef) next = 0;
            break;
        case FAT32:
            disk_read(fat -> disk_id, &next, sizeof(_u32_t), fat -> fat_sector * fat -> sector_size + cluster * sizeof(_u32_t));
            if (next < 0x2 || next > 0xfffffef) next = 0;
            break;
    }
    
    return next;
}

static _size_t dir_cluster(_fat_t* fat, _u32_t cluster) {
    
    _size_t n = 1;
    
    while ((cluster = next_cluster(fat, cluster))) {n++;}
    
    return n;
}

static _ssize_t read_cluster(_fat_t* fat, void* buf, _u32_t cluster, _size_t count, _u64_t offect) {
    
    _ssize_t n = 0, i;
    _u64_t buf_offect = 0;
    
    do {
        if (offect < fat -> cluster_size) {
            if (offect + count > fat -> cluster_size) {
                i = disk_read(fat -> disk_id, (_u8_t*) buf + buf_offect, fat -> cluster_size - offect, fat -> start_cluster + (cluster - 2) * fat -> cluster_size + offect);
                if (i == -1) return -1;
                else n += i;
                count -= fat -> cluster_size - offect;
                offect = 0;
                buf_offect += fat -> cluster_size - offect;
            }
            else {
                i = disk_read(fat -> disk_id, (_u8_t*) buf + buf_offect, count, fat -> start_cluster + (cluster - 2) * fat -> cluster_size + offect);
                if (i == -1) return -1;
                else n += i;
                break;
            }
        }
        else {
            offect -= fat -> cluster_size;
        }
    } while ((cluster = next_cluster(fat, cluster)));
    
    return n;
}

static _fat_dir_t read_dir_entry(_fat_t* fat, _u32_t cluster, _size_t n) {
    
    _fat_dir_t dir;
    
    read_cluster(fat, &dir, cluster, sizeof(_fat_dir_t), n * sizeof(_fat_dir_t));
    
    return dir;
}

static _fat_dir_t read_root_dir_entry(_fat_t* fat, _size_t n) {
    
    _fat_dir_t dir;
    
    switch (fat -> fat_type) {
        case FAT16:
            disk_read(fat -> disk_id, &dir, sizeof(_fat_dir_t), fat -> root_dir_sector * fat -> sector_size + n * sizeof(_fat_dir_t));
            break;
        case FAT32:
            dir = read_dir_entry(fat, fat -> root_dir_cluster, n);
            break;
    }
    
    return dir;
}

static _u8_t lfn_checksum (const _u8_t *p)
{
	int i;
	_u8_t sum = 0;

	for (i = 11; i; i--)
		sum = ((sum & 1) ? 0x80 : 0) + (sum >> 1) + *p++;
	return sum;
}

static _u16_t lfn_char(_fat_long_name_t lfn, _size_t n) {
    
    if (n < 5) return lfn.first_char[n];
    else if (n < 11) return lfn.next_char[n - 5];
    else if (n < 13) return lfn.final_char[n - 11];
    else return 0;
    
}

static int lfn_cmp(const char* name, _fat_long_name_t lfn, _size_t len) {
    
    int n;
    for (n = 0; n < len; n++) {
        if (name[n] != lfn_char(lfn, n)) {
            return 1;
        }
    }
    
    return lfn_char(lfn, len);
}

static _size_t lfn_cpy(char* name, _fat_long_name_t lfn) {
    
    int n;
    for (n = 0; n < 13; n++) {
        name[n] = lfn_char(lfn, n);
        if (name[n] == 0) break;
    }
    
    return n;
}

static _fat_std_dir_t get_root_file(_fat_t* fat, const char* name, _size_t len) {
    
    _size_t n, i, lfn_count = (len + 12) / 13, last_len = (len % 13) ? (len % 13) : 13;
    _fat_dir_t entry;
    
    if (fat -> fat_type == FAT32) fat -> root_dir_count = fat -> cluster_size * dir_cluster(fat, fat -> root_dir_cluster) / sizeof(_fat_dir_t);
    
    for (n = 0; n < fat -> root_dir_count; n++) {
        entry = read_root_dir_entry(fat, n);
        
        if ((entry.dir.name.order != FLAG_DEL) && (entry.dir.name.attribute == ATTR_LFN) &&
        (entry.dir.name.order & FLAG_LAST) && ((entry.dir.name.order & 0x3f) == lfn_count) &&
        (lfn_cmp(name + (lfn_count - 1) * 13, entry.dir.name, last_len) == 0)) {
            for (i = lfn_count - 1; i > 0; i--) {
                entry = read_root_dir_entry(fat, n + lfn_count - i);
                if (!((entry.dir.name.order != FLAG_DEL) && (entry.dir.name.attribute == ATTR_LFN) &&
                !(entry.dir.name.order & FLAG_LAST) && ((entry.dir.name.order & 0x3f) == i) &&
                (lfn_cmp(name + (i - 1) * 13, entry.dir.name, 13) == 0))) {
                    break;
                }
            }
            if (i == 0) {
                entry = read_root_dir_entry(fat, n + lfn_count);
                if ((entry.dir.name.order != FLAG_DEL) && (entry.dir.name.attribute != ATTR_LFN)) {
                    return entry.dir.dir;
                }
            }
            else {
                n += lfn_count - i - 1;
            }
        }
    }
    
    entry.dir.name.order = FLAG_DEL;
    
    return entry.dir.dir;
}

static _fat_std_dir_t get_file(_fat_t* fat, _fat_std_dir_t dir, const char* name, _size_t len) {
    
    _size_t n, i, lfn_count = (len + 12) / 13, last_len = (len % 13) ? (len % 13) : 13;
    _fat_dir_t entry;
    
    if ((dir.dos_name[0] == FLAG_DEL) || !(dir.attribute & ATTR_DIR)) {
        entry.dir.name.order = FLAG_DEL;
        
        return entry.dir.dir;
    }
    
    _u32_t cluster = dir.first_cluster_low;
    if (fat -> fat_type == FAT32) cluster |= (dir.first_cluster_high << 0x10);
    _size_t dir_count = fat -> cluster_size * dir_cluster(fat, cluster) / sizeof(_fat_dir_t);
    
    for (n = 0; n < dir_count; n++) {
        entry = read_dir_entry(fat, cluster, n);
        
        if ((entry.dir.name.order != FLAG_DEL) && (entry.dir.name.attribute == ATTR_LFN) &&
        (entry.dir.name.order & FLAG_LAST) && ((entry.dir.name.order & 0x3f) == lfn_count) &&
        (lfn_cmp(name + (lfn_count - 1) * 13, entry.dir.name, last_len) == 0)) {
            for (i = lfn_count - 1; i > 0; i--) {
                entry = read_dir_entry(fat, cluster, n + lfn_count - i);
                if (!((entry.dir.name.order != FLAG_DEL) && (entry.dir.name.attribute == ATTR_LFN) &&
                !(entry.dir.name.order & FLAG_LAST) && ((entry.dir.name.order & 0x3f) == i) &&
                (lfn_cmp(name + (i - 1) * 13, entry.dir.name, 13) == 0))) {
                    break;
                }
            }
            if (i == 0) {
                entry = read_dir_entry(fat, cluster, n + lfn_count);
                if ((entry.dir.name.order != FLAG_DEL) && (entry.dir.name.attribute != ATTR_LFN)) {
                    return entry.dir.dir;
                }
            }
            else {
                n += lfn_count - i - 1;
            }
        }
    }
    
    entry.dir.name.order = FLAG_DEL;
    
    return entry.dir.dir;
}

static _file_t read_root_dir(_fat_t* fat, _size_t offect) {
    
    _size_t n, i;
    _fat_dir_t entry;
    _file_t file;
    
    if (fat -> fat_type == FAT32) fat -> root_dir_count = fat -> cluster_size * dir_cluster(fat, fat -> root_dir_cluster) / sizeof(_fat_dir_t);
    
    for (n = 0; n < fat -> root_dir_count; n++) {
        entry = read_root_dir_entry(fat, n);
        
        if ((entry.dir.name.order != FLAG_DEL) && (entry.dir.name.attribute == ATTR_LFN) &&
        (entry.dir.name.order & FLAG_LAST)) {
            _size_t lfn_count = (entry.dir.name.order & 0x3f);
            if (offect == 0) {
                file.name_len = (lfn_count - 1) * 13 + lfn_cpy((char *) &file.name + (lfn_count - 1) * 13, entry.dir.name);
                for (i = lfn_count - 1; i > 0; i--) {
                    entry = read_root_dir_entry(fat, n + lfn_count - i);
                    if (!((entry.dir.name.order != FLAG_DEL) && (entry.dir.name.attribute == ATTR_LFN) &&
                    !(entry.dir.name.order & FLAG_LAST))) {
                        break;
                    }
                    lfn_cpy((char *) &file.name + ((entry.dir.name.order & 0x3f) - 1) * 13, entry.dir.name);
                }
                if (i == 0) {
                    entry = read_root_dir_entry(fat, n + lfn_count);
                    if ((entry.dir.name.order != FLAG_DEL) && (entry.dir.name.attribute != ATTR_LFN)) {
                        if ((entry.dir.name.attribute & 0xf0) == ATTR_DIR) {
                            file.type = FILE_DIR;
                            return file;
                        }
                        if ((entry.dir.name.attribute & 0xf0) == ATTR_ARCHIVE) {
                            file.type = FILE_REG;
                            return file;
                        }
                        if ((entry.dir.name.attribute & 0xf0) == 0) {
                            file.type = FILE_REG;
                            return file;
                        }
                    }
                }
            }
            else {
                n += lfn_count;
                offect--;
            }
        }
    }
    
    file.name_len = 0;
    
    return file;
}

static _file_t read_dir(_fat_t* fat, _fat_std_dir_t dir, _size_t offect){
    
    _size_t n, i;
    _fat_dir_t entry;
    _file_t file;
    
    if ((dir.dos_name[0] == FLAG_DEL) || !(dir.attribute & ATTR_DIR)) {
        file.name_len = 0;
        
        return file;
    }
    
    _u32_t cluster = dir.first_cluster_low;
    if (fat -> fat_type == FAT32) cluster |= (dir.first_cluster_high << 0x10);
    _size_t dir_count = fat -> cluster_size * dir_cluster(fat, cluster) / sizeof(_fat_dir_t);
    
    for (n = 0; n < dir_count; n++) {
        entry = read_dir_entry(fat, cluster, n);
        
        if ((entry.dir.name.order != FLAG_DEL) && (entry.dir.name.attribute == ATTR_LFN) &&
        (entry.dir.name.order & FLAG_LAST)) {
            _size_t lfn_count = (entry.dir.name.order & 0x3f);
            if (offect == 0) {
                file.name_len = (lfn_count - 1) * 13 + lfn_cpy((char *) &file.name + (lfn_count - 1) * 13, entry.dir.name);
                for (i = lfn_count - 1; i > 0; i--) {
                    entry = read_dir_entry(fat, cluster, n + lfn_count - i);
                    if (!((entry.dir.name.order != FLAG_DEL) && (entry.dir.name.attribute == ATTR_LFN) &&
                    !(entry.dir.name.order & FLAG_LAST))) {
                        break;
                    }
                    lfn_cpy((char *) &file.name + ((entry.dir.name.order & 0x3f) - 1) * 13, entry.dir.name);
                }
                if (i == 0) {
                    entry = read_dir_entry(fat, cluster, n + lfn_count);
                    if ((entry.dir.name.order != FLAG_DEL) && (entry.dir.name.attribute != ATTR_LFN)) {
                        if ((entry.dir.name.attribute & 0xf0) == ATTR_DIR) {
                            file.type = FILE_DIR;
                            return file;
                        }
                        if ((entry.dir.name.attribute & 0xf0) == ATTR_ARCHIVE) {
                            file.type = FILE_REG;
                            return file;
                        }
                    }
                }
            }
            else {
                n += lfn_count;
                offect--;
            }
        }
    }
    
    file.name_len = 0;
    
    return file;
}

static _ssize_t read_file(_fat_t* fat, _fat_std_dir_t file, void* buf, _size_t count, _u64_t offect){
    
    if ((file.dos_name[0] == FLAG_DEL) || !(file.attribute & ATTR_ARCHIVE)) {
        return -1;
    }
    
    if (offect >= file.file_size) return 0;
    if (offect + count > file.file_size) count = file.file_size - offect;
    
    _u32_t cluster = file.first_cluster_low;
    if (fat -> fat_type == FAT32) cluster |= (file.first_cluster_high << 0x10);
    
    return read_cluster(fat, buf, cluster, count, offect);
}

static _fat_std_dir_t get_path(_fat_t* fat, char* path) {
    
    _size_t len;
    _fat_std_dir_t file;
    file.reserved = '/';
    
    while (strchr(path, '/')) {
        len = strchr(path, '/') - path;
        
        if (len != 0) {
            if (file.reserved == '/') {
                file = get_root_file(fat, path, len);
            }
            else {
                file = get_file(fat, file, path, len);
            }
            if (file.dos_name[0] == FLAG_DEL) return file;
        }
        
        path += len + 1;
    }
    
    len = strlen(path);
    
    if (len != 0) {
        if (file.reserved == '/') {
            file = get_root_file(fat, path, len);
        }
        else {
            file = get_file(fat, file, path, len);
        }
        if (file.dos_name[0] == FLAG_DEL) return file;
    }
    
    return file;
}

_file_t fat_readdir(_fat_t* fat, const char* path, _size_t n) {
    
    _fat_std_dir_t dir = get_path(fat, path);
    
    if (dir.reserved == '/') {
        return read_root_dir(fat, n);
    }
    else {
        return read_dir(fat, dir, n);
    }
    
}

_ssize_t fat_read(_fat_t* fat, const char* path, void* buf, _size_t count, _u64_t offect) {
    
    _fat_std_dir_t file = get_path(fat, path);
    
    if (file.reserved == '/') {
        return -1;
    }
    else {
        return read_file(fat, file, buf, count, offect);
    }
    
}

_ssize_t fat_write(_fat_t* fat, const char* path, void* buf, _size_t count, _u64_t offect) {
    
    return -1;
}

void fat_free(_fat_t* fat) {
    
    if (fat != NULL) {
        kfree(fat);
    }
}

_fat_t* fat_init(_size_t id) {
    
    _fat_header_t header;
    _fat_t fat, *p = NULL;
    
    fat.disk_id = id;
    if (disk_read(id, &header,  sizeof(_fat_header_t), 3) != -1) {
        if (memcmp(header.ext.fat_ext.fat_type, "FAT16   ", 8) == 0) {
            debug("Disk %d: FAT16 filesystem", id);
            fat.fat_type = FAT16;
            fat.root_dir_sector = header.reserved_sectors + header.table_sectors * header.table_count;
            fat.start_cluster = fat.root_dir_sector * header.sector_size + header.root_dir_count * sizeof(_fat_dir_t);
        }
        else if (memcmp(header.ext.fat32_ext.fat.fat_type, "FAT32   ", 8) == 0) {
            debug("Disk %d: FAT32 filesystem", id);
            fat.fat_type = FAT32;
            fat.start_cluster = (header.reserved_sectors + header.ext.fat32_ext.table_sectors * header.table_count) * header.sector_size;
            fat.root_dir_sector = header.reserved_sectors + header.ext.fat32_ext.table_sectors * header.table_count + (header.ext.fat32_ext.root_dir_cluster - 2) * header.cluster_sectors;
            fat.root_dir_cluster = header.ext.fat32_ext.root_dir_cluster;
        }
        else
            return NULL;
    }
    
    fat.sector_size = header.sector_size;
    fat.cluster_size = header.cluster_sectors * header.sector_size;
    fat.fat_sector = header.reserved_sectors;
    fat.root_dir_count = header.root_dir_count;
    
    p = kmalloc(sizeof(_fat_t));
    
    if (p != NULL) *p = fat;
    
    return p;
}