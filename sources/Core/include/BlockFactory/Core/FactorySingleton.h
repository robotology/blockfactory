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
#include "sharedlibpp/SharedLibrary.h"
#include "sharedlibpp/SharedLibraryClass.h"

#include <memory>

namespace blockfactory {
    namespace core {
        class Block;
        class ClassFactorySingleton;
    } // namespace core
} // namespace blockfactory

/**
 * @brief Class for interfacing with sharedlibpp plugin library
 *
 * This helper class ease the integration of sharedlibpp within the BlockFactory framework. It is
 * implemented with a singleton pattern.
 *
 * It can handle multiple plugin libraries together and provides support of destructing the related
 * factory objects (and hence unloading the plugin) when all the extracted classes have been
 * destroyed.
 */
class blockfactory::core::ClassFactorySingleton
{
private:
#ifndef DOXYGEN_SHOULD_SKIP_THIS
    class Impl;
    std::unique_ptr<Impl> pImpl;
#endif

public:
    using ClassFactory = sharedlibpp::SharedLibraryClassFactory<blockfactory::core::Block>;
    using ClassFactoryPtr = std::shared_ptr<ClassFactory>;

    /// @brief Name of the factory associated to the class specified during its registration
    using ClassFactoryName = std::string;

    /// @brief Name of the library object independent from the OS
    ///
    /// Given `libFoo.so`, this field would be `Foo`
    using ClassFactoryLibrary = std::string;

    /// Contains the data that indentifies a specific factory contained in a plugin library
    using ClassFactoryData = std::pair<ClassFactoryLibrary, ClassFactoryName>;

    ClassFactorySingleton();
    ~ClassFactorySingleton() = default;

    /**
     * @brief Get the singleton instance of the object
     *
     * @return The reference to the singleton object
     */
    static ClassFactorySingleton& getInstance();

    /**
     * @brief Get a factory object associated to a registered class
     *
     * It will lazy-allocate the factory related to the registered class at the first query.
     *
     * @param factorydata The identifier of a factory object
     * @return The pointer to the asked factory if it was loaded successfully, `nullptr` otherwise.
     */
    ClassFactoryPtr getClassFactory(const ClassFactoryData& factorydata);

    /**
     * @brief Ask to destroy a factory identified by the factory data
     *
     * @param factorydata The identifier of a factory object
     * @return True if the factory was destroyed, false otherwise
     */
    bool destroyFactory(const ClassFactoryData& factorydata);

    /**
     * @brief Add path to search for plugins
     *
     * @param path The new path to be added.
     */
    void extendPluginSearchPath(const std::string& path);
};

#endif
