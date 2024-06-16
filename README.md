# FBRS - Single Header Fibers

This simple library aims to provide building blocks for your own Fiber solution.

My plan for now is:
1. Have the "core" of this library be as portable as possible, not making any assumptions about the platform
or presence of libc etc. Just a few simple utilities to switch between "contexts" that work on each supported Arch and ABI.
   You can take the core and use it to build your own scheduler and synchronization primitives as you see fit.
2. On top of that I will build a scheduler that will be portable as well, but using libc and system threading libraries.
3. Third layer will build on the scheduler and will include synchronization primitives and async io in the future

TODO:
- [X] Windows x64 support
- [X] SysV (Unix) x64 support
- [X] ARM64 support
- [X] CPU context saving, loading and switching
- [ ] Scheduler
- [ ] Proper stack memory protection utils
- [ ] Fiber Local Storage
- [ ] gdb probes support
- [ ] valgrind stack awareness
- [ ] cooperative synchronization primitives (will require a scheduler)
- [ ] async io (if I get to it)

Based on the awesome [Fibers, Oh My!](https://graphitemaster.github.io/fibers/) blog post.
