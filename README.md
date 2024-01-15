# UNIX-style dynamic library example

A quick start guide to creating a UNIX-style dynamic library that calls functions from main program like a platform API.

## Getting started

### Requirements
- [Git](https://git-scm.com/downloads)
- [CMake](https://cmake.org/download/)
- C/C++ compiler (tested on [MSVC](https://visualstudio.microsoft.com/visual-cpp-build-tools/) and [LLVM/Clang](https://releases.llvm.org/download.html))

Optional:
- [Visual Studio Code](https://code.visualstudio.com/download)
- [Ninja](https://ninja-build.org/)
- [CCache](https://ccache.dev/download.html)
- [Windows SDK](https://developer.microsoft.com/en-us/windows/downloads/windows-sdk/) (Comes pre-installed when installing `MSVC`; `LLVM/Clang` alone will need this installed separately)
- [ms-vscode.cmake-tools](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools) vscode extension

### Usage
#### Demo
Either hit `F5` in vscode after configuring a launch target in `CMake Tools`, or issue the following commands in the project directory to try out the programs:
```
cmake -S . -B build && cmake --build build
bin\dltest
```

#### Mechanism
Unlike UNIX which only requires the symbols to be available before calling them like a platform API, the approach in Windows is hacky and restricted to the main program it explicitly links against - so the plugins cannot be used on different main programs.

The main program will produce an import library (`.lib`) file by using `CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS`, it will set a module-definition (`.def`) file output under the hood which will be used to generate the  `.lib` file for the plugin to link against.

While this demonstrates that it is possible to perform this UNIX-style library calling main program functions like a platform API, the proper way to share functions for library and main program is to move common code into another shared library where both will link against.

Do note that in Windows, it can still be compiled properly if the main program links against the common code as object/static library while the shared library links against the common code shared library, they will not share variable space, which will be different from expectations.