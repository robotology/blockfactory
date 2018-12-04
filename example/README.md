# Toolbox Example

This folder contains a simple toolbox with a test block. It can be used as starting point to create a new toolbox based on the machinery provided by Whole Body Toolbox.

It is also the result of the tutorial [Toolbox Example](https://robotology.github.io/wb-toolbox/mkdocs/create_new_library/).

### Instructions

Execute the following commands to build the example:

```bash
cd example/
mkdir build
cd build
cmake ..
cmake --build .
```

On a Linux distribution, you can test the Toolbox Example library as follows:

```bash
cd example/
export LD_LIBRARY_PATH=$(pwd)/build:$LD_LIBRARY_PATH
export MATLABPATH=$(pwd)/matlab:$MATLABPATH
matlab
```

Finally, open the `Model.mdl` file and run it. 