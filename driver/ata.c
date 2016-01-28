// driver/ata.c

#include <arch/x86/io.h>
#include <driver/ata.h>
#include <driver/i8259.h>
#include <kernel/dev/disk.h>
#include <kernel/process/lock.h>
#include <debug.h>

#define ATA_P_DATA_IO   0x1f0
#define ATA_P_ERR_IO    0x1f1
#define ATA_P_SC_IO     0x1f2
#define ATA_P_LBA_L_IO  0x1f3
#define ATA_P_LBA_M_IO  0x1f4
#define ATA_P_LBA_H_IO  0x1f5
#define ATA_P_DEV_IO    0x1f6
#define ATA_P_STAT_IO   0x1f7
#define ATA_P_COM_IO    0x1f7
#define ATA_P_AS_IO     0x3f6
#define ATA_P_DC_IO     0x3f6

#define ATA_S_DATA_IO   0x170
#define ATA_S_ERR_IO    0x171
#define ATA_S_SC_IO     0x172
#define ATA_S_LBA_L_IO  0x173
#define ATA_S_LBA_M_IO  0x174
#define ATA_S_LBA_H_IO  0x175
#define ATA_S_DEV_IO    0x176
#define ATA_S_STAT_IO   0x177
#define ATA_S_COM_IO    0x177
#define ATA_S_AS_IO     0x376
#define ATA_S_DC_IO     0x376

#define ERR_REG_AMNF    0x01
#define ERR_REG_TKONF   0x02
#define ERR_REG_ABRT    0x04
#define ERR_REG_MAC     0x08
#define ERR_REG_IDNF    0x10
#define ERR_REG_MC      0x20
#define ERR_REG_UNC     0x40
#define ERR_REG_BBK     0x80

#define DEV_REG         0xa0
#define DEV_REG_L       0xe0
#define DEV_REG_DRV     0xb0

#define STAT_REG_ERR    0x01
#define STAT_REG_IDX    0x02
#define STAT_REG_CORR   0x04
#define STAT_REG_DRQ    0x08
#define STAT_REG_DSC    0x10
#define STAT_REG_DF     0x20
#define STAT_REG_DRDY   0x40
#define STAT_REG_BSY    0x80

#define DC_REG_IEN      0x0a
#define DC_REG_SRST     0x0c

#define COM_READ_SECTORS        0x20
#define COM_READ_SECTORS_EXT    0x24
#define COM_WRITE_SECTORS       0x30
#define COM_WRITE_SECTORS_EXT   0x34
#define COM_IDENTIFY            0xec

#define FLAGS_NOHISK    0x0
#define FLAGS_CHS       0x1
#define FLAGS_LBA28     0x2
#define FLAGS_LBA48     0x3

typedef enum ata_state_t {
    NODISK = 0,
    CHS = 1,
    LBA28 = 2,
    LBA48 = 3
} _ata_state_t;

typedef struct ata_io_t {
    _u8_t   n;
    _u16_t  data_io;
    _u16_t  error_io;
    _u16_t  sector_count_io;
    _u16_t  lba_low_io;
    _u16_t  lba_mid_io;
    _u16_t  lba_high_io;
    _u16_t  device_io;
    _u16_t  status_io;
    _u16_t  command_io;
    _u16_t  alternate_status_io;
    _u16_t  device_contorl_io;
    _u8_t   device_reg;
    _ata_state_t state;
    _lock_t* ilock;
} _ata_io_t;

static _u8_t ata_state;

static _u64_t ata_sectors[4];

static _u8_t buf_1[ATA_SECTOR_SIZE];
static _u8_t buf_2[ATA_SECTOR_SIZE];

static _lock_t lock_p;
static _lock_t lock_s;
static _lock_t ilock_p;
static _lock_t ilock_s;

static void irq14_handler(_intr_regs_t *regs) {

    if ((inb(ATA_P_STAT_IO) & STAT_REG_BSY) == 0) {
        intr_unlock(&ilock_p);
    }

}

static void irq15_handler(_intr_regs_t *regs) {

    if ((inb(ATA_S_STAT_IO) & STAT_REG_BSY) == 0) {
        intr_unlock(&ilock_s);
    }

}

