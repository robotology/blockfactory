/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <shlibpp/SharedLibraryFactory.h>
#include <shlibpp/SharedLibraryClassApi.h>
#include <shlibpp/SharedLibrary.h>

#include <cstdlib>
#include <fstream>
#include <string>
#include <sstream>
#include <sys/stat.h>
#include <vector>

#if defined(_WIN32)
# define shlibpp_struct_stat struct _stat
# define shlibpp_stat ::_stat
#else
# define shlibpp_struct_stat struct ::stat
# define shlibpp_stat ::stat
#endif

#if defined(WIN32) || defined(_WIN32)
#define PATH_SEPARATOR "\\"
#else
#define PATH_SEPARATOR "/"
#endif

class shlibpp::SharedLibraryFactory::Private
{
public:
    Private(int32_t startCheck,
            int32_t endCheck,
            int32_t systemVersion,
            const char* factoryName);

    bool open(const char* dll_name);
    bool isValid() const;
    bool useFactoryFunction(void *factory);

    void extendSearchPath(const std::string& path);
    void readExtendedPathFromEnvironment();
    std::string findLibraryInExtendedPath(const std::string& libraryName);
    static std::vector<std::string> platformSpecificLibName(const std::string& library);

    SharedLibrary lib;
    SharedLibraryFactory::Status status;
    SharedLibraryClassApi api;
    int returnValue;
    int rct; // FIXME Remove this reference counter and use a shared_ptr instead.
    std::string name;
    std::string className;
    std::string baseClassName;
    std::string error;

    int32_t startCheck;
    int32_t endCheck;
    int32_t systemVersion;
    const char* factoryName;

    std::vector<std::string> extendedPath;
    std::string pluginPathEnvVar = "SHLIBPP_PLUGIN_PATH";
};



shlibpp::SharedLibraryFactory::Private::Private(int32_t startCheck,
                                                int32_t endCheck,
                                                int32_t systemVersion,
                                                const char* factoryName) :
        status(Status::None),
        returnValue(0),
        rct(1),
        startCheck(startCheck),
        endCheck(endCheck),
        systemVersion(systemVersion),
        factoryName(factoryName)
{
    memset(&api, 0, sizeof(SharedLibraryClassApi));
}

std::vector<std::string> shlibpp::SharedLibraryFactory::Private::platformSpecificLibName(const std::string& library)
{

#if defined(_WIN32)
#if defined(NDEBUG)
    return {library + ".dll", library + "d.dll", "lib" + library + ".dll"};
#else
    return {library + "d.dll", library + ".dll", "lib" + library + ".dll"};
#endif
#elif defined(__linux__)
    return {"lib" + library + ".so"};
#elif defined(__APPLE__)
    return {"lib" + library + ".dylib"};
#else
#error "This platform not supported by this project"
#endif
}

std::string shlibpp::SharedLibraryFactory::Private::findLibraryInExtendedPath(const std::string& libraryName)
{
    std::size_t found = libraryName.find_first_of("\\/");
    if (found != std::string::npos) {
        return {};
    }

    for (const auto& path: extendedPath) {
        for (const auto& osLibName : platformSpecificLibName(libraryName)){
            std::string absolutePath = path + PATH_SEPARATOR + osLibName;

            if (std::ifstream(absolutePath)) {
                return absolutePath;
            }
        }
    }

    return {};
}

bool shlibpp::SharedLibraryFactory::Private::open(const char* dll_name)
{
    returnValue = 0;
    name = "";
    className = "";
    baseClassName = "";
    status = Status::None;
    error = "";
    api.startCheck = 0;

    readExtendedPathFromEnvironment();
    std::string pathToLib = findLibraryInExtendedPath(dll_name);

    if (pathToLib.empty()) {
        pathToLib = dll_name;
    }

    if (!lib.open(pathToLib.c_str())) {
        shlibpp_struct_stat dummy;
        if (shlibpp_stat(pathToLib.c_str(), &dummy) != 0) {
            status = Status::LibraryNotFound;
        } else {
            status = Status::LibraryNotLoaded;
        }
        error = lib.error();
        return false;
    }
    void *fn = lib.getSymbol((factoryName != nullptr) ? factoryName : SHLIBPP_DEFAULT_FACTORY_NAME_STRING);
    if (fn == nullptr) {
        status = Status::FactoryNotFound;
        error = lib.error();
        lib.close();
        return false;
    }
    if (!useFactoryFunction(fn)) {
        status = Status::FactoryNotFunctional;
        error = "Hook in shared library misbehaved";
        return false;
    }
    status = Status::OK;
    name = dll_name;

    char buf[256];
    api.getClassName(buf, 256);
    className = buf;
    api.getBaseClassName(buf, 256);
    baseClassName = buf;

    return true;
}

bool shlibpp::SharedLibraryFactory::Private::isValid() const
{
    if (returnValue != startCheck) {
        return false;
    }
    if (api.startCheck != startCheck) {
        return false;
    }
    if (api.structureSize != sizeof(SharedLibraryClassApi)) {
        return false;
    }
    if (api.systemVersion != systemVersion) {
        return false;
    }
    if (api.endCheck != endCheck) {
        return false;
    }
    return true;
}

