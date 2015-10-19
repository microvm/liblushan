CFLAGS += -std=gnu11
ifndef OS
    uname := $(shell uname)
    ifeq ($(uname),Darwin)
	OS = OSX
    else
    ifeq ($(uname),Linux)
	OS = LINUX
	LDFLAGS += -lunwind -lunwind-x86_64
    else
	$(error Unrecognized operating system $(uname))
    endif
    endif
endif

ASMS = $(filter-out $(wildcard test_*.S), $(wildcard *.S))

TEST_SRCS = $(wildcard test_*.c)

TEST_OUTS = $(TEST_SRCS:.c=.out)

DEPS_C = liblushan.c libunwind-support.c $(ASMS)
DEPS_H = liblushan.h libunwind-support.h ls_asm_common.S.inc
DEPS = $(DEPS_C) $(DEPS_H)

.SECONDEXPANSION:

.PHONY: all
all: tests

.PHONY: tests
tests: $(TEST_OUTS)

$(TEST_OUTS): %.out: %.c $$(wildcard $$*.S) $(DEPS)
	$(CC) $(CFLAGS) -D LS_$(OS) -o $@ $< $(wildcard $*.S) $(DEPS_C) $(LDFLAGS)

.PHONY: clean

clean:
	rm *.out
