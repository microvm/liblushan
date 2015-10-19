#include "libunwind-support.h"

void ls_stack_swap_top_to_unw_context(void *sp, unw_context_t *ctx) {
    uint64_t *s = (uint64_t*)sp;

#if defined(LS_OSX)
    ctx->data[15] = s[2];     // r15
    ctx->data[14] = s[3];     // r14
    ctx->data[13] = s[4];     // r13
    ctx->data[12] = s[5];     // r12
    ctx->data[ 1] = s[6];     // rbx
    ctx->data[ 6] = s[7];     // rbp
    ctx->data[16] = s[8];     // rip (return address)
    ctx->data[ 7] = (uint64_t)&s[9];    // rsp
#elif defined(LS_LINUX)
    uint64_t *c = (uint64_t*)ctx;

    // see ucontext_i.h in libunwind
    c[0x60] = s[2];     // r15
    c[0x58] = s[3];     // r14
    c[0x50] = s[4];     // r13
    c[0x48] = s[5];     // r12
    c[0x80] = s[6];     // rbx
    c[0x78] = s[7];     // rbp
    c[0xa8] = s[8];     // rip (return address)
    c[0xa0] = (uint64_t)&s[9];    // rsp
#endif
}

void ls_unw_cursor_to_simple_frame_state(unw_cursor_t *cursor,
        LSSimpleFrameState *sfs) {
    unw_get_reg(cursor, UNW_X86_64_RBP, &sfs->rbp);
    unw_get_reg(cursor, UNW_X86_64_RBX, &sfs->rbx);
    unw_get_reg(cursor, UNW_X86_64_R12, &sfs->r12);
    unw_get_reg(cursor, UNW_X86_64_R13, &sfs->r13);
    unw_get_reg(cursor, UNW_X86_64_R14, &sfs->r14);
    unw_get_reg(cursor, UNW_X86_64_R15, &sfs->r15);
    unw_get_reg(cursor, UNW_REG_IP,     &sfs->rip);
    unw_get_reg(cursor, UNW_X86_64_RSP, &sfs->rsp);
}