bool shlibpp::SharedLibraryFactory::Private::useFactoryFunction(void *factory)
{
    api.startCheck = 0;
    if (factory == nullptr) {
        return false;
    }
    returnValue =
        ((int (*)(void *ptr,int len)) factory)(&api,sizeof(SharedLibraryClassApi));
    return isValid();
}

void shlibpp::SharedLibraryFactory::Private::extendSearchPath(const std::string& path)
{
    std::string pathToAdd = path;

    if (pathToAdd.back() == '/' || pathToAdd.back() == '\\') {
        pathToAdd.pop_back();
    }

    for (const auto& storedPath : extendedPath) {
        if (storedPath == pathToAdd) {
            return;
        }
    }

    extendedPath.push_back(pathToAdd);
}

void shlibpp::SharedLibraryFactory::Private::readExtendedPathFromEnvironment()
{
    std::string path;
    auto content = std::getenv(pluginPathEnvVar.c_str());

    if (!content) {
        return;
    }

    std::stringstream envStream(content);

#if defined(_WIN32)
    char delim = ';';
#else
    char delim = ':';
#endif

    while (getline(envStream, path, delim)) {
        extendSearchPath(path);
    }
}

shlibpp::SharedLibraryFactory::SharedLibraryFactory(int32_t startCheck,
                                                    int32_t endCheck,
                                                    int32_t systemVersion,
                                                    const char *factoryName) :
        mPriv(new Private(startCheck, endCheck, systemVersion, factoryName))
{
}

shlibpp::SharedLibraryFactory::SharedLibraryFactory(const char *dll_name,
                                                    int32_t startCheck,
                                                    int32_t endCheck,
                                                    int32_t systemVersion,
                                                    const char *factoryName) :
        SharedLibraryFactory(startCheck, endCheck, systemVersion, factoryName)
{
    mPriv->open(dll_name);
}

shlibpp::SharedLibraryFactory::SharedLibraryFactory(const char* dll_name,
                                                    const char* factoryName) :
        SharedLibraryFactory(SHLIBPP_DEFAULT_START_CHECK,
                             SHLIBPP_DEFAULT_END_CHECK,
                             SHLIBPP_DEFAULT_SYSTEM_VERSION,
                             factoryName)
{
    mPriv->open(dll_name);
}

shlibpp::SharedLibraryFactory::~SharedLibraryFactory()
{
    delete mPriv;
}

bool shlibpp::SharedLibraryFactory::open(const char *dll_name,
                                         int32_t startCheck,
                                         int32_t endCheck,
                                         int32_t systemVersion,
                                         const char *factoryName)
{
    mPriv->startCheck = startCheck;
    mPriv->endCheck = endCheck;
    mPriv->systemVersion = systemVersion;
    mPriv->factoryName = factoryName;
    return mPriv->open(dll_name);
}

bool shlibpp::SharedLibraryFactory::open(const char* dll_name, const char* factoryName)
{
    mPriv->startCheck = SHLIBPP_DEFAULT_START_CHECK;
    mPriv->endCheck = SHLIBPP_DEFAULT_END_CHECK;
    mPriv->systemVersion = SHLIBPP_DEFAULT_SYSTEM_VERSION;
    mPriv->factoryName = factoryName;
    return mPriv->open(dll_name);
}

void shlibpp::SharedLibraryFactory::setPluginPathEnvVarName(const std::string &env_var)
{
    mPriv->pluginPathEnvVar = env_var;
}

void shlibpp::SharedLibraryFactory::extendSearchPath(const std::string& path)
{
    mPriv->extendSearchPath(path);
}

bool shlibpp::SharedLibraryFactory::isValid() const
{
    return mPriv->isValid();
}

shlibpp::SharedLibraryFactory::Status shlibpp::SharedLibraryFactory::getStatus() const
{
    return mPriv->status;
}

std::string shlibpp::SharedLibraryFactory::getError() const
{
    return mPriv->error;
}

const shlibpp::SharedLibraryClassApi& shlibpp::SharedLibraryFactory::getApi() const
{
    return mPriv->api;
}

int shlibpp::SharedLibraryFactory::getReferenceCount() const
{
    return mPriv->rct;
}

int shlibpp::SharedLibraryFactory::addRef()
{
    mPriv->rct++;
    return mPriv->rct;
}

int shlibpp::SharedLibraryFactory::removeRef()
{
    mPriv->rct--;
    return mPriv->rct;
}

std::string shlibpp::SharedLibraryFactory::getName() const
{
    return mPriv->name;
}

std::string shlibpp::SharedLibraryFactory::getClassName() const
{
    return mPriv->className;
}

std::string shlibpp::SharedLibraryFactory::getBaseClassName() const
{
    return mPriv->baseClassName;
}

bool shlibpp::SharedLibraryFactory::useFactoryFunction(void *factory)
{
    return mPriv->useFactoryFunction(factory);
}
