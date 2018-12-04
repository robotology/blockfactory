Simple cross platform plug-in system
====================================

The sharedlibpp is a tiny cross-platform library to create and load shared
libraries for different platform (Linux/Mac/Windows). The sharedlibpp provide
an easy and portable way to create plug-ins which encapsulate your c++ classes
inside a shared library (so, dylib, dll).
The original code is taken and from
[YARP (Yet Another Robot Platform)](http://www.yarp.it/).
The code is simplified by removing dependencies and some helper functions are
added to report the native OS error messages on failures.


Building on Linux/Mac
---------------------
    $ cd sharedlibpp
    $ mkdir build; cd build
    $ cmake ../; make


Building on Windows
-------------------
The easiest way is to use Cmake to create VStudio project. To do that:

* download and install [cmake](http://www.cmake.org/download/) for windows.
* open the cmake gui and set the source path to `sharedlibpp` and set the
  build path (for example `sharedlibpp/build`).
* configure and generate visual studio project.
* open the project and built it.

Running example
---------------
The build system by defualt compiles and build the examples.

* On Linux/Mac
```
    $ cd sharedlibpp/build/examples
    $ ./math_test mymath.so
    $ ./math_test_custom mymathcustom.so
```
* On Windows first switch to directory where example is created and then
```
    > math_test mymath.dll
    > math_test_custom mymathcustom.dll
```
