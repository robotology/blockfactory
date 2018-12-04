/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef SHAREDLIBPP_SHAREDLIBRARYCLASSAPI_H
#define SHAREDLIBPP_SHAREDLIBRARYCLASSAPI_H

#include <shlibpp/api.h>
#include <shlibpp/config.h>
#include <cstring>

namespace shlibpp {

// Be careful loading C++ classes from DLLs.  Generally you
// need an exact or very close match between compilers used
// to compile those DLLs and your own code.

extern "C" {

    /**
     * Collection of hooks for creating/destroying a plugin.
     * Be careful to check carefully for compatibility before
     * using create() or destroy().
     */
    struct SHLIBPP_API SharedLibraryClassApi {
    public:
        int32_t startCheck;    // A 32-bit integer that is checked when loading
                               // a plugin.
                               // Don't touch anything further if it isn't.
        int32_t structureSize; // size of the SharedLibraryClassApi.
                               // If this doesn't match what you expect,
                               // Don't touch anything further if it isn't.
        int32_t systemVersion; // Overall version of plugin system.
                               // This does *not* cover compiler version etc.

        using createFn_t = void*(*)();
        using destroyFn_t = void(*)(void*);
        using getFn_t = int32_t(*)(char*, size_t);

        createFn_t create;        // Instantiate a plugin object.
        destroyFn_t destroy;      // Destroy a plugin object.
        getFn_t getVersion;       // Plugin-related version.
        getFn_t getAbi;           // Compiler-related version.
        getFn_t getClassName;     // Name of plugin (subclass).
        getFn_t getBaseClassName; // Name superclass.

        int32_t roomToGrow[SHLIBPP_SHAREDLIBRARYCLASSAPI_PADDING]; // Padding.
        int32_t endCheck;      // A 32-bit integer that is checked when loading
                               // a plugin.
    };

} // extern "C"

} // namespace shlibpp


#define SHLIBPP_SHARED_CLASS_FN extern "C" SHLIBPP_EXPORT

constexpr int32_t SHLIBPP_DEFAULT_START_CHECK =
    static_cast<int32_t>('S') +
    (static_cast<int32_t>('H') << 8) +
    (static_cast<int32_t>('P') << 16) +
    (static_cast<int32_t>('P') << 24);
constexpr int32_t SHLIBPP_DEFAULT_END_CHECK =
    static_cast<int32_t>('P') +
    (static_cast<int32_t>('L') << 8) +
    (static_cast<int32_t>('U') << 16) +
    (static_cast<int32_t>('G') << 24);
constexpr int32_t SHLIBPP_DEFAULT_SYSTEM_VERSION = 5;
#define SHLIBPP_DEFAULT_FACTORY_NAME shlibpp_default_factory

#define SHLIBPP_STRINGIFY2(X) #X
#define SHLIBPP_STRINGIFY(X) SHLIBPP_STRINGIFY2(X)
#define SHLIBPP_DEFAULT_FACTORY_NAME_STRING SHLIBPP_STRINGIFY(SHLIBPP_DEFAULT_FACTORY_NAME)


/**
 * Macro to create a bunch of functions with undecorated names that can
 * be found within a plugin library to handle creation/deletion of that
 * plugin.  Use with care.
 *
 * @param factoryname the name of the "hook" function to make.
 *        A collection of other helper functions with names composed of the
 *        factoryname with _create/_destroy/... appended.
 * @param startcheck a 32-bit integer that is checked when loading a plugin.
 * @param endcheck a 32-bit integer that is checked when loading a plugin.
 * @param systemversiona a 32-bit integer representing the version of the plugin
 *        api that is checked when loading a plugin.
 * @param classname the class that the hook will be able to instantiate.
 * @param basename the superclass that the user of the plugin should be
 *        working with.
 */
