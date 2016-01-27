// driver/ata.h

#ifndef _DRIVER_ATA_H_
#define _DRIVER_ATA_H_

#include <bits/types.h>

#define ATA_INT     IRQ14_INT

#define ATA_SECTOR_SIZE     0x200

_ssize_t ata_read(_u8_t n, void* buf, _size_t count, _u64_t offect);
_ssize_t ata_write(_u8_t n, void* buf, _size_t count, _u64_t offect);

_u64_t get_ata_sectors(_u8_t n);

void init_ata(void);

#endif