static _ata_io_t ata_io_lock(_u8_t n) {

    _ata_io_t io;

    io.n = n;
    if (n % 2) {
        io.device_reg = DEV_REG_DRV;
    }
    else {
        io.device_reg = DEV_REG;
    }
    io.state = (ata_state >> (n * 2)) & 0x3;
    if (n < 2) {
        io.data_io = ATA_P_DATA_IO;
        io.error_io = ATA_P_ERR_IO;
        io.sector_count_io = ATA_P_SC_IO;
        io.lba_low_io = ATA_P_LBA_L_IO;
        io.lba_mid_io = ATA_P_LBA_M_IO;
        io.lba_high_io = ATA_P_LBA_H_IO;
        io.device_io = ATA_P_DEV_IO;
        io.status_io = ATA_P_STAT_IO;
        io.command_io = ATA_P_COM_IO;
        io.alternate_status_io = ATA_P_AS_IO;
        io.device_contorl_io = ATA_P_DC_IO;
        io.ilock = &ilock_p;
        process_lock(&lock_p);
    }
    else if (n < 4) {
        io.data_io = ATA_S_DATA_IO;
        io.error_io = ATA_S_ERR_IO;
        io.sector_count_io = ATA_S_SC_IO;
        io.lba_low_io = ATA_S_LBA_L_IO;
        io.lba_mid_io = ATA_S_LBA_M_IO;
        io.lba_high_io = ATA_S_LBA_H_IO;
        io.device_io = ATA_S_DEV_IO;
        io.status_io = ATA_S_STAT_IO;
        io.command_io = ATA_S_COM_IO;
        io.alternate_status_io = ATA_S_AS_IO;
        io.device_contorl_io = ATA_S_DC_IO;
        io.ilock = &ilock_s;
        process_lock(&lock_s);
    }

    return io;
}

static void ata_io_unlock(_u8_t n) {

    if (n < 2) {
        process_unlock(&lock_p);
    }
    else if (n < 4) {
        process_unlock(&lock_s);
    }

}

static void init_status(void) {

    ata_state = 0;

    int n;
    for (n = 0; n < 4; n++) {

        _ata_io_t io = ata_io_lock(n);

        _ata_state_t state = NODISK;

        ata_sectors[n] = 0;

        init_intr_lock(io.ilock);

        outb_p(io.device_reg, io.device_io);
        outb_p(COM_IDENTIFY, io.command_io);

        if ((inb(io.status_io) & STAT_REG_DRDY) != 0) {

            intr_lock(io.ilock);

            int i;
            for (i = 0; i < 256; i++) {
                _u16_t data = inw(io.data_io);
                if (i == 0) {
                    if (data == 0)
                        break;
                    else
                        state = CHS;
                }
                else if (i == 49 && (data & 0x100) != 0) {
                    state = LBA28;
                }
                else if (i == 60 && state == LBA28) {
                    ata_sectors[n] = data;
                }
                else if (i == 61 && state == LBA28) {
                    ata_sectors[n] += (_u64_t) data << 16;
                }
                else if (i == 83 && (data & 0x200) != 0) {
                    state = LBA48;
                }
                else if (i == 100 && state == LBA48) {
                    ata_sectors[n] = data;
                }
                else if (i == 101 && state == LBA48) {
                    ata_sectors[n] += (_u64_t) data << 16;
                }
                else if (i == 102 && state == LBA48) {
                    ata_sectors[n] += (_u64_t) data << 32;
                }
                else if (i == 103 && state == LBA48) {
                    ata_sectors[n] += (_u64_t) data << 48;
                }
            }

        }

        ata_state |= state << (n * 2);

        ata_io_unlock(n);

        if (ata_sectors[n] != 0) {
            debug("ATA %d: %s Bus, %s Device, %s", n, (n / 2) ? "Secondary":"Primary", (n % 2) ? "Slave":"Master",
            state ? ((state - 1) ? ((state - 2) ? "LBA48 Mode":"LBA28 Mode"):"CHS Mode"):"NoDisk");
            disk_create (DISK_ATA, n, ata_sectors[n]);
        }
    }

}

