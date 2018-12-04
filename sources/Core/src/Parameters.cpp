/*
 * Copyright (C) 2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * GNU Lesser General Public License v2.1 or any later version.
 */

#include "BlockFactory/Core/Parameters.h"
#include "BlockFactory/Core/ConvertStdVector.h"
#include "BlockFactory/Core/Log.h"
#include "BlockFactory/Core/Parameter.h"

#include <stddef.h>
#include <unordered_map>
#include <utility>
#include <vector>

using namespace wbt;

// ================
// PARAMETERS::IMPL
// ================

class Parameters::impl
{
public:
    // Typedefs for generic scalar / vector parameters
    using ParameterInt = Parameter<int>;
    using ParameterBool = Parameter<bool>;
    using ParameterDouble = Parameter<double>;
    using ParameterString = Parameter<std::string>;

    // Typedefs for the storage of vector parameters
    using ParamVectorInt = std::vector<int>;
    using ParamVectorBool = std::vector<bool>;
    using ParamVectorDouble = std::vector<double>;
    using ParamVectorString = std::vector<std::string>;

    // Maps for storing parameters and their metadata
    std::unordered_map<ParamName, ParameterInt> paramsInt;
    std::unordered_map<ParamName, ParameterBool> paramsBool;
    std::unordered_map<ParamName, ParameterDouble> paramsDouble;
    std::unordered_map<ParamName, ParameterString> paramsString;

    // Maps for handling the internal indexing
    std::unordered_map<ParamName, wbt::ParameterType> nameToType;
    std::unordered_map<ParamIndex, ParamName> indexToName;
    std::unordered_map<ParamName, ParamIndex> nameToIndex;

    bool existIndex(const ParamIndex& index) const;
    bool existName(const ParamName& name, const wbt::ParameterType& type) const;

    impl* clone() { return new impl(*this); }
};

bool Parameters::impl::existName(const Parameters::ParamName& name,
                                 const wbt::ParameterType& type) const
{
    switch (type) {
        case ParameterType::INT:
        case ParameterType::CELL_INT:
        case ParameterType::STRUCT_INT:
        case ParameterType::STRUCT_CELL_INT:
            if (paramsInt.find(name) == paramsInt.end()) {
                return false;
            }
            break;
        case ParameterType::BOOL:
        case ParameterType::CELL_BOOL:
        case ParameterType::STRUCT_BOOL:
        case ParameterType::STRUCT_CELL_BOOL:
            if (paramsBool.find(name) == paramsBool.end()) {
                return false;
            }
            break;
        case ParameterType::DOUBLE:
        case ParameterType::CELL_DOUBLE:
        case ParameterType::STRUCT_DOUBLE:
        case ParameterType::STRUCT_CELL_DOUBLE:
            if (paramsDouble.find(name) == paramsDouble.end()) {
                return false;
            }
            break;
        case ParameterType::STRING:
        case ParameterType::CELL_STRING:
        case ParameterType::STRUCT_STRING:
        case ParameterType::STRUCT_CELL_STRING:
            if (paramsString.find(name) == paramsString.end()) {
                return false;
            }
            break;
    }
    return true;
}

bool Parameters::impl::existIndex(const Parameters::ParamIndex& index) const
{
    if (indexToName.find(index) == indexToName.end()) {
        return false;
    }

    return true;
}

// ==========
// PARAMETERS
// ==========

Parameters::Parameters()
    : pImpl{new impl()}
{}

// Defining the destructor as default here in the cpp avoids the usage
// of a custom pimpl deleter
Parameters::~Parameters() = default;

Parameters::Parameters(const wbt::Parameters& other)
    : pImpl{other.pImpl->clone()}
{}

wbt::Parameters& Parameters::operator=(const wbt::Parameters& other)
{
    pImpl.reset(other.pImpl->clone());
    return *this;
}

Parameters::ParamName Parameters::getParamName(const Parameters::ParamIndex& index) const
{
    if (pImpl->indexToName.find(index) == pImpl->indexToName.end()) {
        return PARAM_INVALID_NAME;
    }

    return pImpl->indexToName.at(index);
}

