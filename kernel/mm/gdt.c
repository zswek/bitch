// kernel/mm/gdt.c

#include <kernel/def.h>
#include <arch/x86/descriptor.h>
#include <arch/x86/tss.h>

static __attribute__ ((aligned(8))) _segment_descriptor_t gdt_list[GDT_LENGTH];

void gdt_init(void) {
    set_segment_descriptor(&gdt_list[0], 0, 0, 0);
    set_segment_descriptor(&gdt_list[GDT_KERNEL_CODE_INDEX], PAGE_OFFSET, 
    (MAX_ATTR - PAGE_OFFSET - 1) >> 12, 
    SEGM_G|SEGM_DB|DESC_P|DESC_DPL0|DESC_S|SEGM_TYPE_CR);
    set_segment_descriptor(&gdt_list[GDT_KERNEL_DATA_INDEX], PAGE_OFFSET, 
    (MAX_ATTR - PAGE_OFFSET - 1) >> 12, 
    SEGM_G|SEGM_DB|DESC_P|DESC_DPL0|DESC_S|SEGM_TYPE_RW);
    set_segment_descriptor(&gdt_list[GDT_USER_CODE_INDEX], 0, (MAX_ATTR - 1) >> 12, 
    SEGM_G|SEGM_DB|DESC_P|DESC_DPL3|DESC_S|SEGM_TYPE_CR);
    set_segment_descriptor(&gdt_list[GDT_USER_DATA_INDEX], 0, (MAX_ATTR - 1) >> 12, 
    SEGM_G|SEGM_DB|DESC_P|DESC_DPL3|DESC_S|SEGM_TYPE_RW);
    set_segment_descriptor(&gdt_list[GDT_TSS_INDEX], (_u32_t) &globl_tss + PAGE_OFFSET, sizeof(_task_state_segment_t), 
    DESC_P|DESC_DPL3|DESC_D|SEGM_TYPE_TSS);
    
    load_gdt((void*) ((_u32_t) &gdt_list + PAGE_OFFSET), GDT_LENGTH);
}
