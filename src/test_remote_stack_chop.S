#include "ls_asm_common.S.inc"

  // Expect this configuration in the beginning:

  // `new_rv`                   <- rsp
  // ret_address

  // Then just return new_rv

begin_func simple_returner
  popq %rax
  ret
end_func simple_returner