Parameters::ParamIndex Parameters::getParamIndex(const Parameters::ParamName& name) const
{
    if (pImpl->nameToIndex.find(name) == pImpl->nameToIndex.end()) {
        return PARAM_INVALID_INDEX;
    }

    return pImpl->nameToIndex.at(name);
}

bool Parameters::existName(const Parameters::ParamName& name) const
{
    if (pImpl->existName(name, ParameterType::INT) || pImpl->existName(name, ParameterType::BOOL)
        || pImpl->existName(name, ParameterType::DOUBLE)
        || pImpl->existName(name, ParameterType::STRING)
        || pImpl->existName(name, ParameterType::STRUCT_INT)
        || pImpl->existName(name, ParameterType::STRUCT_BOOL)
        || pImpl->existName(name, ParameterType::STRUCT_DOUBLE)
        || pImpl->existName(name, ParameterType::STRUCT_STRING)) {
        return true;
    }
    return false;
}

unsigned Parameters::getNumberOfParameters() const
{
    const size_t numIntParams = pImpl->paramsInt.size();
    const size_t numBoolParams = pImpl->paramsBool.size();
    const size_t numDoubleParams = pImpl->paramsDouble.size();
    const size_t numStringParams = pImpl->paramsString.size();

    return numIntParams + numBoolParams + numDoubleParams + numStringParams;
}

std::vector<Parameter<int>> Parameters::getIntParameters() const
{
    std::vector<Parameter<int>> vectorParams;

    for (auto p : pImpl->paramsInt) {
        vectorParams.push_back(p.second);
    }

    return vectorParams;
}

std::vector<Parameter<bool>> Parameters::getBoolParameters() const
{
    std::vector<Parameter<bool>> vectorParams;

    for (auto p : pImpl->paramsBool) {
        vectorParams.push_back(p.second);
    }

    return vectorParams;
}

std::vector<Parameter<double>> Parameters::getDoubleParameters() const
{
    std::vector<Parameter<double>> vectorParams;

    for (auto p : pImpl->paramsDouble) {
        vectorParams.push_back(p.second);
    }

    return vectorParams;
}

std::vector<Parameter<std::string>> Parameters::getStringParameters() const
{
    std::vector<Parameter<std::string>> vectorParams;

    for (auto p : pImpl->paramsString) {
        vectorParams.push_back(p.second);
    }

    return vectorParams;
}

wbt::ParameterMetadata Parameters::getParameterMetadata(const ParamName& name)
{
    if (!existName(name) || !pImpl->existName(name, pImpl->nameToType.at(name))) {
        // TODO: here dummy metadata are returned. This can be improved.
        wbtError << "Failed to get metadata of " << name << " parameter.";
        return {ParameterType::INT, 0, 0, 0, "dummy"};
    }

    switch (pImpl->nameToType[name]) {
        case ParameterType::INT:
        case ParameterType::CELL_INT:
        case ParameterType::STRUCT_INT:
        case ParameterType::STRUCT_CELL_INT:
            return pImpl->paramsInt.at(name).getMetadata();
        case ParameterType::BOOL:
        case ParameterType::CELL_BOOL:
        case ParameterType::STRUCT_BOOL:
        case ParameterType::STRUCT_CELL_BOOL:
            return pImpl->paramsBool.at(name).getMetadata();
        case ParameterType::DOUBLE:
        case ParameterType::CELL_DOUBLE:
        case ParameterType::STRUCT_DOUBLE:
        case ParameterType::STRUCT_CELL_DOUBLE:
            return pImpl->paramsDouble.at(name).getMetadata();
        case ParameterType::STRING:
        case ParameterType::CELL_STRING:
        case ParameterType::STRUCT_STRING:
        case ParameterType::STRUCT_CELL_STRING:
            return pImpl->paramsString.at(name).getMetadata();
    }
}

// =========
// TEMPLATES
// =========

// GETPARAMETER
// ============

// SCALAR
// ------

