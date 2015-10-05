#include <assert.h>
#include <stdlib.h>

#include "liblushan.h"

void ls_stack_new(LSStack *stack, int64_t size) {
    assert(size % 16 == 0);

    LSPtr begin = (LSPtr)malloc(size);
    stack->begin = begin;
    stack->size = size;
}

void ls_stack_free(LSStack *stack) {
    free(stack->begin);
}
/*
 * The stack layout after initialisation
 *
 * (low addr)
 * ------------------
 *  0 (rbx)                 <- SP. ls_stack_swap expects this.
 *  0 (rbp)
 *  0 (r12)
 *  0 (r13)
 *  0 (r14)
 *  0 (r15)
 *  &ls_sbf_start
 *  `func`                  <- 16-byte aligned. SP is here in *  ls_sbf_start
 *  `arg`                   <-
 *  0 (cusion)              <- 16-byte aligned
 *  0 (cusion)              <- 
 * ------------------       <- 16-byte aligned (begin + size) 
 *
 * When `func` starts, it will think it is called by a function at 0x000000000.
 */
void ls_stack_init(LSStack *stack, LSStackBottomFunc func, LSPtr arg) {
    uintptr_t *sp = (uintptr_t*)((unsigned char*)stack->begin+stack->size);

    *(--sp) = 0;   // cusion
    *(--sp) = 0;   // cusion
    *(--sp) = (uintptr_t)arg;
    *(--sp) = (uintptr_t)func;
    *(--sp) = (uintptr_t)ls_sbf_start;
    *(--sp) = 0;
    *(--sp) = 0;
    *(--sp) = 0;
    *(--sp) = 0;
    *(--sp) = 0;
    *(--sp) = 0;

    stack->sp = sp;
}

// vim: tw=80 ts=4 sw=4 sws=4
