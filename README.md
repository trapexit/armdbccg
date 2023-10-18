# armdbccg: optimal ARM divide-by-constant code generator

This is a tool, based on ARM's divc.c app found in their old ARM
Software Development Toolkit, which will generate ARMv3 assembly to
divide an unsigned integer by a constant. This is an optimization due
to the lack of hardware supported division. The limitation is that the
constant must be either equal to (2^n - 2^m) or (2^n + 2^m).

The original tool output assembly for `armasm` and only generated
the quotient and remainder. This tool outputs a C/C++ header file with
3 static inlined inline assembly functions that can return either
quotient and remainder, just quotient, or just remainder.

This style was chosen over the assembly versions or a non-inline
function version so that the compiler can fully inline the code. The
compiler appears not to inline the function unless declared
`__inline`. If you wish for it to be called as a function and not
inlined simply remove the `__inline` declarator.


## Usage

### armdbccg

```
$ ./armdbccg --help
armdbccg: optimal ARM divide-by-constant code generator
Usage: ./build/armdbccg [OPTIONS] [divisor]

Positionals:
  divisor INT                 constant divisor

Options:
  -h,--help                   Print this help message and exit
  -v,--version                print version and exit
  -o,--output-path PATH       dirpath or filepath for output

$ ./armdbccg 10
header file written: div_by_10.h
```

### header

```C
#include "div_by_10.h"


unsigned x;


x = div_by_10_q(100);
```


## Notes

### headers

See https://github.com/trapexit/armdbccg/tree/master/headers for a
number of headers for available constant divs between 3 and 256.

### segfaults

There appears to be a bug in the `armcc` and `armcpp` compiler which
if the 'qr' version is used but the remainder variable in the 'div_t'
struct is not used it will segfault. It is currently unclear why this
occurs but without the source code there likely isn't anything that
can be done about it. If you don't need both values you should simply
use one of the other functions. Worse case remove the `__inline`
declarator.


## Documentation

* https://3dodev.com/documentation/hardware/opera/arm
* https://github.com/trapexit/armdbccg/blob/master/docs/Arm%20Programming%20Techniques.pdf
  * Find details on this techinque in section 5.4
* https://github.com/trapexit/armdbccg/blob/master/orig/divc.c


## Other Links

* 3DO Dev Repo: https://3dodev.com
* 'Modern' 3DO DevKit: https://github.com/trapexit/3do-devkit


## Support

Development and support of the projects I maintain require a
significant amount of time and effort.

If you are a personal user and find my software and its support
valuable and would like to support the project financially it would be
very much appreciated.

https://github.com/trapexit/support