// Instantiate the declared templates
template bool Parameters::getParameter<int>(const Parameters::ParamName& name, int& param) const;
template bool Parameters::getParameter<bool>(const Parameters::ParamName& name, bool& param) const;
template bool Parameters::getParameter<double>(const Parameters::ParamName& name,
                                               double& param) const;

template <typename T>
bool wbt::Parameters::getParameter(const wbt::Parameters::ParamName& name, T& param) const
{
    if (!existName(name) || !pImpl->existName(name, pImpl->nameToType.at(name))) {
        wbtError << "Trying to get a non existing " << name << " parameter.";
        return false;
    }

    switch (pImpl->nameToType.at(name)) {
        case ParameterType::INT:
        case ParameterType::CELL_INT:
        case ParameterType::STRUCT_INT:
        case ParameterType::STRUCT_CELL_INT:
            if (!pImpl->paramsInt.at(name).isScalar()) {
                wbtError << "Trying to get a scalar from a vector parameter.";
                return false;
            }
            param = static_cast<T>(pImpl->paramsInt.at(name).getScalarParameter());
            break;
        case ParameterType::BOOL:
        case ParameterType::CELL_BOOL:
        case ParameterType::STRUCT_BOOL:
        case ParameterType::STRUCT_CELL_BOOL:
            if (!pImpl->paramsBool.at(name).isScalar()) {
                wbtError << "Trying to get a scalar from a vector parameter.";
                return false;
            }
            param = static_cast<T>(pImpl->paramsBool.at(name).getScalarParameter());
            break;
        case ParameterType::DOUBLE:
        case ParameterType::CELL_DOUBLE:
        case ParameterType::STRUCT_DOUBLE:
        case ParameterType::STRUCT_CELL_DOUBLE:
            if (!pImpl->paramsDouble.at(name).isScalar()) {
                wbtError << "Trying to get a scalar from a vector parameter.";
                return false;
            }
            param = static_cast<T>(pImpl->paramsDouble.at(name).getScalarParameter());
            break;
        case ParameterType::STRING:
        case ParameterType::CELL_STRING:
        case ParameterType::STRUCT_STRING:
        case ParameterType::STRUCT_CELL_STRING:
            if (!pImpl->paramsString.at(name).isScalar()) {
                wbtError << "Trying to get a scalar from a vector parameter.";
                return false;
            }
            param = static_cast<T>(std::stod(pImpl->paramsString.at(name).getScalarParameter()));
            break;
    }
    return true;
}

// VECTOR
// ------

template bool Parameters::getParameter<int>(const Parameters::ParamName& name,
                                            std::vector<int>& param) const;
template bool Parameters::getParameter<bool>(const Parameters::ParamName& name,
                                             std::vector<bool>& param) const;
template bool Parameters::getParameter<double>(const Parameters::ParamName& name,
                                               std::vector<double>& param) const;
template bool Parameters::getParameter<std::string>(const Parameters::ParamName& name,
                                                    std::vector<std::string>& param) const;

template <typename T>
bool wbt::Parameters::getParameter(const wbt::Parameters::ParamName& name,
                                   std::vector<T>& param) const
{
    if (!existName(name) || !pImpl->existName(name, pImpl->nameToType.at(name))) {
        wbtError << "Trying to get a non existing " << name << " parameter.";
        return false;
    }

    param.clear();

    switch (pImpl->nameToType.at(name)) {
        case ParameterType::INT:
        case ParameterType::CELL_INT:
        case ParameterType::STRUCT_INT:
        case ParameterType::STRUCT_CELL_INT: {
            if (pImpl->paramsInt.at(name).isScalar()) {
                wbtError << "Trying to get a vector from a scalar parameter.";
                return false;
            }
            std::vector<T> output;
            convertStdVector(pImpl->paramsInt.at(name).getVectorParameter(), param);
            break;
        }
        case ParameterType::BOOL:
        case ParameterType::CELL_BOOL:
        case ParameterType::STRUCT_BOOL:
        case ParameterType::STRUCT_CELL_BOOL: {
            if (pImpl->paramsBool.at(name).isScalar()) {
                wbtError << "Trying to get a vector from a scalar parameter.";
                return false;
            }
            std::vector<T> output;
            convertStdVector(pImpl->paramsBool.at(name).getVectorParameter(), param);
            break;
        }
        case ParameterType::DOUBLE:
        case ParameterType::CELL_DOUBLE:
        case ParameterType::STRUCT_DOUBLE:
        case ParameterType::STRUCT_CELL_DOUBLE: {
            if (pImpl->paramsDouble.at(name).isScalar()) {
                wbtError << "Trying to get a vector from a scalar parameter.";
                return false;
            }
            std::vector<T> output;
            convertStdVector(pImpl->paramsDouble.at(name).getVectorParameter(), param);
            break;
        }
        case ParameterType::STRING:
        case ParameterType::CELL_STRING:
        case ParameterType::STRUCT_STRING:
        case ParameterType::STRUCT_CELL_STRING: {
            if (pImpl->paramsString.at(name).isScalar()) {
                wbtError << "Trying to get a vector from a scalar parameter.";
                return false;
            }
            std::vector<T> output;
            convertStdVector(pImpl->paramsString.at(name).getVectorParameter(), param);
            break;
        }
    }
    return true;
}

