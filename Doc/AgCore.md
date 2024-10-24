# AG Core - Documentation

AG (inspired by the periodic table symbol for Silver) represents a set of
non-domain-specific libraries which underpin the Mighty Oak project and may
go on to form the basis of other projects.

AG Core is the lowest level library. It provides the most basic functionality
for up-stream code. It is dependent upon the platform C Runtime Library
(currently MSVC Runtime and glibc), the C++ Standard Template Library and, in
the case of Windows, the Win32 subsystem and shell. Currently AG Core is not
dependent upon any third party libraries which are not available to the host
platform as standard.

The library is intended mostly for internal use. It is completely annotated
with Doxygen comments, although could benefit from more detailed explanation
about various topics. Compiling the Documentation CMake project will yield
this information and in .chm form if the HTML Help compiler is available
under Windows.

Some notable features of AG Core are:
* Optimised low-level binary operations (bit scan, byte order, etc.).
* Tools for dynamic management of in-line memory blocks.
* Enumeration metadata tools.
* Support for Unicode conversions.
* Immutable UTF-8 strings.
* Symbol extraction and packaging for use in stack traces.
* An Immutable exception class hierarchy with stack traces.
* Try/Catch mechanism for hardware exceptions.
* A robust application framework including command line handling.
* Optimised sorted linear maps and sets.
* String formatting using type-safe variable arguments.
* URI management.
* File path management using both Win32 and POSIX-style paths.
* File system integration.

# QtInterop

This is a library which provides basic utilities for working Qt6 and
allowing the AgCore library to inter-operate with it.