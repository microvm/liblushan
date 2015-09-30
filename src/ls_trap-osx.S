.text
.globl _ls_trap
.align 16
_ls_trap:
  // RSP is 8-byte aligned but not 16-byte aligned.
  subq $0x108, %rsp    // sizeof(LSRegState) = 0x108

  movq %rax, 0x0(%rsp)
  movq %rbx, 0x8(%rsp)
  movq %rcx, 0x10(%rsp)
  movq %rdx, 0x18(%rsp)
  movq %rsi, 0x20(%rsp)
  movq %rdi, 0x28(%rsp)
  movq %rbp, 0x30(%rsp)

  movq %r8,  0x40(%rsp)
  movq %r9,  0x48(%rsp)
  movq %r10, 0x50(%rsp)
  movq %r11, 0x58(%rsp)
  movq %r12, 0x60(%rsp)
  movq %r13, 0x68(%rsp)
  movq %r14, 0x70(%rsp)
  movq %r15, 0x78(%rsp)

  movaps %xmm0, 0x80(%rsp)
  movaps %xmm1, 0x90(%rsp)
  movaps %xmm2, 0xa0(%rsp)
  movaps %xmm3, 0xb0(%rsp)
  movaps %xmm4, 0xc0(%rsp)
  movaps %xmm5, 0xd0(%rsp)
  movaps %xmm6, 0xe0(%rsp)
  movaps %xmm7, 0xf0(%rsp)

  // Let the rip field be the return address.
  movq 0x108(%rsp), %rax
  movq %rax, 0x100(%rsp)

  // Let the rsp field be the rsp after returning.
  leaq 0x110(%rsp), %rax
  movq %rax, 0x38(%rsp)

  // first arg: state
  movq %rsp, %rdi

  movq _ls_handler(%rip), %rax
  callq *%rax

  movq 0x0(%rsp),  %rax
  movq 0x8(%rsp),  %rbx
  movq 0x10(%rsp), %rcx
  movq 0x18(%rsp), %rdx
  movq 0x20(%rsp), %rsi
  movq 0x28(%rsp), %rdi
  movq 0x30(%rsp), %rbp
                        
  movq 0x40(%rsp), %r8
  movq 0x48(%rsp), %r9
  movq 0x50(%rsp), %r10
  movq 0x58(%rsp), %r11
  movq 0x60(%rsp), %r12
  movq 0x68(%rsp), %r13
  movq 0x70(%rsp), %r14
  movq 0x78(%rsp), %r15

  movaps 0x80(%rsp), %xmm0
  movaps 0x90(%rsp), %xmm1
  movaps 0xa0(%rsp), %xmm2
  movaps 0xb0(%rsp), %xmm3
  movaps 0xc0(%rsp), %xmm4
  movaps 0xd0(%rsp), %xmm5
  movaps 0xe0(%rsp), %xmm6
  movaps 0xf0(%rsp), %xmm7

  // Do not change rip and rsp for now.

  addq $0x108, %rsp
  ret
