// kernel/binfmt/a.out.c

#include <bits/types.h>

typedef struct exec_header_t {
    _u32_t a_midmag;
    _u32_t a_text;
    _u32_t a_data;
    _u32_t a_bss;
    _u32_t a_syms;
    _u32_t a_entry;
    _u32_t a_trsize;
    _u32_t a_drsize;
} __attribute__((packed)) _exec_header_t;

void a_out_exec(const char* path) {

}
