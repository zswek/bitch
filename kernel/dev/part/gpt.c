// kernel/dev/part/gpt.c

#include <bits/types.h>
#include <kernel/dev/disk.h>
#include <debug.h>

typedef struct gpt_header_t {
    _u64_t sign;
    _u32_t revision;
    _u32_t header_size;
    _u32_t header_crc32;
    _u32_t reserved;
    _u64_t current_lba;
    _u64_t backup_lba;
    _u64_t first_part_lba;
    _u64_t last_part_lba;
    _u8_t disk_guid[16];
    _u64_t start_part_entries_lba;
    _u32_t part_entries_num;
    _u32_t part_entry_size;
    _u32_t part_array_crc32;
} __attribute__((packed)) _gpt_header_t;

typedef struct gpt_entry_t {
    _u8_t type_guid[16];
    _u8_t part_guid[16];
    _u64_t first_lba;
    _u64_t last_lba;
    _u64_t flags;
    _u16_t part_name[36];
} __attribute__((packed)) _gpt_entry_t;

_size_t gpt_part(_size_t id) {
    
    _gpt_header_t header;
    _gpt_entry_t entry;
    _size_t i, n = 0;
    
    if ((disk_read(id, &header, sizeof(_gpt_header_t), get_disk_sector_size(id)) != -1) && (header.sign == 0x5452415020494645)) {
        debug("Disk %d: GPT Partition Table", id);
        for (i = 0; i < header.part_entries_num; i++) {
            if((disk_read(id, &entry, sizeof(_gpt_entry_t), get_disk_sector_size(id) * header.start_part_entries_lba + i * header.part_entry_size) != -1) && (entry.first_lba != 0)) {
                debug("Disk %d: GPT Partition %d", i);
                disk_mmap(id, entry.first_lba, entry.last_lba - entry.first_lba + 1);
                n++;
            }
        }
    }
    
    return n;
}