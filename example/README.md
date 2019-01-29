# Toolbox Example

This folder contains a simple toolbox with a test block. It can be used as starting point to create a new toolbox based on the machinery provided by BlockFactory.

It is also the result of the tutorial [Toolbox Example](https://robotology.github.io/blockfactory/mkdocs/create_new_library/).

### Build the example

Execute the following commands to build the example:

```bash
cd example/
mkdir build
cd build
cmake ..
cmake --build .
```

### Configure the environment

Add the absolute path of the `build` directory to the `BLOCKFACTORY_PLUGIN_PATH` environment variable.

### Open Simulink

Launch Matlab, make the `matlab/` folder of this example the current directory, and then open the `Model.mdl` file.

You should see the following Simulink model:

![Model.mdl](/doc/mkdocs/data/images/TestSimulinkModelWithMask.png)
