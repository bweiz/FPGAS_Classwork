# Utilities
This folder contains various utilities that are helpful during development. Feel free to add stuff to this folder.

## ARM cross-compile environment

`arm-env.sh` exports some environment variables to make cross-compiling your Linux kernel modules, device trees, etc. easier. It exports `CROSS_COMPILE` and `ARCH`, both of which the Linux kernel build system uses. 

It also changes the shell prompt to indicate that you are in the arm cross-compiling environment.

> [!IMPORTANT]
> Any time you need to compile a Linux kernel module or device tree, those environment variables need to be exported! If they aren't, you'll run into issues that might require recompiling the Linux kenrel

## Makefile

The Makefile in this folder is used for cross-compiling "normal" C code (i.e., not kenrel modules). It compiles code for x86 and ARM at the same time. This allows you to test your code on your x86 virtual machine, which can be helpful. Testing your code on your virtual machine is only fully possible for code that doesn't access memory-mapped I/O; when using memory-mapped I/O, you'd have to mock or comment-out the memory-mapped I/O operations in order to test your code on an x86 machine.
