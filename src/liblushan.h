#include <stdint.h>

typedef void *LSPtr;

typedef struct LSStack {
    LSPtr sp;
} LSStack;

LSStack *ls_handler_stack;

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

// Call this to trigger the trap.
void ls_trap();

// Signature of trap handler.
typedef void (*LSTrapHandler)(LSRegState *state);

// Set this handler and it will be called back.
LSTrapHandler ls_handler;

