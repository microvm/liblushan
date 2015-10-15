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
 *  0 (cushion)             <- 16-byte aligned
 *  0 (cushion)             <- 
 * ------------------       <- 16-byte aligned (begin + size) 
 *
 * When `func` starts, it will think it is called by a function at 0x000000000.
 */
void ls_stack_init(LSStack *stack, LSStackBottomFunc func, LSPtr arg) {
    uintptr_t *sp = (uintptr_t*)((unsigned char*)stack->begin+stack->size);

    *(--sp) = 0;   // cushion
    *(--sp) = 0;   // cushion
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

/*
 * The stack layout after initialisation
 *
 * (low addr)
 * ------------------
 *  &ls_swap_in             <- SP. This always points to the "appropriate" func,
 *  0 (alignment)                  thus allows multiple stack-top impls.
 *  0 (r15)
 *  0 (r14)                 <- 16-byte aligned
 *  0 (r13)
 *  0 (r12)                 <- 16-byte aligned
 *  0 (rbx)                 
 *  0 (rbp)                 <- 16-byte aligned
 *  &ls_sbf_start
 *  `func`                  <- 16-byte aligned. SP is here in *  ls_sbf_start
 *  `arg`                   <-
 *  0 (cushion)             <- 16-byte aligned
 *  0 (cushion)             <- 
 * ------------------       <- 16-byte aligned (begin + size) 
 *
 * When `func` starts, it will think it is called by a function at 0x000000000.
 */
void ls_stack_init_alt(LSStack *stack, LSStackBottomFunc func, LSPtr arg) {
    uintptr_t *sp = (uintptr_t*)((unsigned char*)stack->begin+stack->size);

    *(--sp) = 0;   // cushion
    *(--sp) = 0;   // cushion
    *(--sp) = (uintptr_t)arg;
    *(--sp) = (uintptr_t)func;
    *(--sp) = (uintptr_t)ls_sbf_start;
    *(--sp) = 0;
    *(--sp) = 0;
    *(--sp) = 0;
    *(--sp) = 0;
    *(--sp) = 0;
    *(--sp) = 0;
    *(--sp) = 0;
    *(--sp) = (uintptr_t)ls_swap_in;

    stack->sp = sp;
}

// vim: tw=80 ts=4 sw=4 sws=4