// STOREPARAMETER
// ============

// SCALAR
// ------

template bool Parameters::storeParameter<int>(const int& param,
                                              const ParameterMetadata& paramMetadata);
template bool Parameters::storeParameter<bool>(const bool& param,
                                               const ParameterMetadata& paramMetadata);
template bool Parameters::storeParameter<double>(const double& param,
                                                 const ParameterMetadata& paramMetadata);

template <typename T>
bool wbt::Parameters::storeParameter(const T& param, const wbt::ParameterMetadata& paramMetadata)
{
    if (existName(paramMetadata.name) || pImpl->existName(paramMetadata.name, paramMetadata.type)) {
        wbtError << "Trying to store an already existing " << paramMetadata.name << " parameter.";
        return false;
    }

    if (paramMetadata.rows != 1 && paramMetadata.cols != 1) {
        wbtError << "2D parameters are not supported.";
        return false;
    }

    switch (paramMetadata.type) {
        case ParameterType::INT:
        case ParameterType::CELL_INT:
        case ParameterType::STRUCT_INT:
        case ParameterType::STRUCT_CELL_INT:
            pImpl->paramsInt.emplace(std::make_pair(
                paramMetadata.name, impl::ParameterInt(static_cast<int>(param), paramMetadata)));
            break;
        case ParameterType::BOOL:
        case ParameterType::CELL_BOOL:
        case ParameterType::STRUCT_BOOL:
        case ParameterType::STRUCT_CELL_BOOL:
            pImpl->paramsBool.emplace(std::make_pair(
                paramMetadata.name, impl::ParameterBool(static_cast<bool>(param), paramMetadata)));
            break;
        case ParameterType::DOUBLE:
        case ParameterType::CELL_DOUBLE:
        case ParameterType::STRUCT_DOUBLE:
        case ParameterType::STRUCT_CELL_DOUBLE:
            pImpl->paramsDouble.emplace(
                std::make_pair(paramMetadata.name,
                               impl::ParameterDouble(static_cast<double>(param), paramMetadata)));
            break;
        case ParameterType::STRING:
        case ParameterType::CELL_STRING:
        case ParameterType::STRUCT_STRING:
        case ParameterType::STRUCT_CELL_STRING:
            pImpl->paramsString.emplace(std::make_pair(
                paramMetadata.name, impl::ParameterString(std::to_string(param), paramMetadata)));
            break;
    }

    pImpl->nameToType[paramMetadata.name] = paramMetadata.type;
    pImpl->nameToIndex[paramMetadata.name] = paramMetadata.index;
    pImpl->indexToName[paramMetadata.index] = paramMetadata.name;

    return true;
}

// VECTOR
// ------

template bool Parameters::storeParameter<int>(const std::vector<int>& param,
                                              const ParameterMetadata& paramMetadata);
template bool Parameters::storeParameter<bool>(const std::vector<bool>& param,
                                               const ParameterMetadata& paramMetadata);
