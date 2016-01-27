// kernel/def.h

#ifndef _KERNEL_DEF_H_
#define _KERNEL_DEF_H_

//Architecture Define

#define HIGH_ATTR   0x100000        //1M
#define MAX_ATTR    0x100000000     //4G

#define PAGE_PDE_INDEX  0x400       //1024
#define PAGE_PTE_INDEX  0x400       //1024
#define PAGE_SIZE       0x1000      //4K


//Kernel Define

#define KERNEL_STACK_SIZE       0x2000      //8K

#define GDT_LENGTH              6
#define GDT_KERNEL_CODE_INDEX   1
#define GDT_KERNEL_DATA_INDEX   2
#define GDT_USER_CODE_INDEX     3
#define GDT_USER_DATA_INDEX     4
#define GDT_TSS_INDEX           5

#define IDT_LENGTH              0x100       //256

#define PAGE_OFFSET          0xc0000000     //3G
#define GLOBL_PAGE_NUM       0x30000        //192K*4K = 768M
#define DMMAP_PAGE_MAX       0x20000        //128K*4K = 512M

#endif