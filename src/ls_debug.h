#include <stdio.h>

#define lprintf(fmt, ...) printf("[%s] " fmt, __func__, ## __VA_ARGS__)
