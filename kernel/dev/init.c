// kernel/dev/init.c

#include <kernel/dev/console.h>
#include <kernel/dev/keyboard.h>
#include <kernel/dev/disk.h>

void dev_init(void) {
    
    //console_init();
    
    keyboard_init();
    
    set_input_mode(2);
    
    disk_init();
    
}