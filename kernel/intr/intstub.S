// kernel/intr/intstub.S

#include    <kernel/def.h>
#include    <arch/x86/selector.h>

.macro int n
    .globl  int_\n
int_\n:
    cli
    pushl   $0
    pushl   $\n
    jmp     int_stub
.endm

.macro inte n
    .globl  int_\n
int_\n:
    cli
    pushl   $\n
    jmp     int_stub
.endm
    
    .text
    
int_stub:
    pusha
    movw    %ds, %ax
    pushl   %eax
    
    movw    $selector(GDT_KERNEL_DATA_INDEX, SEL_RPL0), %ax
    movw    %ax, %ds
    movw    %ax, %es
    movw    %ax, %fs
    movw    %ax, %gs
    movw    %ax, %ss
    
    pushl   %esp
    call    int_handler
    addl    $4, %esp
    
    popl    %ebx
    movw    %bx, %ds
    movw    %bx, %es
    movw    %bx, %fs
    movw    %bx, %gs
    //movw    %bx, %ss
    
    popa
    addl    $8, %esp
    iret
    
    
    
    int     0x00
    int     0x01
    int     0x02
    int     0x03
    int     0x04
    int     0x05
    int     0x06
    int     0x07
    inte    0x08
    int     0x09
    inte    0x0a
    inte    0x0b
    inte    0x0c
    inte    0x0d
    inte    0x0e
    int     0x0f
    int     0x10
    inte    0x11
    int     0x12
    int     0x13
    
    
    int     0x14
    int     0x15
    int     0x16
    int     0x17
    int     0x18
    int     0x19
    int     0x1a
    int     0x1b
    int     0x1c
    int     0x1d
    int     0x1e
    int     0x1f
    int     0x20
    
    
    int     0x21
    int     0x22
    int     0x23
    int     0x24
    int     0x25
    int     0x26
    int     0x27
    int     0x28
    int     0x29
    int     0x2a
    int     0x2b
    int     0x2c
    int     0x2d
    int     0x2e
    int     0x2f
    
    int     0x30
    int     0x31
    int     0x32
    int     0x33
    int     0x34
    int     0x35
    int     0x36
    int     0x37
    int     0x38
    int     0x39
    int     0x3a
    int     0x3b
    int     0x3c
    int     0x3d
    int     0x3e
    int     0x3f
    
    int     0x40
    int     0x41
    int     0x42
    int     0x43
    int     0x44
    int     0x45
    int     0x46
    int     0x47
    int     0x48
    int     0x49
    int     0x4a
    int     0x4b
    int     0x4c
    int     0x4d
    int     0x4e
    int     0x4f
    
    int     0x50
    int     0x51
    int     0x52
    int     0x53
    int     0x54
    int     0x55
    int     0x56
    int     0x57
    int     0x58
    int     0x59
    int     0x5a
    int     0x5b
    int     0x5c
    int     0x5d
    int     0x5e
    int     0x5f
    
    int     0x60
    int     0x61
    int     0x62
    int     0x63
    int     0x64
    int     0x65
    int     0x66
    int     0x67
    int     0x68
    int     0x69
    int     0x6a
    int     0x6b
    int     0x6c
    int     0x6d
    int     0x6e
    int     0x6f
    
    int     0x70
    int     0x71
    int     0x72
    int     0x73
    int     0x74
    int     0x75
    int     0x76
    int     0x77
    int     0x78
    int     0x79
    int     0x7a
    int     0x7b
    int     0x7c
    int     0x7d
    int     0x7e
    int     0x7f
    
    int     0x80
    int     0x81
    int     0x82
    int     0x83
    int     0x84
    int     0x85
    int     0x86
    int     0x87
    int     0x88
    int     0x89
    int     0x8a
    int     0x8b
    int     0x8c
    int     0x8d
    int     0x8e
    int     0x8f
    
    int     0x90
    int     0x91
    int     0x92
    int     0x93
    int     0x94
    int     0x95
    int     0x96
    int     0x97
    int     0x98
    int     0x99
    int     0x9a
    int     0x9b
    int     0x9c
    int     0x9d
    int     0x9e
    int     0x9f
    
    int     0xa0
    int     0xa1
    int     0xa2
    int     0xa3
    int     0xa4
    int     0xa5
    int     0xa6
    int     0xa7
    int     0xa8
    int     0xa9
    int     0xaa
    int     0xab
    int     0xac
    int     0xad
    int     0xae
    int     0xaf
    
    int     0xb0
    int     0xb1
    int     0xb2
    int     0xb3
    int     0xb4
    int     0xb5
    int     0xb6
    int     0xb7
    int     0xb8
    int     0xb9
    int     0xba
    int     0xbb
    int     0xbc
    int     0xbd
    int     0xbe
    int     0xbf
    
    int     0xc0
    int     0xc1
    int     0xc2
    int     0xc3
    int     0xc4
    int     0xc5
    int     0xc6
    int     0xc7
    int     0xc8
    int     0xc9
    int     0xca
    int     0xcb
    int     0xcc
    int     0xcd
    int     0xce
    int     0xcf
    
    int     0xd0
    int     0xd1
    int     0xd2
    int     0xd3
    int     0xd4
    int     0xd5
    int     0xd6
    int     0xd7
    int     0xd8
    int     0xd9
    int     0xda
    int     0xdb
    int     0xdc
    int     0xdd
    int     0xde
    int     0xdf
    
    int     0xe0
    int     0xe1
    int     0xe2
    int     0xe3
    int     0xe4
    int     0xe5
    int     0xe6
    int     0xe7
    int     0xe8
    int     0xe9
    int     0xea
    int     0xeb
    int     0xec
    int     0xed
    int     0xee
    int     0xef
    
    int     0xf0
    int     0xf1
    int     0xf2
    int     0xf3
    int     0xf4
    int     0xf5
    int     0xf6
    int     0xf7
    int     0xf8
    int     0xf9
    int     0xfa
    int     0xfb
    int     0xfc
    int     0xfd
    int     0xfe
    int     0xff