static void ata_lba28_attr(_ata_io_t io, _u8_t count, _u32_t offect) {

    outb_p(count, io.sector_count_io);

    outb_p(offect & 0xff, io.lba_low_io);
    outb_p((offect >> 8) & 0xff, io.lba_mid_io);
    outb_p((offect >> 16) & 0xff, io.lba_high_io);

    outb_p(((offect >> 24) & 0x0f)|DEV_REG_L|io.device_reg, io.device_io);

}

static void ata_read_lba28(_ata_io_t io, _u8_t count, _u32_t offect) {

    ata_lba28_attr(io, count, offect);

    outb_p(COM_READ_SECTORS, io.command_io);

}

static void ata_write_lba28(_ata_io_t io, _u8_t count, _u32_t offect) {

    ata_lba28_attr(io, count, offect);

    outb_p(COM_WRITE_SECTORS, io.command_io);

}

static void ata_lba48_attr(_ata_io_t io, _u16_t count, _u64_t offect)  {

    outb_p((count >> 8) & 0xff, io.sector_count_io);

    outb_p((_u8_t) ((offect >> 24) & 0xff), io.lba_low_io);
    outb_p((_u8_t) ((offect >> 32) & 0xff), io.lba_mid_io);
    outb_p((_u8_t) ((offect >> 40) & 0xff), io.lba_high_io);

    outb_p(count & 0xff, io.sector_count_io);

    outb_p((_u8_t) (offect & 0xff), io.lba_low_io);
    outb_p((_u8_t) ((offect >> 8) & 0xff), io.lba_mid_io);
    outb_p((_u8_t) ((offect >> 16) & 0xff), io.lba_high_io);

    outb_p(DEV_REG_L|io.device_reg, io.device_io);

}

static void ata_read_lba48(_ata_io_t io, _u16_t count, _u64_t offect) {

    ata_lba48_attr(io, count, offect);

    outb_p(COM_READ_SECTORS_EXT, io.command_io);

}

static void ata_write_lba48(_ata_io_t io, _u16_t count, _u64_t offect) {

    ata_lba48_attr(io, count, offect);

    outb_p(COM_WRITE_SECTORS_EXT, io.command_io);

}

static void ata_read_data(_ata_io_t io, void* buf, _size_t sector_count, _size_t count, _u64_t offect) {

    int i,n;
    for (n = 0; n < sector_count; n++) {
        //while ((inb(io.status_io) & STAT_REG_BSY) != 0) {}
        intr_lock(io.ilock);
        init_intr_lock(io.ilock);
        for (i = 0; i < (ATA_SECTOR_SIZE / 2); i++) {
            _u16_t data = inw(io.data_io);
            if ((n * ATA_SECTOR_SIZE + i * 2 >= offect) && (n * ATA_SECTOR_SIZE + i * 2 - offect < count)) {
                ((_u8_t*) buf)[n * ATA_SECTOR_SIZE + i * 2 - offect] = data & 0xff;
            }
            if ((n * ATA_SECTOR_SIZE + i * 2 + 1 >= offect) && (n * ATA_SECTOR_SIZE + i * 2 - offect + 1 < count)) {
                ((_u8_t*) buf)[n * ATA_SECTOR_SIZE + i * 2 - offect + 1] = (data >> 8) & 0xff;
            }
        }
    }

}

_ssize_t ata_read(_u8_t n, void* buf, _size_t count, _u64_t offect) {

    if (n >= 4) return -1;

    _u64_t start_sector = offect / ATA_SECTOR_SIZE;

    if (start_sector >= ata_sectors[n]) return 0;

    _u64_t end_sector = (offect + count + ATA_SECTOR_SIZE - 1) / ATA_SECTOR_SIZE;

    if (end_sector >= ata_sectors[n]) end_sector = ata_sectors[n] - 1;

    _size_t sector_count = end_sector - start_sector;

    if (sector_count == 0) return 0;

    _u64_t start_offect = offect - (offect & ~(_u64_t) (ATA_SECTOR_SIZE - 1));

    _ata_io_t io = ata_io_lock(n);

    init_intr_lock(io.ilock);

    switch (io.state) {

        case LBA28:
            ata_read_lba28(io, sector_count, start_sector);
            break;

        case LBA48:
            ata_read_lba48(io, sector_count, start_sector);
            break;

        default:
            ata_io_unlock(n);
            return -1;
    }

    if ((inb(io.status_io) & STAT_REG_ERR) != 0) {
        ata_io_unlock(n);
        return -1;
    }

    if (count > sector_count * ATA_SECTOR_SIZE - start_offect) count = sector_count * ATA_SECTOR_SIZE - start_offect;

    ata_read_data(io, buf, sector_count, count, start_offect);

    ata_io_unlock(n);

    return count;
}

