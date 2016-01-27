// boot/multiboot/head.S

#define _MULTIBOOT_MAGIC  0x1BADB002
#define _MULTIBOOT_FLAGS  0x00000003

    .text
    .globl  _start
    
_multiboot_header:
    .long   _MULTIBOOT_MAGIC
    .long   _MULTIBOOT_FLAGS
    .long   -(_MULTIBOOT_MAGIC + _MULTIBOOT_FLAGS)
    
_start:
    cli
    
    movl    $0x7c00, %esp
    
    pushl   %ebx
    pushl   %eax
    
    call     _multiboot_init
    
loop:
    hlt
    jmp     loop