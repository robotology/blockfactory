/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef SHAREDLIBPP_SHAREDLIBRARYFACTORY_H
#define SHAREDLIBPP_SHAREDLIBRARYFACTORY_H

#include <shlibpp/api.h>
#include <string>

namespace shlibpp {

class SharedLibraryClassApi;

/**
 * A wrapper for a named factory method in a named shared library.
 * This wrapper will do some basic checks that the named method does
 * indeed behave like a shlibpp plugin hook before offering access to it.
 * This is to avoid accidents, it is not a security mechanism.
 */
class SHLIBPP_API SharedLibraryFactory
{
public:
    /**
     * The status of a factory can be:
     *  - None: Not configured yet
     *  - OK: Present and sane
     *  - LibraryNotFound: Named shared library was not found
     *  - LibraryNotLoaded: Named shared library failed to load
     *  - FactoryNotFound: Named method wasn't present in library
     *  - FactoryNotFunctional: Named method is not working right
     */
    enum class Status : std::uint32_t
    {
        None = 0,            //!< Not configured yet.
        OK,                  //!< Present and sane.
        LibraryNotFound,     //!< Named shared library was not found.
        LibraryNotLoaded,    //!< Named shared library failed to load.
        FactoryNotFound,     //!< Named method wasn't present in library.
        FactoryNotFunctional //!< Named method is not working right.
    };

    /**
     * Constructor for unconfigured factory with custom start check, end check,
     * system version and factory name.
     *
     * @param startCheck a 32-bit integer that is checked when loading a plugin.
     *                   It must be the same used when creating the plugin
     * @param endCheck a 32-bit integer that is checked when loading a plugin.
     *                   It must be the same used when creating the plugin
     * @param systemVersion a number representing the version of the plugin api
     *                      that is checked when loading a plugin.
     *                      It must be the same used when creating the plugin.
     * @param factoryName name of factory method, a symbol within the shared
     *                    library.
     *                    If set, it must be the same used when creating the
     *                    plugin.
     */
    explicit SharedLibraryFactory(int32_t startCheck = -1,
                                  int32_t endCheck = -1,
                                  int32_t systemVersion = -1,
                                  const char *factoryName = nullptr);

    /**
     * Constructor with custom start check, end check, system version and
     * factoryName
     *
     * @param dll_name name/path of shared library.
     * @param startCheck a 32-bit integer that is checked when loading a plugin.
     *                   It must be the same used when creating the plugin
     * @param endCheck a 32-bit integer that is checked when loading a plugin.
     *                   It must be the same used when creating the plugin
     * @param systemVersion a number representing the version of the plugin api
     *                      that is checked when loading a plugin.
     *                      It must be the same used when creating the plugin.
     * @param factoryName name of factory method, a symbol within the shared
     *                    library.
     *                    If set, it must be the same used when creating the
     *                    plugin.
     */
    SharedLibraryFactory(const char *dll_name,
                         int32_t startCheck = -1,
                         int32_t endCheck = -1,
                         int32_t systemVersion = -1,
                         const char *factoryName = nullptr);

    /**
     * Constructor with default start check, end check and system version.
     *
     * @param dll_name name/path of shared library.
     * @param factoryName name of factory method, a symbol within the shared
     *                    library.
     *                    If set, it must be the same used when creating the
     *                    plugin.
     */
    SharedLibraryFactory(const char *dll_name,
                         const char *factoryName = nullptr);

    /**
     * Destructor
     */
    virtual ~SharedLibraryFactory();

    /**
     * Configure the factory.
     *
     * @param dll_name name/path of shared library.
     * @param startCheck a 32-bit integer that is checked when loading a plugin.
     *                   It must be the same used when creating the plugin
     * @param endCheck a 32-bit integer that is checked when loading a plugin.
     *                   It must be the same used when creating the plugin
     * @param systemVersion a number representing the version of the plugin api
     *                      that is checked when loading a plugin.
     *                      It must be the same used when creating the plugin.
     * @param factoryName name of factory method, a symbol within the shared
     *                    library.
     *                    If set, it must be the same used when creating the
     *                    plugin.
     * @return true on success.
     */
    bool open(const char *dll_name,
              int32_t startCheck = -1,
              int32_t endCheck = -1,
              int32_t systemVersion = -1,
              const char *factoryName = nullptr);

    /**
     * Configure the factory.
     *
     * @param dll_name name/path of shared library.
     * @param factoryName name of factory method, a symbol within the shared
     *                    library.
     *                    If set, it must be the same used when creating the
     *                    plugin.
     * @return true on success.
     */
    bool open(const char *dll_name,
              const char *factoryName = nullptr);

    /**
     * Check if factory is configured and present.
     *
     * @return true iff factory is good to go.
     */
    bool isValid() const;

    /**
     * Get the status of the factory.
     *
     * @return one of the SharedLibraryFactory::STATUS_* codes.
     */
    Status getStatus() const;

    /**
     * Get the latest error of the factory.
     *
     * @return the latest error.
     */
    std::string getError() const;

    /**

     * Get the factory API, which has creation/deletion methods.
     *
     * @return the factory API
     */
    const SharedLibraryClassApi& getApi() const;

    /**
     * Get the current reference count of this factory.
     *
     * @return the current reference count of this factory.
     */
    int getReferenceCount() const;

    /**
     * Increment the reference count of this factory.
     *
     * @return the current reference count of this factory, after increment.
     */
    int addRef();

    /**
     * Decrement the reference count of this factory.
     *
     * @return the current reference count of this factory, after decrement.
     */
    int removeRef();

    /**
     * Get the name associated with this factory.
     *
     * @return the name associated with this factory.
     */
    std::string getName() const;

    /**
     * Get the type associated with this factory.
     *
     * @return the type associated with this factory.
     */
    std::string getClassName() const;

    /**
     * Get the base type associated with this factory.
     *
     * @return the base type associated with this factory.
     */
    std::string getBaseClassName() const;

    /**
     *
     * Specify function to use as factory.
     *
     * @param factory function to use as factory.
     *
     * @result true on success.
     *
     */
    bool useFactoryFunction(void *factory);

#ifndef DOXYGEN_SHOULD_SKIP_THIS
private:
    class SHLIBPP_HIDDEN Private;
    Private* mPriv;
#endif
};

} // namespace shlibpp

#endif // SHAREDLIBPP_SHAREDLIBRARYFACTORY_H
