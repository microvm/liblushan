#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>

#include "liblushan.h"

const char *reg_names[] = { "RAX", "RBX", "RCX", "RDX", "RSI", "RDI", "RSP", "RBP",
    "R8", "R9", "R10", "R11", "R12", "R13", "R14", "R15" };

void my_handler(LSRegState *state) {
    int i;

    printf("Hi! I am the trap handler!\n");

    for (i=0; i<16; i++) {
        printf("%s = 0x%" PRIx64 "\n", reg_names[i], state->gpr[i]);
    }

    for (i=0; i<8; i++) {
        printf("xmm%d = 0x%x 0x%x 0x%x 0x%x\n", i,
                state->xmm[i][0], state->xmm[i][1], 
                state->xmm[i][2], state->xmm[i][3]);
    }

    printf("return address = 0x%" PRIx64 "\n", state->rip);

    return;
}

void set_handler() {
    ls_handler = my_handler;
}

int fac(volatile int n) {
    ls_trap();
    int i;
    volatile int p=1;
    for (i=1; i<=n; i++) {
        p *= i;
    }
    return p;
}

void (*abusing_trap)(int a, int b, int c, int d, int e, int f, int g, double h, double i, double j, double k) = ls_trap;

int main() {
    set_handler();
    int result = fac(5);
    printf("fac(5) = %d\n", result);

    abusing_trap(1,2,3,4,5,6,7,8.0,9.0,10.0,11.0);
}
