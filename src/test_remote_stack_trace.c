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

        ls_stack_swap_top_to_unw_context(stack->sp, &uc);
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
