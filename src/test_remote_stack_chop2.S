#include "ls_asm_common.S.inc"

  // Expect this configuration in the beginning:

  // `new_rv`                   <- rsp
  // ret_address

  // Then just return new_rv

begin_func simple_returner
  popq %rax
  ret
end_func simple_returner

// implementing int f(int n) in asm.
begin_func f
    .cfi_startproc
    pushq %rbp
    .cfi_def_cfa_offset 16
    .cfi_offset %rbp, -16
    movq %rsp, %rbp
    .cfi_def_cfa_register %rbp
    pushq %rbx
    pushq %r12
    pushq %r13
    pushq %r14
    pushq %r15
    xorq %rax, %rax
    pushq %rax  // stack alignment
    .cfi_offset %r15, -56
    .cfi_offset %r14, -48
    .cfi_offset %r13, -40
    .cfi_offset %r12, -32
    .cfi_offset %rbx, -24

    movq $0x1111111111111111, %rbx
    movq $0x2222222222222222, %r12
    movq $0x3333333333333333, %r13
    movq $0x4444444444444444, %r14
    movq $0x5555555555555555, %r15

    movq %rbx, %rdi
    movq %rbp, %rsi
    movq %r12, %rdx
    movq %r13, %rcx
    movq %r14, %r8
    movq %r15, %r9

    callq CNAME(print_stack_state)

    callq CNAME(g)

    movq %rbx, %rdi
    movq %rbp, %rsi
    movq %r12, %rdx
    movq %r13, %rcx
    movq %r14, %r8
    movq %r15, %r9

    movq %rax, %r15     // return value from g()

    callq CNAME(print_stack_state)

    movq %r15, %rax

    popq %r15   // padding
    popq %r15
    popq %r14
    popq %r13
    popq %r12
    popq %rbx
    popq %rbp

    ret

    .cfi_endproc
end_func f
