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

void print_stack_trace(LSStack *stack) {
    unw_cursor_t cursor;
    unw_context_t uc;
    unw_word_t ip, sp;
    int frameNo = 0;

    spy3();
    
    if (stack == NULL) {
        // The current stack.
        lprintf("Getting context...\n");
        unw_getcontext(&uc);
    } else {
        // The other stack.
        lprintf("Making my own context...\n");
        memset(&uc, 0, sizeof(uc));

        translate_context(stack->sp, &uc);
    }

    spy3();

    lprintf("Initialising cursor...\n");
    unw_init_local(&cursor, &uc);

    lprintf("Stepping in...\n");
    do {
        lprintf("Frame %d:\n", frameNo);
        unw_get_reg(&cursor, UNW_REG_IP, &ip);
        unw_get_reg(&cursor, UNW_REG_SP, &sp);
        lprintf ("  ip = %lx, sp = %lx\n", (long) ip, (long) sp);

        frameNo++;
    } while (unw_step(&cursor) > 0) ;
}

void spy2(void *arg) {
    lprintf("Hi! I will print the stack trace.\n");
    lprintf("My return address is %p\n", __builtin_return_address(0));

    lprintf("Printing stack trace of the spy...\n");
    print_stack_trace(NULL);

    lprintf("Printing stack trace of the main stack...\n");
    print_stack_trace(&mainStack);

    lprintf("Bye!\n");
}

void spy(void *arg) {
    lprintf("My return address is %p\n", __builtin_return_address(0));
    lprintf("arg = %p\n", arg);
    spy2(arg);
    lprintf("Bye!\n");
    ls_stack_swap(&newStack,&mainStack);
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
    int v = h(n) + 1;
    lprintf("After calling. v = %d, but you should not see me.\n", v);
    return v;
}
int f(int n) {
    lprintf("My return address is %p\n", __builtin_return_address(0));
    lprintf("Before calling. n = %d\n", n);
    int w = g(n) * 2;
    lprintf("After calling. w = %d\n", w);
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
