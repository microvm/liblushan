  // Let's reinvent boost::context!

  // %rdi: cur_stack, %rsi: new_stack

  // Push callee-saved registers
  pushq %r15
  pushq %r14
  pushq %r13
  pushq %r12
  pushq %rbp
  pushq %rbx

  // Swap-stack
  movq %rsp, (%rdi)
  movq (%rsi), %rsp
  
  // Recover callee-saved registers
  popq %rbx
  popq %rbp
  popq %r12
  popq %r13
  popq %r14
  popq %r15

  // Return to the caller in the new_stack
  ret
