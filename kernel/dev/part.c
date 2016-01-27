// kernel/dev/part.c

#include <bits/types.h>
#include <kernel/dev/part/gpt.h>
#include <kernel/dev/part/mbr.h>

_size_t disk_part(_size_t id) {
    
    return mbr_part(id);
    
}