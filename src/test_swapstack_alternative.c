#include <stdio.h>
#include "liblushan.h"

struct my_context {
    LSStack main_stack;
    LSStack sub_stack;
};

void coro_hello(void *arg) {
    printf("[coro] Hello! I am the coroutine!\n");

    struct my_context *ctx = (struct my_context*)arg;
    
    ls_swap_out(&ctx->sub_stack, &ctx->main_stack);
}

int main() {
    struct my_context ctx;

    ls_stack_new(&ctx.sub_stack, 4096);
    ls_stack_init_alt(&ctx.sub_stack, coro_hello, &ctx);

    printf("[main] Hello! I am main!\n");

    ls_swap_out(&ctx.main_stack, &ctx.sub_stack);

    printf("[main] Welcome back!\n");

    ls_stack_free(&ctx.sub_stack);
    return 0;
}

// vim: tw=80 ts=4 sw=4 sws=4
