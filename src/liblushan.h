#ifndef __LIBLUSHAN_H__
#define __LIBLUSHAN_H__

#include <stdint.h>

//// Basic types

// A general (void*) pointer type
typedef void *LSPtr;

//// Basic utilities for stacks and swap-stack

// A descriptor of a stack. It does not contain the stack itself.
// It can also be used to temporarily hold a stack pointer for stacks not
// created by this library.
typedef struct LSStack {
    LSPtr sp;       // The current SP
    LSPtr begin;    // The begin of the allocated memory
    int64_t size;   // The size (in bytes)
} LSStack;

// Allocate a new stack. After this, `*stack` will describe the stack.
//
// `size` must be 16-byte aligned.
void ls_stack_new(LSStack *stack, int64_t size);

// Deallocate a stack. `stack` must be allocated before.
void ls_stack_free(LSStack *stack);

// The signature of the stack-bottom function. (Sorry. I am not as powerful as
// the Mu micro VM, so I cannot use arbitrary functions.)
typedef void (*LSStackBottomFunc)(LSPtr arg);

// Initialise the stack-bottom function so that when swapping to it, it
// continues from the beginning of that function, receiving the `arg` as its
// argument. This will destroy that stack.
void ls_stack_init(LSStack *stack, LSStackBottomFunc func, LSPtr arg);

// Swap to `new_stack`, and save the current stack context in `cur_stack`.
// 
// This function only uses the `sp` field. If the current stack is not created
// by this library, this function will only use `&cur_stack->sp` as a storage
// space for the SP so that we can swap back to the old stack again.
void ls_stack_swap(LSStack *cur_stack, LSStack *new_stack);

// Basically reinventing mcontext_t, which is supposed to be private to libc.
// This structure should match the stack-top structure.
typedef struct LSRegState {
    uint64_t gpr[16];
    uint32_t xmm[8][4];
    uint64_t rip;
} LSRegState;

enum {
    LS_REG_RAX = 0,
    LS_REG_RBX,
    LS_REG_RCX,
    LS_REG_RDX,
    LS_REG_RSI,
    LS_REG_RDI,
    LS_REG_RBP,
    LS_REG_RSP,
    LS_REG_R8,
    LS_REG_R9,
    LS_REG_R10,
    LS_REG_R11,
    LS_REG_R12,
    LS_REG_R13,
    LS_REG_R14,
    LS_REG_R15,
};

// The stack of the trap handler (currently unused)
LSStack *ls_handler_stack;

// Call this to trigger the trap.
void ls_trap();

// Signature of trap handler.
typedef void (*LSTrapHandler)(LSRegState *state);

// Set this handler and it will be called back.
LSTrapHandler ls_handler;

// The following functions should not be called directly from C.

// The "stack-top routine". It resumes a suspended stack.
void ls_swap_in();

// This function is a trampoline to start the stack-bottom function.
void ls_sbf_start();

#endif // __LIBLUSHAN_H__

// vim: tw=80 ts=4 sw=4 sws=4
