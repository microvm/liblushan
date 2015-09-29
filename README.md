# liblushan: Simple Stack Introspection Library

> I don't know how the Lushan mountain looks like, because I am personally in
> that mountain.

This library allows a program to trigger a "trap" by simply calling the
`ls_trap` function. Then it will enter your registered "trap handler" which can
introspect the exact state of all registers just before calling `ls_trap`.

The main audience is compiler writers and VM writers. It is supposed to be a
debugging and testing tool.

How to use: Just put a `callq ls_trap` anywhere in the program and let the trap
handler observe the state.

How it works: The `ls_trap` function does not take any argument (more precisely,
it takes *everything* as the argument), so the program does not need to prepare
registers for the call and thus does not interfere with any register allocators.
The `ls_trap` function itself, instead of behaving like an ordinary function,
immediately preserves all registers and calls the registered handler. When
returning from the handler, it recovers the old state and returns to the old
program as if nothing happened.

Comparing to signal handling (you can achieve the same thing by triggering a
segmentation fault), this does not go to the OS kernel, so you can use it
frequently. Comparing to the ex-POSIX `swapcontext()`, the `ls_trap` magical
function aims to minimise the impact on the stack structure, especially the
register usage. `swapcontext()` is an ordinary function that takes two arguments
which should be passed via registers on x86\_64.

Limitation: It only supports x86\_64 at this time.

TODO: Use swap-stack to run the handler on a *different* stack from the one
triggered the trap. This allows modifying the stack state.

# License

MIT license. See [LICENSE](LICENSE).

# Author

Kunshan Wang <wks1986@gmail.com>

<!--
vim: tw=80
-->
