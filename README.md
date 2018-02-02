Elispidae (Elisp)
=================

***Formerly PocoLithp***

An implementation of [Lithp](https://github.com/andrakis/node-lithp) written in C++14 and using the [Stackless](https://github.com/andrakis/Stackless) library.

It is a mix of Erlang and Common Lisp - the minimalist syntax of Scheme but without most of the advanced features of Scheme, more similar to Common Lisp.

Features taken from Scheme:

   * Minimalist syntax and interpreter

   * Shared namespace for procedures and variables

   * Tail-call-optimization

Features taken from Common Lisp:

   * Dynamic scoping

   * Fixed data types

       * Integers (signed and unsigned)

       * Floats (stored as double)

       * Atoms (integers), AKA symbols

       * Strings

       * Lists

       * Dictionaries

       * Lambdas

       * Procs

Features taken from Erlang:

   * Atoms: `lowercase` is atom

   * Variables: `Uppercase` is a variable reference, and will be looked up

   * Microthreading: Running multiple scripts at a time. Incorporates message passing.

It is extremely fast owing to its dynamic runtime types and reliance on atoms (numbers) rather than strings, and tight memory management.

The main execution loop is essentially looking up integers in a vector table and performing arithmatic or decisions on them.

Why?
----

An attempt at a much more lightweight implementation of a C++ Lithp (other attempt [Stackful](https://github.com/andrakis/Stackful) being rather more complicated than desired.)

What?
-----

Elisp implements a working and extremely fast Scheme-based interpreter. See the [tests](https://github.com/andrakis/Elispidae/blob/master/PocoLithp/PLtests.cpp) for syntax examples.

It supports most modern types:

   * **Comments:** `;; Comments use double semicolon`

   * **Numbers:** `1`, `2.34`, `-3.14e159`, `0xDEADBEEF`

   * **Atoms:** `most` `symbols` `are` `atoms`, additionally an `'extended atom syntax'` is available.

   * **Variables:** `Uppercase` symbols are Variables, and will be resolved to their value at runtime. This replaces atoms/symbols being used to get values from the environment.

   * **Strings:** "Strings are supported"

   * **Lists:** `(a 1 (2 b (3 c)))`

   * **Lambdas:** `(lambda (A B C) (+ A B C))`, `(lambda (N) (begin (test) (+ N 1)))`

   * **Lithp-like Lambdas:** `(# (A B C) (+ A B C))`, `(# (N) (begin (test) (+ N 1)))`

   * **Macros:** `(define ifDefined (macro (Name) (_eval (list if (list defined Name) (list get! Name) none))))` - macros are simply lambdas whose arguments are not evaluated before passing, and use the environment of their callee so have access to the callees symbols. Abritrary code can be executed, such as in this snippet where an AST is created that checks if the given variable is defined, and returns its value or the atom `none`.

   * **Procs:** C++ code with a signature: `LithpCell func_name(const LithpCells &args)`

   * **ExtendedProcs:** C++ code with a signature: `LithpCell func_name(const LithpCells &args, Env_p env)`

How?
----

   * Built upon the now deprecated [PocoLithp](https://github.com/andrakis/PocoLithp) project.

   * [Stackless](https://github.com/andrakis/Stackless) provides microthreading and a stackless interpreter.

   * Uses atoms for most features (builtins, standard library functions) instead of strings. An atom is a 32 bit integer, much quicker to lookup than a string.

   * [Poco](https://procoproject.org) library is used for the underlying dynamic type. Some extensions to this allow for minimal coding and maximum support.

   * Memory is almost universally managed by the standard template library and restricting the use of pointers in code. `shared_ptr`s take care of environment management, resolving memory issues.

   * Code is data! Data is code!

      * Say your code is: `(begin (define add1 (lambda (X) (+ X 1)))(print "Add1:" (add1 5)))`

	  * You could store this to a variable: `(define Code (quote (begin (define add1 (lambda (X) (+ X 1)))(print "Add1:" (add1 5)))))`

	  * You can print it out in readable form: `(str Code)` => `(begin (define add1 (lambda (X) (+ X 1))) (print Add1: (add1 5)))`

	  * Or you can print the underlying representation: `(str Code true)` => `(8 (6 51 (7 (52) (24 52 1))) (44 "Add1:" (51 5)))` **(individual results may vary)**

	  * And you can run it too: `(_eval Code)` => `Add1: 6`

Status
------

**Version: 0.81**

**Language compatibility level:** Lisp-ish, with tail-call-optimization and macros.

   Mainly Lisp-like syntax, mixed with Lithp (Variables are introduced, `#` is synonym for `lambda`).

   Microthreading and [crude message passing](https://github.com/andrakis/Elispidae/blob/master/samples/threading.lisp) implemented. (See branch [pull request #1](https://github.com/andrakis/Elispidae/pull/1) for progress on proper message passing and thread management.)

Building
========

* Requires Visual Studio 2015 or G++ 4.9.1 or higher.

* Coming soon: CMake support

Building (Visual Studio)
------------------------

This solution does not use Conan, and is the recommended way forward. It does require the large Poco library as a submodule though.

1. Update submodules: `git submodule update --init`

2. Open the solution `Elispidae` and compile.

Building (Linux):
-----------------

1. Update submodules: `git submodule update --init`

2. Use `make`:

   For building on most systems:

     `make CONF=Release`

     `make CONF=Debug`

   For building on OpenRISC 1000:

     `make CONF=Release CC=or1k-linux-musl-gcc CXX=or1k-linux-musl-g++ CXXFLAGS="-D__EMSCRIPTEN__ -static" CCFLAGS="-static"`

     **Note:** POCO does not currently support OpenRISC 1000, however we can fake enough with the above flags that it compiles and runs successfully.

     **Note:** The `-static` flag is required when building with anything other than GCC 4.9.1.

Running
-------

The executable is found in the following location:

* **Windows:** `bin/Elispidae.exe`

* **Linux:** `bin/elisp`

Two methods are available:

* **REPL:**

      Launch the `Debug` or `Release` configuration within Visual Studio, or

      Run the executable:
      bin\Elispidae.exe
      bin/elisp

* **Run a file:** Run the executable

      Launch the `Debug - Run Sample` configuration within Visual Studio, or

      Run the executable:
      bin\Elispidae.exe lib\init.lisp
      bin/elisp lib/init.lisp

Notable Milestones
------------------

* Crude [message passing](https://github.com/andrakis/Elispidae/blob/master/samples/threading.lisp) implemented. See [pull request #1](https://github.com/andrakis/Elispidae/pull/1) for progress on proper message passing.

* Project renamed from PocoLithp to Elispidae.

* Microthreading implemented.

* Stackless interpreter complete.

* Start incorporating Stackless interpreter.

* Macros are implemented. They function like lambdas, except their arguments are not evaluated before passing.

* Fixed a number of issues that could result in a segfault, crashing the interpreter. Argument counts are now checked.

* Fixes multiplication operator dropping floating point value.

* Adds some better string handling.

* Now using [Linenoise-ng](https://github.com/arangodb/linenoise-ng) for REPL input.

* Lists can now be compared

* Improved builtin parameter passing. `proc_type` is no longer passed an environment, if needed use `extended_proc`.

* Full tail-call-optimization has been implemented. Essentially, infinite stack depth is available (for [properly tail-recursive functions](https://github.com/andrakis/PocoLithp/blob/master/samples/fac_recursive.lisp).) This involved resolving more memory management issues.

* Comment handling improved, no longer able to crash interpreter by entering a comment into the REPL.

* Memory leaks have been plugged by using `std::shared_ptr`. Unfortunately tail call optimization code had to be dropped. It may return in the future.

* Comments `;;` are now supported. Parser also handles whitespace better now.

* Reduction capability: lambdas now reduce too.

* Reduction capability: `if` and `lambda` now save an `eval` call, resulting in faster execution for tail recursive code. This may still be improved.

* Can now run sample files via command line argument.

* Adds multi-line support, tabs no longer result in invalid code.

* The REPL is now the standard mode of operation.

* Added comparison operators `=` (or `==`), `!=`.

* Added exception handling.

* Now compiles on Linux under G++ (4.9.1 or higher, C++14 support required.) without need for Conan.

* Implements <code>"strings"</code> and <code>'extended atom syntax'</code>

* Displays eval and parse times

* Implemented Atom type. All Symbols are now Atoms, uint32 instead of strings, making comparisons much quicker.

* Fixed comparison operators (were using strings for comparisons), resulting in correct behaviour and speed improvements.

* Speed is close to Lithp speed running on a web browser. About 20% slower than Lithp (aka V8 / JavaScript / Node.js)

* No memory leaks! The basic types use the standard templates, avoiding manual memory management. The only leakable type is the environment, but this is managed by the parent environment such that when the global environment dies, all child environments are cleared.

* All test cases passing!

* Does not use strings for numbers and arithmatic. Results in large speed improvement.

* The basic scheme parser and interpreter is implemented using the Poco Dynamic Var as the underlying type.

Building (Conan / Visual Studio) (DEPRECATED)
---------------------------------------------

**NOTE:** Due to the complexity of this method, it is now deprecated in favour of the `PocoLithpContrib` solution.

Uses [Conan](https://www.conan.io/) package manager and currently requires Visual Studio 2015 or higher. Support for other compilers will be implemented in time.

**NOTE:** Please ensure you select the appropriate build configuration in Visual Studio, according to your system settings.

On Visual Studio:
-----------------

1. Open the correct solution file:

   * For Visual Studio 2017 or higher, open `PocoLithp.sln`

   * For Visual Studio 2015, open `PocoLithp2015.sln`

2. Install the required packages depending on the build type you want:

    * **NOTE:** Conan automatically determines the `arch` to build for based on your system, unless you override it. To simplify configuration, pick based on the profile you want to run.

    * **Debug/x86**: `conan install -s arch=x86 -s build_type=Debug -s compiler.runtime=MDd`

    * **Release/x86**: `conan install -s arch=x86 -s build_type=Release -s compiler.runtime=MD`

    * **Debug/x64**: `conan install -s arch=x86_64 -s build_type=Debug -s compiler.runtime=MDd`

    * **Release/x64**: `conan install -s arch=x86_64 -s build_type=Release -s compiler.runtime=MD`

3. Open the Property Manager (`View -> Other Windows -> Property Manager` and `Add existing property sheet` using `conanbuildinfo.props` from the top level directory. (The icon next to the save icon in the Property Manager.)

4. Build or run normally

**NOTE:** Switching build configurations requires performing steps 2 to 4 again to match the configuration.

