// kernel/dev/part/mbr.c

#include <asm/types.h>
#include <kernel/dev/disk.h>
#include <kernel/dev/part/gpt.h>
#include <debug.h>

#define TYPE_EMPTY  0x00
#define TYPE_EXT    0x05
#define TYPE_GPT    0xee

typedef struct mbr_part_t {
    _u8_t status;
    _u8_t start_head;
    _u16_t start_sector;
    _u8_t part_type;
    _u8_t end_head;
    _u16_t end_sector;
    _u32_t first_sector_lba;
    _u32_t sector_num;
} __attribute__((packed)) _mbr_part_t;

typedef struct mbr_t {
    _mbr_part_t part[4];
    _u16_t sign;
} __attribute__((packed)) _mbr_t;

_size_t mbr_part(_size_t id) {
    
    _mbr_t mbr;
    _size_t i, n = 0;
    
    if ((disk_read(id, &mbr, sizeof(_mbr_t), 0x1be) != -1) && (mbr.sign == 0xaa55)) {
        debug("Disk %d: Boot signature", id);
        for (i = 0; i < 4; i++) {
            if (mbr.part[i].part_type == TYPE_EXT) {
                debug("Disk %d: DOS Partition %d, Extended", id, i);
                n += mbr_part(disk_mmap(id, mbr.part[i].first_sector_lba, mbr.part[i].sector_num));
                n++;
            }
            else if (mbr.part[i].part_type == TYPE_GPT) {
                n += gpt_part(id);
            }
            else if (mbr.part[i].part_type != TYPE_EMPTY) {
                debug("Disk %d: DOS Partition %d, Id %02X", id, i, mbr.part[i].part_type);
                disk_mmap(id, mbr.part[i].first_sector_lba, mbr.part[i].sector_num);
                n++;
            }
        }
    }
    
    return n;
    
}