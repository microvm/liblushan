# liblushan: Experimental Stack Introspection and Manipulation Library

|题西林壁           | Poem on the Wall of Xilin Temple                  |
|-------------------|---------------------------------------------------|
|[[宋](https://zh.wikipedia.org/wiki/%E5%AE%8B%E6%9C%9D)][苏轼](https://zh.wikipedia.org/wiki/%E8%8B%8F%E8%BD%BC)(1037-1101)| by [Su Shi](https://en.wikipedia.org/wiki/Su_Shi) ([Song Dynasty](https://en.wikipedia.org/wiki/Song_dynasty), 1037-1101)               |
|横看成岭侧成峰     | From side, cliff walls, yet a peak from base.     |
|远近高低各不同     | Far, near, high, low, never the same place.       |
|不识[庐山](https://zh.wikipedia.org/wiki/%E5%BA%90%E5%B1%B1)真面目     | Since I stand here within its midst,              |
|只缘身在此山中     | how can I know [Mount Lu](https://en.wikipedia.org/wiki/Mount_Lu)'s true face?              |

This is a testbed of some of my works on stack state introspection and manipulation, where on-stack replacement (OSR) is just one special case. One goal is to show the significance of the SWAP-STACK operation so that the observer will observe the target stack "from outside" (as opposed to "observing the mountain from within", where you never find the truth), which greatly simplfies the conceptual model as well as implementation.

This project works on native (C) programs from stock compilers. It depends on the application binary interface (ABI) which includes the calling convention and the standard stack-unwinding mechanisms (involving the `.eh_frame` section or the compact unwind info).

Just type `make` in the `src` directory to compile all executable files. `liblushan` currently only works on x86_64 on Linux and Mac OS X. On Linux, [libunwind](http://www.nongnu.org/libunwind/) needs to be installed.

There are some test programs in the `src` directory. Each `test_*.c` (and the `test_*.S` of the same base name if present) compiles to an executable file `text_*.out`.

- [src/test_realcallhandler.c](src/test_realcallhandler.c) demonstrates the `ls_trap` function which introspects the state of all registers at the call site, but does not currently perform SWAP-STACK.
- `src/test_remote_stack_*.c` use `libunwind` to print stack traces of a swapped-away stack, and also performs stack manipulation (OSR) to force a function activation (stack frame) to return earlier.

# License

MIT license. See [LICENSE](LICENSE).

# Author

Kunshan Wang <wks1986@gmail.com>

<!--
vim: tw=80
-->
