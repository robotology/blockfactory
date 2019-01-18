# Install

!!! info "Disclaimer"
    BlockFactory has been widely tested on `Ubuntu 16:04` and `Ubuntu 18.04` with Matlab `R2017b`. If you face any issue either with your OS or Matlab version, please submit an [Issue](https://github.com/robotology/blockfactory/issues).

## Requirements

- Supported Operating Systems
	- GNU/Linux
	- macOS
	- Windows
- C++14 compiler
- CMake 3.5

BlockFactory provides three components, listed here below with their dependendencies. If a component dependency is not found, the component is not built. 

|                  | `Core` | `SimulinkCoder` | `Simulink`             |
| ---------------- | ------ | --------------- | ---------------------- |
| **Dependencies** | ~      | ~               | - Matlab<br>- Simulink |

!!! note
    Simulink Coder is not a build dependency of the `Coder` component. Of course, you must have it if you want to generate C++ code from a Simulink Model.

## Installation

!!! warning
    The following instructions are for Unix-like systems, but they work similarly on other operating systems.

```sh
git clone https://github.com/robotology/blockfactory.git
mkdir -p blockfactory/build && cd blockfactory/build
cmake .. -DCMAKE_INSTALL_PREFIX=<install-prefix>
cmake --build . --config Release
cmake --build . --config Release --target install
```

!!! note
    From now on, this guide refers to your install directory with the variable `<install-prefix>`. Every time you see this variable, you should substitute the absolute install path.

## Configuration

### Simulink and Simulink Coder

BlockFactory provides the support of:

- **Simulink** by shipping a generic [MEX Level-2 S-Function S-Functions](https://it.mathworks.com/help/simulink/sfg/what-is-an-s-function.html)
- **Simulink Coder** by shipping a TLC file to achieve [Wrapper Inlined S-Functions](https://it.mathworks.com/help/rtw/tlc/inlining-s-functions.html)

These two files are installed in the `<install-prefix>/mex` folder, which should be added to the [Matlab search path](https://it.mathworks.com/help/matlab/matlab_env/what-is-the-matlab-search-path.html).