template bool Parameters::storeParameter<double>(const std::vector<double>& param,
                                                 const ParameterMetadata& paramMetadata);
template bool Parameters::storeParameter<std::string>(const std::vector<std::string>& param,
                                                      const ParameterMetadata& paramMetadata);

template <typename T>
bool wbt::Parameters::storeParameter(const std::vector<T>& param,
                                     const wbt::ParameterMetadata& paramMetadata)
{
    if (existName(paramMetadata.name) || pImpl->existName(paramMetadata.name, paramMetadata.type)) {
        wbtError << "Trying to store an already existing " << paramMetadata.name << " parameter.";
        return false;
    }

    if (paramMetadata.rows != 1 && paramMetadata.cols != param.size()) {
        wbtError << "2D parameters are not supported.";
        return false;
    }

    switch (paramMetadata.type) {
        case ParameterType::INT:
        case ParameterType::CELL_INT:
        case ParameterType::STRUCT_INT:
        case ParameterType::STRUCT_CELL_INT: {
            std::vector<int> paramInt(param.size());
            convertStdVector<T, int>(param, paramInt);
            pImpl->paramsInt.emplace(
                std::make_pair(paramMetadata.name, impl::ParameterInt(paramInt, paramMetadata)));
            break;
        }
        case ParameterType::BOOL:
        case ParameterType::CELL_BOOL:
        case ParameterType::STRUCT_BOOL:
        case ParameterType::STRUCT_CELL_BOOL: {
            std::vector<bool> paramBool(param.size());
            convertStdVector<T, bool>(param, paramBool);
            pImpl->paramsBool.emplace(
                std::make_pair(paramMetadata.name, impl::ParameterBool(paramBool, paramMetadata)));
            break;
        }
        case ParameterType::DOUBLE:
        case ParameterType::CELL_DOUBLE:
        case ParameterType::STRUCT_DOUBLE:
        case ParameterType::STRUCT_CELL_DOUBLE: {
            std::vector<double> paramDouble(param.size());
            convertStdVector<T, double>(param, paramDouble);
            pImpl->paramsDouble.emplace(std::make_pair(
                paramMetadata.name, impl::ParameterDouble(paramDouble, paramMetadata)));
            break;
        }
        case ParameterType::STRING:
        case ParameterType::CELL_STRING:
        case ParameterType::STRUCT_STRING:
        case ParameterType::STRUCT_CELL_STRING: {
            std::vector<std::string> paramString(param.size());
            convertStdVector<T, std::string>(param, paramString);
            pImpl->paramsString.emplace(std::make_pair(
                paramMetadata.name, impl::ParameterString(paramString, paramMetadata)));
            break;
        }
    }

    pImpl->nameToType[paramMetadata.name] = paramMetadata.type;
    pImpl->nameToIndex[paramMetadata.name] = paramMetadata.index;
    pImpl->indexToName[paramMetadata.index] = paramMetadata.name;

    return true;
}

// PARAMETER
// ---------

template bool Parameters::storeParameter<int>(const Parameter<int>& parameter);
template bool Parameters::storeParameter<bool>(const Parameter<bool>& parameter);
template bool Parameters::storeParameter<double>(const Parameter<double>& parameter);
template bool Parameters::storeParameter<std::string>(const Parameter<std::string>& parameter);

template <typename T>
bool wbt::Parameters::storeParameter(const wbt::Parameter<T>& parameter)
{
    if (existName(parameter.getMetadata().name)) {
        wbtError << "Trying to store an already existing " << parameter.getMetadata().name
                 << " parameter.";
        return false;
    }

    if (parameter.isScalar()) {
        return storeParameter(parameter.getScalarParameter(), parameter.getMetadata());
    }
    else {
        return storeParameter(parameter.getVectorParameter(), parameter.getMetadata());
    }
}

// TEMPLATE SPECIALIZATIONS
// ========================

