#include "ls_asm_common.S.inc"

  // Expect this configuration in the beginning:

  // 0 (padding)            <- rsp
  // func
  // arg
  // ret_address

  // Then tail call func with arg as its argument.

begin_func gprime_starter
  popq %rax
  popq %rax
  popq %rdi
  jmp *%rax
end_func gprime_starter
