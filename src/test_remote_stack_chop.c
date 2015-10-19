#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libunwind.h>

#ifdef LS_LINUX
#include <ucontext.h>
#endif

#include "liblushan.h"

#define lprintf(fmt, ...) printf("[%s] " fmt, __func__, ## __VA_ARGS__)

LSStack mainStack, newStack;

void translate_context(void *sp, unw_context_t *ctx) {
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

    lprintf("I think the rsp should be %p after returning.\n", s+9);
}

void spy3() {
    lprintf("My return address is %p\n", __builtin_return_address(0));
}

void* g_ret_addr;

struct frame_descriptor {
    uint64_t rbp, rbx, r12, r13, r14, r15, rip, rsp;
};

void get_f_frame(LSStack *stack, struct frame_descriptor *desc) {
    unw_cursor_t cursor;
    unw_context_t uc;
    unw_word_t ip, sp;
    int frameNo = 0;

    // The other stack.
    lprintf("Making my own context...\n");
    memset(&uc, 0, sizeof(uc));

    translate_context(stack->sp, &uc);

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

            unw_get_reg(&cursor, UNW_X86_64_RBP, &desc->rbp);
            unw_get_reg(&cursor, UNW_X86_64_RBX, &desc->rbx);
            unw_get_reg(&cursor, UNW_X86_64_R12, &desc->r12);
            unw_get_reg(&cursor, UNW_X86_64_R13, &desc->r13);
            unw_get_reg(&cursor, UNW_X86_64_R14, &desc->r14);
            unw_get_reg(&cursor, UNW_X86_64_R15, &desc->r15);
            unw_get_reg(&cursor, UNW_REG_IP, &desc->rip);
            unw_get_reg(&cursor, UNW_X86_64_RSP, &desc->rsp);
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

void cut_above_f(struct frame_descriptor *desc, int new_rv) {
    lprintf("f() state:\n");
    lprintf("rbx=%p, rbp=%p, r12=%p, r13=%p, r14=%p, r15=%p, rip=%p, rsp=%p\n",
            (void*)desc->rbx, (void*)desc->rbp, (void*)desc->r12, (void*)desc->r13,
            (void*)desc->r14, (void*)desc->r15, (void*)desc->rip, (void*)desc->rsp);

    // Current state of f()'s frame:
    //
    // +------------+
    // |(g frame)   |
    // |desc->rip   |
    // +------------+
    // |(f frame)   |    <--- desc->rsp
    // |...         |
    // +------------+
    //
    // We'll make it like this:
    //
    // +------------------------+
    // |ls_swap_in              |    <--- mainStack->sp
    // |0                       |
    // |desc->r15               |
    // |desc->r14               |
    // |desc->r13               |
    // |desc->r12               |
    // |desc->rbx               |
    // |desc->rbp               |
    // |simple_returner         |
    // +------------------------+
    // |new_rv                  |
    // |desc->rip               |
    // +------------------------+
    // |(f frame)               |    <--- desc->rsp
    // |...                     |
    // +------------------------+
    
    uint64_t *f_rsp = (uint64_t*)desc->rsp;
    f_rsp[- 1] = desc->rip;
    f_rsp[- 2] = (uint64_t)new_rv;
    f_rsp[- 3] = (uint64_t)simple_returner;
    f_rsp[- 4] = desc->rbp;
    f_rsp[- 5] = desc->rbx;
    f_rsp[- 6] = desc->r12;
    f_rsp[- 7] = desc->r13;
    f_rsp[- 8] = desc->r14;
    f_rsp[- 9] = desc->r15;
    f_rsp[-10] = 0LL;
    f_rsp[-11] = (uint64_t)ls_swap_in;

    mainStack.sp = &f_rsp[-11];
}

void spy(void *arg) {
    lprintf("Hi! I will make g() return early.\n");
    lprintf("My return address is %p\n", __builtin_return_address(0));

    struct frame_descriptor desc;

    lprintf("Looking for g()'s parent frame...\n");
    get_f_frame(&mainStack, &desc);

    int new_rv = 12345678;
    lprintf("Start replacement. I'll let it return %d\n", new_rv);
    cut_above_f(&desc, new_rv);

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
