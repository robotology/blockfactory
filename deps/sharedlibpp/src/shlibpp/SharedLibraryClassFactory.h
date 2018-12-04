/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef SHAREDLIBPP_SHAREDLIBRARYCLASSFACTORY_H
#define SHAREDLIBPP_SHAREDLIBRARYCLASSFACTORY_H

#include <shlibpp/SharedLibraryFactory.h>
#include <shlibpp/SharedLibraryClassApi.h>

namespace shlibpp {

/**
 * A type-safe wrapper for SharedLibraryFactory, committing to
 * creation/destruction of instances of a particular super-class.
 *
 * Note that we take on faith that the named factory method in the
 * named shared library does in fact create the named type.
 */
template <class T>
class SharedLibraryClassFactory : public SharedLibraryFactory
{
public:
    explicit SharedLibraryClassFactory(int32_t startCheck = SHLIBPP_DEFAULT_START_CHECK,
                                       int32_t endCheck = SHLIBPP_DEFAULT_END_CHECK,
                                       int32_t systemVersion = SHLIBPP_DEFAULT_SYSTEM_VERSION,
                                       const char *factoryName = nullptr);

    explicit SharedLibraryClassFactory(const char *dll_name,
                                       int32_t startCheck = SHLIBPP_DEFAULT_START_CHECK,
                                       int32_t endCheck = SHLIBPP_DEFAULT_END_CHECK,
                                       int32_t systemVersion = SHLIBPP_DEFAULT_SYSTEM_VERSION,
                                       const char *factoryName = nullptr);

    explicit SharedLibraryClassFactory(const char *dll_name,
                                       const char *factoryName = nullptr);

    T *create() const;

    void destroy(T *obj) const;
};

} // namespace shlibpp

#include <shlibpp/SharedLibraryClassFactory-inl.h>

#endif // SHAREDLIBPP_SHAREDLIBRARYCLASSFACTORY_H
