OS = ""

ASMS := $(wildcard *.S)

TEST_SRCS = $(wildcard test_*.c)

TEST_OUTS = $(TEST_SRCS:.c=.out)

DEPS = liblushan.c liblushan.h ls_asm_common.S.inc $(ASMS)

all: tests

.PHONY: tests

tests: $(TEST_OUTS)

%.out: %.c $(DEPS)
	$(CC) $(CFLAGS) -D LS_$(OS) -o $@ $< liblushan.c $(ASMS)
