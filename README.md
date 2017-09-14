PocoLithp
=========

An implementation of [Lithp](https://github.com/andrakis/node-lithp) written in C++14 and using the [Poco](https://procoproject.org) library.

Why?
----

An attempt at a much more lightweight implementation of a C++ Lithp (other attempt [Stackful](https://github.com/andrakis/Stackful) being rather more complicated than desired.)

An improved version of the [90 line C++ Scheme interpreter](https://gist.github.com/ofan/721464) is included for experimentation and basis for the interpreter.


Status
------

**Language compatibility level**: Scheme

    Currently only supports a Scheme syntax. Most Lithp-specific types not implemented.

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


Building (Conan / Visual Studio)
--------------------------------

Uses [Conan](https://www.conan.io/) package manager and currently requires Visual Studio 2015 or higher. Support for other compilers will be implemented in time.

**NOTE:** Please ensure you select the appropriate build configuration in Visual Studio, according to your system settings.

On Visual Studio:
-----------------

1. Open the correct solution file:

   * For Visual Studio 2017 or higher, open `PocoLithp.sln`

   * For Visual Studio 2015, open `PocoLithp2015.sln`

2. Install the required packages depending on the build type you want:

    * **NOTE:** Conan automatically determines the `arch` to build for based on your system, unless you override it. To simplify configuration, pick based on the profile you want to run.

	* **NOTE:** On Linux, remove the `-s compiler.runtime=...` parameter.

    * **Debug/x86**: `conan install -s arch=x86 -s build_type=Debug -s compiler.runtime=MDd`

    * **Release/x86**: `conan install -s arch=x86 -s build_type=Release -s compiler.runtime=MD`

    * **Debug/x64**: `conan install -s arch=x86_64 -s build_type=Debug -s compiler.runtime=MDd`

    * **Release/x64**: `conan install -s arch=x86_64 -s build_type=Release -s compiler.runtime=MD`

3. Open the Property Manager (`View -> Other Windows -> Property Manager` and `Add existing property sheet` using `conanbuildinfo.props` from the top level directory. (The icon next to the save icon in the Property Manager.)

4. Build or run normally

**NOTE:** Switching build configurations requires performing steps 2 to 4 again to match the configuration.

Building (Linux):
-----------------

This section needs work.

1. Update submodules: `git submodule update --init`

2. Use `make`:

   For building on most systems:

    make CONF=Release

    make CONF=Debug

  For building on OpenRISC 1000:

    make CONF=Release CC=or1k-linux-musl-gcc CXX=or1k-linux-musl-g++ CXXFLAGS=-DPOCO_ARCH_OR1000=0x10 -DPOCO_ARCH=POCO_ARCH_OR1000 -DPOCO_ARCH_LITTLE_ENDIAN=1 -D__EMSCRIPTEN__