template <>
bool Parameters::getParameter<std::string>(const ParamName& name, std::string& param) const
{
    if (!existName(name) || !pImpl->existName(name, pImpl->nameToType.at(name))) {
        wbtError << "Trying to get a non existing " << name << " parameter.";
        return false;
    }

    switch (pImpl->nameToType.at(name)) {
        case ParameterType::INT:
        case ParameterType::CELL_INT:
        case ParameterType::STRUCT_INT:
        case ParameterType::STRUCT_CELL_INT:
            if (!pImpl->paramsInt.at(name).isScalar()) {
                wbtError << "Trying to get a scalar from a vector parameter.";
                return false;
            }
            param = std::to_string(pImpl->paramsInt.at(name).getScalarParameter());
            break;
        case ParameterType::BOOL:
        case ParameterType::CELL_BOOL:
        case ParameterType::STRUCT_BOOL:
        case ParameterType::STRUCT_CELL_BOOL:
            if (!pImpl->paramsBool.at(name).isScalar()) {
                wbtError << "Trying to get a scalar from a vector parameter.";
                return false;
            }
            param = std::to_string(pImpl->paramsBool.at(name).getScalarParameter());
            break;
        case ParameterType::DOUBLE:
        case ParameterType::CELL_DOUBLE:
        case ParameterType::STRUCT_DOUBLE:
        case ParameterType::STRUCT_CELL_DOUBLE:
            if (!pImpl->paramsDouble.at(name).isScalar()) {
                wbtError << "Trying to get a scalar from a vector parameter.";
                return false;
            }
            param = std::to_string(pImpl->paramsDouble.at(name).getScalarParameter());
            break;
        case ParameterType::STRING:
        case ParameterType::CELL_STRING:
        case ParameterType::STRUCT_STRING:
        case ParameterType::STRUCT_CELL_STRING:
            if (!pImpl->paramsString.at(name).isScalar()) {
                wbtError << "Trying to get a scalar from a vector parameter.";
                return false;
            }
            param = pImpl->paramsString.at(name).getScalarParameter();
            break;
    }
    return true;
}

template <>
bool wbt::Parameters::storeParameter<std::string>(const std::string& param,
                                                  const wbt::ParameterMetadata& paramMetadata)
{
    if (existName(paramMetadata.name) || pImpl->existName(paramMetadata.name, paramMetadata.type)) {
        wbtError << "Trying to store an already existing " << paramMetadata.name << " parameter.";
        return false;
    }

    if (paramMetadata.rows != 1 && paramMetadata.cols != 1) {
        wbtError << "2D parameters are not supported.";
        return false;
    }

    switch (paramMetadata.type) {
        case ParameterType::INT:
        case ParameterType::CELL_INT:
        case ParameterType::STRUCT_INT:
        case ParameterType::STRUCT_CELL_INT:
            pImpl->paramsInt.emplace(std::make_pair(
                paramMetadata.name, impl::ParameterInt(std::stoi(param), paramMetadata)));
            break;
        case ParameterType::BOOL:
        case ParameterType::CELL_BOOL:
        case ParameterType::STRUCT_BOOL:
        case ParameterType::STRUCT_CELL_BOOL:
            pImpl->paramsBool.emplace(std::make_pair(
                paramMetadata.name,
                impl::ParameterBool(static_cast<bool>(std::stoi(param)), paramMetadata)));
            break;
        case ParameterType::DOUBLE:
        case ParameterType::CELL_DOUBLE:
        case ParameterType::STRUCT_DOUBLE:
        case ParameterType::STRUCT_CELL_DOUBLE:
            pImpl->paramsDouble.emplace(std::make_pair(
                paramMetadata.name, impl::ParameterDouble(std::stod(param), paramMetadata)));
            break;
        case ParameterType::STRING:
        case ParameterType::CELL_STRING:
        case ParameterType::STRUCT_STRING:
        case ParameterType::STRUCT_CELL_STRING:
            pImpl->paramsString.emplace(
                std::make_pair(paramMetadata.name, impl::ParameterString(param, paramMetadata)));
            break;
    }

    pImpl->nameToType[paramMetadata.name] = paramMetadata.type;
    pImpl->nameToIndex[paramMetadata.name] = paramMetadata.index;
    pImpl->indexToName[paramMetadata.index] = paramMetadata.name;

    return true;
}
