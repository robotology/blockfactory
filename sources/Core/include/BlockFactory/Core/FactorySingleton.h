/*
 * Copyright (C) 2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * GNU Lesser General Public License v2.1 or any later version.
 */

#ifndef BLOCKFACTORY_CORE_FACTORYSINGLETON_H
#define BLOCKFACTORY_CORE_FACTORYSINGLETON_H

#include "BlockFactory/Core/Log.h"
#include "shlibpp/SharedLibrary.h"
#include "shlibpp/SharedLibraryClass.h"

#include <memory>

namespace blockfactory {
    namespace core {
        class Block;
        class ClassFactorySingleton;
    } // namespace core
} // namespace blockfactory

class blockfactory::core::ClassFactorySingleton
{
private:
    class Impl;
    std::unique_ptr<Impl> pImpl;

public:
    using ClassFactory = shlibpp::SharedLibraryClassFactory<blockfactory::core::Block>;
    using ClassFactoryPtr = std::shared_ptr<ClassFactory>;

    // This is the name of the factory associated to the class
    // specified during its registration
    using ClassFactoryName = std::string;

    // This is the name of the library object independent from the OS.
    // E.g. Given "libFoo.so", this field would be "Foo".
    using ClassFactoryLibrary = std::string;

    using ClassFactoryData = std::pair<ClassFactoryLibrary, ClassFactoryName>;

    ClassFactorySingleton();
    ~ClassFactorySingleton() = default;

    static ClassFactorySingleton& getInstance();
    //    ClassFactoryPtr getClassFactory(const ClassFactoryData& factorydata);
    ClassFactoryPtr getClassFactory(const ClassFactoryData& factorydata);
};

#endif
