  // Expect this configuration in the beginning:

  // `func`       <- SP, 16-byte aligned
  // `arg`
  // 0 (cusion)   <- 16-byte aligned. It is here to prevent underflow.

  popq %rsi
  popq %rdi
  callq *%rsi
