#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libunwind.h>

#include "liblushan.h"
#include "libunwind-support.h"
#include "ls_debug.h"

LSStack mainStack, newStack;

void spy3() {
    lprintf("My return address is %p\n", __builtin_return_address(0));
}

void* g_ret_addr;

void get_f_frame(LSStack *stack, LSSimpleFrameState *sfs) {
    unw_cursor_t cursor;
    unw_context_t uc;
    unw_word_t ip, sp;
    int frameNo = 0;

    // The other stack.
    lprintf("Making my own context...\n");
    memset(&uc, 0, sizeof(uc));

    lprintf("Translating stack top...\n");
    ls_stack_swap_top_to_unw_context(stack->sp, &uc);

    lprintf("Initialising cursor...\n");
    unw_init_local(&cursor, &uc);

    lprintf("Stepping in...\n");
    do {
        lprintf("Frame %d:\n", frameNo);
        unw_get_reg(&cursor, UNW_REG_IP, &ip);
        unw_get_reg(&cursor, UNW_REG_SP, &sp);
        lprintf ("  ip = %lx, sp = %lx\n", (long) ip, (long) sp);

        if (ip == (uint64_t) g_ret_addr) {
            lprintf("Found f()!\n");
            lprintf("Now I am in f(). Restoring callee-saved registers...\n");

            ls_unw_cursor_to_simple_frame_state(&cursor, sfs);
            return;
        }

        frameNo++;
    } while (unw_step(&cursor) > 0);

    lprintf("ERROR: g() frame not found!\n");
    exit(1);

    return;
}

// This function simply pops a value and return it. Implemented in asm.
void simple_returner();

void cut_above_f(LSSimpleFrameState *sfs, int new_rv) {
    lprintf("f() state:\n");
    lprintf("rbx=%p, rbp=%p, r12=%p, r13=%p, r14=%p, r15=%p, rip=%p, rsp=%p\n",
            (void*)sfs->rbx, (void*)sfs->rbp, (void*)sfs->r12, (void*)sfs->r13,
            (void*)sfs->r14, (void*)sfs->r15, (void*)sfs->rip, (void*)sfs->rsp);

    // Current state of f()'s frame:
    //
    // +------------+
    // |(g frame)   |
    // |sfs->rip    |
    // +------------+
    // |(f frame)   |    <--- sfs->rsp
    // |...         |
    // +------------+
    //
    // We'll make it like this:
    //
    // +------------------------+
    // |ls_swap_in              |    <--- mainStack->sp
    // |0                       |
    // |sfs->r15                |
    // |sfs->r14                |
    // |sfs->r13                |
    // |sfs->r12                |
    // |sfs->rbx                |
    // |sfs->rbp                |
    // |simple_returner         |
    // +------------------------+
    // |new_rv                  |
    // |sfs->rip                |
    // +------------------------+
    // |(f frame)               |    <--- sfs->rsp
    // |...                     |
    // +------------------------+
    
    uint64_t *f_rsp = (uint64_t*)sfs->rsp;
    f_rsp[- 1] = sfs->rip;
    f_rsp[- 2] = (uint64_t)new_rv;
    f_rsp[- 3] = (uint64_t)simple_returner;
    f_rsp[- 4] = sfs->rbp;
    f_rsp[- 5] = sfs->rbx;
    f_rsp[- 6] = sfs->r12;
    f_rsp[- 7] = sfs->r13;
    f_rsp[- 8] = sfs->r14;
    f_rsp[- 9] = sfs->r15;
    f_rsp[-10] = 0LL;
    f_rsp[-11] = (uint64_t)ls_swap_in;

    mainStack.sp = &f_rsp[-11];
}

void spy(void *arg) {
    lprintf("Hi! I will make g() return early.\n");
    lprintf("My return address is %p\n", __builtin_return_address(0));

    LSSimpleFrameState sfs;

    lprintf("Looking for g()'s parent frame...\n");
    get_f_frame(&mainStack, &sfs);

    int new_rv = 12345678;
    lprintf("Start replacement. I'll let it return %d\n", new_rv);
    cut_above_f(&sfs, new_rv);

    lprintf("Bye!\n");
    ls_stack_swap(&newStack, &mainStack);
}

int h(int n) {
    lprintf("My return address is %p\n", __builtin_return_address(0));
    lprintf("My caller's return address is %p\n", __builtin_return_address(1));

    ls_stack_swap(&mainStack, &newStack);

    lprintf("Bye!\n");
    return n;
}

int g(int n) {
    lprintf("My return address is %p\n", __builtin_return_address(0));
    lprintf("Before calling. n = %d\n", n);

    g_ret_addr = __builtin_return_address(0);

    int v = h(n) + 1;
    lprintf("After calling. v = %d, but you should not see me.\n", v);
    return v;
}

int f(int n) {
    lprintf("My return address is %p\n", __builtin_return_address(0));
    lprintf("Before calling. n = %d\n", n);

    int grv = g(n);

    lprintf("g() returned %d\n", grv);

    int w = grv * 2;
    lprintf("w = %d\n", w);
    return w;
}

int main() {
    lprintf("Making stacks...\n");
    ls_stack_new(&newStack, 16384);
    ls_stack_init(&newStack, spy, NULL);

    lprintf("I am about to call f...\n");
    int result = f(42);
    lprintf("result = %d\n", result);
}
