#ifndef __LIBUNWIND_SUPPORT_H__
#define __LIBUNWIND_SUPPORT_H__

#include <libunwind.h>

#ifdef LS_LINUX
#include <ucontext.h>
#endif

// Translate the stack-top status to unw_context_t in a platform-specific way.
// This manually unwinds the top frame of swap-stack, which libuwind may not be
// able to unwind.
void ls_stack_swap_top_to_unw_context(void *sp, unw_context_t *ctx);

// A simple struct of callee-saved registers.
typedef struct {
    uint64_t rbp, rbx, r12, r13, r14, r15, rip, rsp;
} LSSimpleFrameState;

void ls_unw_cursor_to_simple_frame_state(unw_cursor_t *cursor,
        LSSimpleFrameState *sfs);

#endif // __LIBUNWIND_SUPPORT_H__
