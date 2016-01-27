// kernel/process/switch.S

#include    <kernel/def.h>
#include    <arch/x86/selector.h>

    .text
    .globl  switch_context, switch_to_user_mode
    
switch_context:
    movl    4(%esp), %eax
    
    movl    %esp, 0(%eax)
    movl    %ebp, 4(%eax)
    
    pushf
    popl    %ecx
    movl    %ecx, 8(%eax)
    
    
    movl    8(%esp), %eax
    
    movl    0(%eax), %esp
    movl    4(%eax), %ebp
    movl    8(%eax), %eax
    
    push    %eax
    popf
    
    ret
    
    
    
switch_to_user_mode:
    cli
    
    movw    $selector(GDT_USER_DATA_INDEX, SEL_RPL3), %ax
    movw    %ax, %ds
    movw    %ax, %es
    movw    %ax, %fs
    movw    %ax, %gs
    
    movl    8(%esp), %eax
    pushl   $selector(GDT_USER_DATA_INDEX, SEL_RPL3)
    pushl   %eax
    pushf
    
    popl    %eax
    orl     $0x200, %eax
    pushl   %eax
    
    pushl   $selector(GDT_USER_CODE_INDEX, SEL_RPL3)
    pushl   20(%esp)
    
    iret