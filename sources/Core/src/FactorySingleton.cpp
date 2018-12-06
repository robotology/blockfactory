#include "BlockFactory/Core/FactorySingleton.h"
#include <map>

using namespace blockfactory::core;
std::string platformSpecificLibName(const std::string& library);

class ClassFactorySingleton::Impl
{
public:
    std::map<const ClassFactoryData, ClassFactoryPtr> factoryMap;
};

ClassFactorySingleton::ClassFactorySingleton()
    : pImpl(std::make_unique<Impl>())
{}

ClassFactorySingleton& ClassFactorySingleton::getInstance()
{
    static ClassFactorySingleton instance;
    return instance;
}

ClassFactorySingleton::ClassFactoryPtr
ClassFactorySingleton::getClassFactory(const ClassFactoryData& factorydata)
{
    const ClassFactoryLibrary& libraryName = factorydata.first;
    const ClassFactoryName& factoryName = factorydata.second;

    std::string fsLibraryName = platformSpecificLibName(libraryName);

    // Clean possible leftovers
    if (pImpl->factoryMap.find(factorydata) != pImpl->factoryMap.end()
        && !pImpl->factoryMap[factorydata]) {
        pImpl->factoryMap.erase(factorydata);
    }

    // Lazy initialization of the factory that loads the dll
    if (pImpl->factoryMap.find(factorydata) == pImpl->factoryMap.end()) {

        // Allocate the factory
        auto factory = std::make_shared<ClassFactory>(fsLibraryName.c_str(), factoryName.c_str());

        if (!factory || !factory->isValid()) {
            bfError << "Failed to create factory";
            return {};
        }

        // Store it in the map
        pImpl->factoryMap.insert({factorydata, factory});
    }

    if (!pImpl->factoryMap[factorydata]->isValid()) {
        bfError << "The factory " << factoryName << " associated with the plugin " << fsLibraryName
                << " is not valid";
        return {};
    }

    // Return the block factory
    return pImpl->factoryMap[factorydata];
}

std::string platformSpecificLibName(const std::string& library)
{
#if defined(_WIN32)
    return library + "dll";
#elif defined(__linux__)
    return "lib" + library + ".so";
#elif defined(__APPLE__)
    return "lib" + library + ".dylib";
#endif
}