#define SHLIBPP_DEFINE_SHARED_SUBCLASS_CUSTOM(startcheck, endcheck, systemversion, factoryname, classname, basename) \
    SHLIBPP_SHARED_CLASS_FN void* factoryname ## _create () \
    { \
        classname* cn = new classname; \
        basename* bn = dynamic_cast<basename*>(cn); \
        if (!bn) { \
            delete cn; \
        } \
        return static_cast<void*>(bn); \
    } \
    \
    SHLIBPP_SHARED_CLASS_FN void factoryname ## _destroy (void* obj) \
    { \
        classname* cn = dynamic_cast<classname*>(static_cast<basename*>(obj)); \
        if(cn) { \
            delete cn; \
        } \
    } \
    \
    SHLIBPP_SHARED_CLASS_FN int32_t factoryname ## _getVersion (char* ver, size_t len) \
    { \
        return 0; \
    } \
    \
    SHLIBPP_SHARED_CLASS_FN int32_t factoryname ## _getAbi (char* abi, size_t len) \
    { \
        return 0; \
    } \
    \
    SHLIBPP_SHARED_CLASS_FN int32_t factoryname ## _getClassName (char* name, size_t len) \
    { \
        char cname[] = # classname; \
        strncpy(name, cname, len); \
        return strlen(cname) + 1; \
    } \
    \
    SHLIBPP_SHARED_CLASS_FN int32_t factoryname ## _getBaseClassName (char* name, size_t len) \
    { \
        char cname[] = # basename; \
        strncpy(name, cname, len); \
        return strlen(cname) + 1; \
    } \
    \
    SHLIBPP_SHARED_CLASS_FN int32_t factoryname(void* api, size_t len) { \
        struct shlibpp::SharedLibraryClassApi* sapi = static_cast<struct shlibpp::SharedLibraryClassApi*>(api); \
        if (len < sizeof(shlibpp::SharedLibraryClassApi)) { \
            return -1; \
        } \
        sapi->startCheck = startcheck; \
        sapi->structureSize = sizeof(shlibpp::SharedLibraryClassApi); \
        sapi->systemVersion = systemversion; \
        sapi->create = factoryname ## _create; \
        sapi->destroy = factoryname ## _destroy; \
        sapi->getVersion = factoryname ## _getVersion; \
        sapi->getAbi = factoryname ## _getAbi; \
        sapi->getClassName = factoryname ## _getClassName; \
        sapi->getBaseClassName = factoryname ## _getBaseClassName; \
        for (int i=0; i<SHLIBPP_SHAREDLIBRARYCLASSAPI_PADDING; i++) { \
            sapi->roomToGrow[i] = 0; \
        } \
        sapi->endCheck = endcheck; \
        return sapi->startCheck; \
    }
// The double cast in the _create() and _destroy() functions are
// required to ensure that everything works when `basename` is not the
// first inherited class:
// _create() will return a valid `basename` or a null pointer if
// `classname` does not inherit from `basename`.
// _destroy() will ensure that we are calling `classname` destructor
// even if `basename` is not the first inherited class. If the
// dynamic_cast fails, it will not delete the object (that is probably
// leaked), but it is less dangerous than executing some other random
// function.

#define SHLIBPP_DEFINE_SHARED_SUBCLASS(factoryname, classname, basename) \
    SHLIBPP_DEFINE_SHARED_SUBCLASS_CUSTOM(SHLIBPP_DEFAULT_START_CHECK, \
                                          SHLIBPP_DEFAULT_END_CHECK, \
                                          SHLIBPP_DEFAULT_SYSTEM_VERSION, \
                                          factoryname, \
                                          classname, \
                                          basename)

#define SHLIBPP_DEFINE_DEFAULT_SHARED_CLASS(classname) \
    SHLIBPP_DEFINE_SHARED_SUBCLASS_CUSTOM(SHLIBPP_DEFAULT_START_CHECK, \
                                          SHLIBPP_DEFAULT_END_CHECK, \
                                          SHLIBPP_DEFAULT_SYSTEM_VERSION, \
                                          SHLIBPP_DEFAULT_FACTORY_NAME, \
                                          classname, \
                                          classname)

#define SHLIBPP_DEFINE_SHARED_CLASS(factoryname, classname) \
    SHLIBPP_DEFINE_SHARED_SUBCLASS_CUSTOM(SHLIBPP_DEFAULT_START_CHECK, \
                                          SHLIBPP_DEFAULT_END_CHECK, \
                                          SHLIBPP_DEFAULT_SYSTEM_VERSION, \
                                          factoryname, \
                                          classname, \
                                          classname)

#endif // SHAREDLIBPP_SHAREDLIBRARYCLASSAPI_H
