#include "ls_asm_common.S.inc"

  // Expect this configuration in the beginning:

  // `func`       <- SP, 16-byte aligned
  // `arg`
  // 0 (cusion)   <- 16-byte aligned. It is here to prevent underflow.

begin_func ls_sbf_start
  popq %rsi
  popq %rdi
  pushq $0  // explicitly use 0 as the "return address" of func, the "jumpee"
  jmp *%rsi // jump to func.
end_func ls_sbf_start
