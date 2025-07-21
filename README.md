# Kurios32
A 32-bit kernel, written in C++, that can (probably) run on real hardware.

I work on this a lot, and it is often a mess. If it doesn't compile, use a previous commit. I try to make a note of when the kernel is messed up. I use Github to archive the kernel's code.


## Features:
 - A working command-line interpreter
 - Some basic commands
 - Colored text!
 - A logging system

# TODO:
 - Handle arrow keys
 - Handle delete
 - binutils
 - coreutils
 - networking
 - Documentation for new developers
 - Mathematics library
 - Graphics driver
 - Make subcmd something made available to the shell once and is reused
 - Scroll up and down with page up and page down
 - Text editor
 - Logging
 - An installer
 - Customized prompts

 
# Bugs:
 - Some warnings are given when kernel is compiled.
 - ls produces two slashes, and they are on the same line
 - cat is not recognized if you do not add a space after it