static void ata_write_data(_ata_io_t io, void* buf, _size_t sector_count, _size_t count, _u64_t offect) {

    int i,n;
    for (n = 0; n < sector_count; n++) {
        while ((inb(io.status_io) & STAT_REG_BSY) != 0) {}
        //intr_lock(io.ilock);
        //init_intr_lock(io.ilock);
        for (i = 0; i < 0x100; i++) {
            _u16_t data;
            if (n * ATA_SECTOR_SIZE + i * 2 < offect) {
                data = buf_1[i * 2];
            }
            else if (n * ATA_SECTOR_SIZE + i * 2 - offect >= count) {
                data = buf_2[(n * ATA_SECTOR_SIZE + i * 2 - offect) - count];
            }
            else {
                data = ((_u8_t*) buf)[n * ATA_SECTOR_SIZE + i * 2 - offect];
            }
            if ((n * ATA_SECTOR_SIZE + i * 2 + 1 < offect)) {
                data |= (_u16_t) buf_1[i * 2 + 1] << 8;
            }
            else if (n * ATA_SECTOR_SIZE + i * 2 - offect + 1 >= count) {
                data |= (_u16_t) buf_2[(n * ATA_SECTOR_SIZE + i * 2 - offect) - count + 1] << 8;
            }
            else {
                data |= (_u16_t) ((_u8_t*) buf)[n * ATA_SECTOR_SIZE + i * 2 - offect + 1] << 8;
            }
            outw(data,io.data_io);
        }
    }

}

_ssize_t ata_write(_u8_t n, void* buf, _size_t count, _u64_t offect) {

    if (n >= 4) return -1;

    _u64_t start_sector = offect >> 9;

    if (start_sector >= ata_sectors[n]) return 0;

    _u64_t end_sector = (offect + count + ATA_SECTOR_SIZE - 1) >> 9;

    if (end_sector >= ata_sectors[n]) end_sector = ata_sectors[n] - 1;

    _size_t sector_count = end_sector - start_sector;

    _u64_t start_offect = offect - (offect & ~(_u64_t) (ATA_SECTOR_SIZE - 1));

    if (start_offect != 0) {
        if (ata_read(n, &buf_1, start_offect, offect & ~(_u64_t) (ATA_SECTOR_SIZE - 1)) == -1) {
            return -1;
        }
    }

    if ((end_sector + 1) * ATA_SECTOR_SIZE > count + offect) {
        if (ata_read(n, &buf_2, end_sector * ATA_SECTOR_SIZE - (count + offect), count + offect) == -1) {
            return -1;
        }
    }

    _ata_io_t io = ata_io_lock(n);

    //init_intr_lock(io.ilock);

    switch (io.state) {

        case LBA28:
            ata_write_lba28(io, sector_count, start_sector);
            break;

        case LBA48:
            ata_write_lba48(io, sector_count, start_sector);
            break;

        default:
            ata_io_unlock(n);
            return -1;
    }

    if ((inb(io.status_io) & STAT_REG_ERR) != 0) {
        ata_io_unlock(n);
        return -1;
    }

    if (count > sector_count * ATA_SECTOR_SIZE - start_offect) count = sector_count * ATA_SECTOR_SIZE - start_offect;

    ata_write_data(io, buf, sector_count, count, start_offect);

    ata_io_unlock(n);

    return count;
}

_u64_t get_ata_sectors(_u8_t n) {
    if (n > 4) return 0;
    return ata_sectors[n];
}

void init_ata(void) {

    reg_irq_handler(14, irq14_handler);
    reg_irq_handler(15, irq15_handler);

    init_process_lock(&lock_p);
    init_process_lock(&lock_s);

    init_status();

}
