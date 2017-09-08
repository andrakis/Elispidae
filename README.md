PocoLithp
=========

An implementation of [Lithp](https://github.com/andrakis/node-lithp) written in C++ and using the [Poco](https://procoproject.org) library.

Why?
----

An attempt at a much more lightweight implementation of a C++ Lithp (other attempt [Stackful](https://github.com/andrakis/Stackful) being rather more complicated than desired.

An improved version of the [90 line C++ Scheme interpreter](https://gist.github.com/ofan/721464) is included for experimentation and basis for the interpreter.


Status
------

* The basic scheme parser and interpreter is implemented using the Poco Dynamic Var as the underlying type.

* Most test cases are passing (3 failures).

* Does not use strings for numbers and arithmatic. Results in large speed improvement.

* No memory leaks! The only leakable type is the environment, but this is managed by the parent environment such that when the global environment dies, all child environments are cleared.

Building
--------

Uses [Conan](https://www.conan.io/) package manager and currently requires Visual Studio 2017 or higher. Support for other compilers will be implemented in time.

**NOTE:** Please ensure you select the appropriate build configuration in Visual Studio, according to your system settings.


1. Open the `PocoLithp.sln` file in Visual Studio 2017 or higher.

2. Install the required packages:

    * **Debug/x86**: `conan install -s arch=x86 -s build_type=Debug -s compiler.runtime=MDd`

	* **Release/x86**: `conan install -s arch=x86 -s build_type=Release -s compiler.runtime=MD`

    * ***Other build targets coming soon***

3. Open the Property Manager (`View -> Other Windows -> Property Manager` and add `conanbuildinfo.props` from the top level directory.

4. Build or run normally

**NOTE**: Switching build configurations requires performing steps 2 to 4 again to match the configuration.