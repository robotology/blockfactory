# Introduction

One of the big advantages of dataflow programming resides in the visual programming tools that are usually associated to it. Blockfactory, however, mainly targets developers of such frameworks, aiming to simplify their learning curve that too often is very steep. We at the Italian Institute of Technology are mainly researchers and PhD students, and the possibility of translating in short time volatile ideas to a first prototypical implementation is paramount.

BlockFactory represents an excellent tool for prototyping. By exploiting its general purpose features, we can expose every C++ algorithm we may need in very short time to other users that just have to use the new blocks that wrap their functionalities to a very high abstraction level.

Though, this is only the first step of development. When an arbitrarily complex model (usually in Simulink) becomes mature enough, we would also like to deploy it to the target platform. With the help of Simulink Coder, the models created with blocks based on BlockFactory can be translated to a C++ library passing through a code autogeneration process. At this point, the library can be compiled (or cross-compiled) and effectively deployed to the target device.

If you want to learn how to exploit our framework to create something similar for your own applications, you can start from here:

- [How to create a new Simulink Library](create_new_library.md)
- [How to autogenerate C++ code](autogenerate_code.md)
