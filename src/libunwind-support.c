#ifdef LS_LINUX
#define _GNU_SOURCE
#include <sys/ucontext.h>
#endif

#include "libunwind-support.h"
#include "ls_debug.h"

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
    ucontext_t *uctx = (ucontext_t*)ctx;
    mcontext_t *mctx = &uctx->uc_mcontext;

    // see ucontext_i.h in libunwind
    mctx->gregs[REG_R15] = s[2];     // r15
    mctx->gregs[REG_R14] = s[3];     // r14
    mctx->gregs[REG_R13] = s[4];     // r13
    mctx->gregs[REG_R12] = s[5];     // r12
    mctx->gregs[REG_RBX] = s[6];     // rbx
    mctx->gregs[REG_RBP] = s[7];     // rbp
    mctx->gregs[REG_RIP] = s[8];     // rip (return address)
    mctx->gregs[REG_RSP] = (uint64_t)&s[9];    // rsp
#else
#error "Expect LS_OSX or LS_LINUX"
#endif

    lprintf("r15 = 0x%llx\n", s[2]);
    lprintf("r14 = 0x%llx\n", s[3]);
    lprintf("r13 = 0x%llx\n", s[4]);
    lprintf("r12 = 0x%llx\n", s[5]);
    lprintf("rbx = 0x%llx\n", s[6]);
    lprintf("rbp = 0x%llx\n", s[7]);
    lprintf("rip = 0x%llx\n", s[8]);
    lprintf("rsp = 0x%llx\n", (uint64_t)&s[9]);

    lprintf("I think the rsp should be %p after returning.\n", s+9);
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


