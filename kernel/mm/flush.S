// kernel/mm/flush.S

#include    <kernel/def.h>
#include    <arch/x86/selector.h>

    .text
    .globl  mm_flush
    
mm_flush:
    
    movl    %cr0, %eax
    orl      $0x80000000, %eax
    movl    %eax, %cr0
    
    movw    $selector(GDT_KERNEL_DATA_INDEX, SEL_RPL0), %ax
    movw    %ax, %ds
    movw    %ax, %es
    movw    %ax, %fs
    movw    %ax, %gs
    movw    %ax, %ss
    
    ljmp    $selector(GDT_KERNEL_CODE_INDEX, SEL_RPL0), $flush
    
flush:
    
    ret