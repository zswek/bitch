// kernel/init.S

#include    <kernel/def.h>

    .bss
    
    .globl  init_kernel_stack
    .comm   init_kernel_stack, KERNEL_STACK_SIZE
    
    .text
    .globl  _kernel_init
    
_kernel_init:
    movl    $(init_kernel_stack + KERNEL_STACK_SIZE), %esp
    
    pushl   $0
    popf
    
    call    kernel_init
    
loop:
    hlt
    jmp     loop