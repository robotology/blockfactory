# BlockFactory

BlockFactory is a framework for dataflow programming that allows **wrapping C and C++ algorithms**. It provides an abstraction inspired by Simulink where algorithms are represented as _blocks_ that can exchange data through _signals_.

Users of this project can easily wrap their algorithms inside the provided software interfaces obtaining C++ classes that can be executed by generic _engines_. BlockFactory implements and provides full support of the following engines:

- `Simulink`
- `Simulink Coder`

The main purpose of BlockFactory is **rapid prototyping** of discrete systems (stateless and stateful). Taking as an example the supported Simulink engine, it simplifies and streamlines the process of creating a block. You don't have be an expert of its complex APIs! Furthermore, with Simulink Coder you can generate C++ code from a Simulink model. You can then deploy the automatically generated C++ class from an arbitrarily complex model to your preferred target platform.

The same code of the blocks is shared among all the supported engines. This means that the same shared library containing your blocks can either be loaded into Simulink during runtime or called by the executable built from the automatically generated code, without any modification.

If you need to run your code inside an unsupported engine (e.g. Scilab), you can implement it yourself with minimal effort.

### How we conceived BlockFactory

BlockFactory for many years represented the back-end of [robotology/wb-toolbox](https://github.com/robotology/wb-toolbox), a Simulink Toolbox for rapid prototyping of humanoid robots whole-body controllers. The toolbox was recently refactored and its core components became BlockFactory.

Despite its roots in robotics, now BlockFactory is very generic! You can use it to wrap your own C / C++ libraries to simulate power converters, design communication systems, benchmark motor drives, interface with exotic devices, and all applications that come in your mind. Its potential is endless.

> _Build bridges, not walls_

### Acknowledgments

The development of WB-Toolbox is supported by:

- FP7 EU projects CoDyCo (No. 600716 ICT 2011.2.1 Cognitive Systems and Robotics)
- H2020 EU projects AnDy (No. 731540 H2020-ICT-2016-1)
