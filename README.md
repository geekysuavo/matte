
# Matte

**ACHTUNG: This is a seriously pre-pre-alpha software package.** Nothing
is set in stone. Everything is buggy. I'll refactor almost all of it by
lunchtime tomorrow. That sort of spiel.

A framework for automatically compiling a superset/subset of
[MATLAB] (https://en.wikipedia.org/wiki/MATLAB) into 
[C] (https://en.wikipedia.org/wiki/C_%28programming_language%29).

This project spawned out of my growing curiosity about compiler design.
The goal is to produce a *compiled* open-source language with the
[syntax] (https://en.wikipedia.org/wiki/Syntax_%29programming_languages%29),
[weak] (https://en.wikipedia.org/wiki/Strong_and_weak_typing)
[dynamic typing] (https://en.wikipedia.org/wiki/Type_system), and
[objects] (https://en.wikipedia.org/wiki/Object-oriented_programming)
of MATLAB. Honestly, it's just a toy project that deviates from *bona fide*
MATLAB behavior whenever I feel like it. *:P*

## Generalities

The **matte** framework consists of three key elements:

 * The runtime library
 * The compiler
 * The REPL *(not yet!)*

The last two members of the above list depend on the first, because the
scanner, parser, syntax tree, *etc.* are components of the runtime
library.

The **matte** compiler transforms MATLAB into C source code,
or optionally to native machine code using the system's C compiler.
The C source code written by the compiler will be linked against the
runtime library, which provides types and builtins similar to the
MATLAB base types and built-in functions.

The **matte** [REPL] (https://en.wikipedia.org/wiki/Read–eval–print_loop)
is a primitive shell that dynamically compiles MATLAB code fragments
input by the user into shared objects, and loads them into the program
to be executed.

## Installation

The **matte** runtime library depends on the C API's to LAPACK and BLAS
provided by [ATLAS] (http://math-atlas.sourceforge.net). On OSX El Capitan,
this is a nightmare. Long story short, dump clang.

I successfully compiled ATLAS 3.10.2 using gcc-5.3.0 from homebrew-core,
with LAPACK 3.6.0, using the following commands:

```bash
brew install gcc --with-fortran
cd /usr/local/bin
sudo ln -sv gcc-5 gcc
cd ~/ATLAS
mkdir osx
cd osx
../configure -b 64 -O 12 -A 28 -V 480 --prefix=/usr/local/atlas \
  --with-netlib-lapack-tarfile=../../lapack-3.6.0.tar \
  -C alg gcc-5 -C if gfortran-5 \
  -Fa acg '-Xassembler -q'
make build
make check
make ptcheck
make time
sudo make install
```

## Licensing

The **matte** compiler and runtime library are released under the
[MIT license] (https://opensource.org/licenses/MIT). See the
[LICENSE.md] (LICENSE.md) file for the complete license terms.

Otherwise, enjoy!

*~ Brad.*

