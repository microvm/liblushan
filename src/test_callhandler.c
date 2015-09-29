#include <stdio.h>
#include <stdlib.h>

#include "liblushan.h"

void my_handler(LSRegState *state) {
    printf("Hello world!\n");
    return;
}

void set_handler() {
    ls_handler = my_handler;
}

int main() {
    set_handler();
    ls_handler(NULL);
}
