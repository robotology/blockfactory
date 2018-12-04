/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef SHAREDLIBPP_SHAREDLIBRARYCLASSFACTORY_INL_H
#define SHAREDLIBPP_SHAREDLIBRARYCLASSFACTORY_INL_H

template <typename T>
shlibpp::SharedLibraryClassFactory<T>::SharedLibraryClassFactory(int32_t startCheck,
                                                                 int32_t endCheck,
                                                                 int32_t systemVersion,
                                                                 const char *factoryName) :
        SharedLibraryFactory(startCheck, endCheck, systemVersion, factoryName)
{
}

template <typename T>
shlibpp::SharedLibraryClassFactory<T>::SharedLibraryClassFactory(const char *dll_name,
                                                                 int32_t startCheck,
                                                                 int32_t endCheck,
                                                                 int32_t systemVersion,
                                                                 const char *factoryName) :
        SharedLibraryFactory(dll_name, startCheck, endCheck, systemVersion, factoryName)
{
}

template <typename T>
shlibpp::SharedLibraryClassFactory<T>::SharedLibraryClassFactory(const char *dll_name,
                                                                 const char *factoryName) :
        SharedLibraryFactory(dll_name, factoryName)
{
}

template <typename T>
T* shlibpp::SharedLibraryClassFactory<T>::create() const
{
    if (!isValid()) {
        return nullptr;
    }
    return static_cast<T*>(getApi().create());
}

template <typename T>
void shlibpp::SharedLibraryClassFactory<T>::destroy(T *obj) const
{
    if (!isValid()) {
        return;
    }
    getApi().destroy(obj);
}

#endif // SHAREDLIBPP_SHAREDLIBRARYCLASSFACTORY_INL_H
