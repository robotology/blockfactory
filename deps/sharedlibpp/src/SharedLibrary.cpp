/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <cstddef>

#if defined(_WIN32)
#  include <windows.h>
#else
#  include <dlfcn.h>
#endif

#include <shlibpp/SharedLibrary.h>

using namespace shlibpp;


class SharedLibrary::Private
{
public:
    Private() :
            implementation(nullptr),
            err_message()
    {
    }

    void* implementation;
    std::string err_message;
};

SharedLibrary::SharedLibrary() :
    mPriv(new Private)
{
}

SharedLibrary::SharedLibrary(const char *filename) :
    SharedLibrary()
{
    open(filename);
}

SharedLibrary::~SharedLibrary()
{
    close();
    delete mPriv;
}

bool SharedLibrary::open(const char *filename)
{
    mPriv->err_message.clear();
    close();
#if defined(_WIN32)
    mPriv->implementation = (void*)LoadLibrary(filename);
    LPTSTR msg = nullptr;
    FormatMessage(
       FORMAT_MESSAGE_FROM_SYSTEM |FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS,
       nullptr, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
       (LPTSTR)&msg, 0, nullptr);

    if(msg != nullptr) {
        mPriv->err_message = std::string(msg);
       // release memory allocated by FormatMessage()
       LocalFree(msg); msg = nullptr;
    }
    return (mPriv->implementation != nullptr);
#else
    mPriv->implementation = dlopen(filename, RTLD_LAZY);
    char* msg = dlerror();
    if(msg)
        mPriv->err_message = msg;
    return mPriv->implementation != nullptr;
#endif
}

bool SharedLibrary::close() {
    int result = 0;
    if (mPriv->implementation != nullptr) {
#if defined(WIN32)
        result = FreeLibrary((HINSTANCE)mPriv->implementation);
        LPTSTR msg = nullptr;
        FormatMessage(
           FORMAT_MESSAGE_FROM_SYSTEM |FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS,
           nullptr, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
           (LPTSTR)&msg, 0, nullptr);

        if(msg != nullptr) {
            mPriv->err_message = std::string(msg);
            // release memory allocated by FormatMessage()
            LocalFree(msg); msg = nullptr;
        }
#else
        result = dlclose(mPriv->implementation);
        if (result != 0) {
        char* msg = dlerror();
        if(msg)
            mPriv->err_message = msg;
        }
#endif
        mPriv->implementation = nullptr;

    }
    return (result == 0);
}

std::string SharedLibrary::error()
{
    return mPriv->err_message;
}

void *SharedLibrary::getSymbol(const char *symbolName) {
    mPriv->err_message.clear();
    if (mPriv->implementation==nullptr) return nullptr;
#if defined(_WIN32)
    FARPROC proc = GetProcAddress((HINSTANCE)mPriv->implementation, symbolName);
    LPTSTR msg = nullptr;
    FormatMessage(
       FORMAT_MESSAGE_FROM_SYSTEM |FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS,
       nullptr, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
       (LPTSTR)&msg, 0, nullptr);

    if(msg != nullptr) {
        mPriv->err_message = std::string(msg);
       // release memory allocated by FormatMessage()
       LocalFree(msg); msg = nullptr;
    }
    return (void*)proc;
#else
    dlerror();
    void* func = dlsym(mPriv->implementation,symbolName);
    char* msg = dlerror();
    if(msg)
        mPriv->err_message = msg;
    return func;
#endif
}

bool SharedLibrary::isValid() const {
    return mPriv->implementation != nullptr;
}
