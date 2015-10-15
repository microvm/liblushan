#if defined (LS_LINUX)

#define CNAME(n) n

  .macro begin_func n
  .text
  .globl CNAME(\n)
  .type CNAME(\n),@function
  .align 16
CNAME(\n):
  .endm

  .macro end_func n
  .size CNAME(\n), .-CNAME(\n)
  .endm

#elif defined (LS_OSX)

#define CNAME(n) _n

  .macro begin_func n
  .text
  .globl CNAME(\n)
  .align 4
CNAME(\n):
  .endm

  .macro end_func n
  .endm

#else
  #error "Please define either LS_LINUX or LS_OSX (for example gcc -DLS_LINUX ...)"
#endif

; vim: ft=asm
