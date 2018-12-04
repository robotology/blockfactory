/*
 * Copyright (C) 2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * GNU Lesser General Public License v2.1 or any later version.
 */

#ifndef WBT_PARAMETERS_H
#define WBT_PARAMETERS_H

#include <memory>
#include <string>
#include <vector>

namespace wbt {
    template <typename T>
    class Parameter;
    class ParameterMetadata;
    class Parameters;
    const int PARAM_INVALID_INDEX = -1;
    const std::string PARAM_INVALID_NAME = {};
} // namespace wbt

/**
 * @brief Class for storing block's parameters
 *
 * This class can contain scalar and vector parameters of the supported types.
 *
 * @see wbt::Parameter, wbt::ParameterMetadata, wbt::ParameterType
 */
class wbt::Parameters
{
public:
    using ParamIndex = int;
    using ParamName = std::string;

private:
    class impl;
    std::unique_ptr<impl> pImpl;

public:
    Parameters();
    Parameters(const wbt::Parameters& other);
    Parameters& operator=(const Parameters& other);

    ~Parameters();

    /**
     * @brief Get the name of a stored parameter from its index.
     * @param index The index of the parameter.
     * @return The name if the parameter exists, wbt::PARAM_INVALID_NAME otherwise.
     */
    ParamName getParamName(const ParamIndex& index) const;

    /**
     * @brief Get the index of a stored parameter from its name.
     * @param name The name of the parameter.
     * @return The index if the parameter exists, wbt::PARAM_INVALID_INDEX otherwise.
     */
    ParamIndex getParamIndex(const ParamName& name) const;

    /**
     * @brief Get the number of stored parameters
     *
     * @return The number of stored parameters.
     */
    unsigned getNumberOfParameters() const;

    /**
     * @brief Check if a parameter with a given name is stored
     *
     * @param name The name of the parameter.
     * @return True if the parameters exists, false otherwise.
     */
    bool existName(const ParamName& name) const;

    /**
     * @brief Store a scalar parameter
     *
     * @tparam The type of the parameter to store. Despite this, the parameter get cast accordingly
     *         to its metadata.
     * @param param The value of the scalar parameter to store.
     * @param paramMetadata The metadata associated to the parameter to store.
     * @return True for success, false otherwise.
     */
    template <typename T>
    bool storeParameter(const T& param, const wbt::ParameterMetadata& paramMetadata);

    /**
     * @brief Store a vector parameter
     *
     * @tparam The type of the parameter to store. Despite this, the parameter get cast accordingly
     *         to its metadata.
     * @param param The value of the vector parameter to store.
     * @param paramMetadata The metadata associated to the parameter to store.
     * @return True for success, false otherwise.
     */
    template <typename T>
    bool storeParameter(const std::vector<T>& param, const wbt::ParameterMetadata& paramMetadata);

    /**
     * @brief Store a parameter
     *
     * @tparam The type of the parameter to store.
     * @param parameter The parameter object to store.
     * @return True for success, false otherwise.
     *
     * @see wbt::Parameter
     */
    template <typename T>
    bool storeParameter(const Parameter<T>& parameter);

    /**
     * @brief Get a scalar parameter
     *
     * @tparam The type of the output argument
     * @param name The name of the parameter.
     * @param[out] param The variable where the parameter value will be stored. Data get cast
     *                   internally, even for string to numeric types.
     * @return True for success, false otherwise.
     */
    template <typename T>
    bool getParameter(const ParamName& name, T& param) const;

    /**
     * @brief Get a vector parameter
     *
     * @tparam The type of the output argument
     * @param name The name of the parameter.
     * @param[out] param The variable where the parameter value will be stored. Data get cast
     *                   internally, even for string to numeric types.
     * @return True for success, false otherwise.
     */
    template <typename T>
    bool getParameter(const ParamName& name, std::vector<T>& param) const;

    /**
     * @brief Get all the integer parameters
     *
     * @return The integer parameters
     */
    std::vector<Parameter<int>> getIntParameters() const;

    /**
     * @brief Get all the boolean parameters
     *
     * @return The boolean parameters
     */
    std::vector<Parameter<bool>> getBoolParameters() const;

    /**
     * @brief Get all the double parameters
     *
     * @return The double parameters
     */
    std::vector<Parameter<double>> getDoubleParameters() const;

    /**
     * @brief Get all the string parameters
     *
     * @return The string parameters
     */
    std::vector<Parameter<std::string>> getStringParameters() const;

    /**
     * @brief Get the metadata associated to a stored parameter
     *
     * @param name The name of the parameter.
     * @return The metadata associate with the parameter for success, a metadata with a stored name
     *         `dummy` otherwise.
     */
    wbt::ParameterMetadata getParameterMetadata(const ParamName& name);
};

// ============
// GETPARAMETER
// ============

// SCALAR
namespace wbt {
    // Explicit declaration for numeric types
    extern template bool Parameters::getParameter<int>(const Parameters::ParamName& name,
                                                       int& param) const;
    extern template bool Parameters::getParameter<bool>(const Parameters::ParamName& name,
                                                        bool& param) const;
    extern template bool Parameters::getParameter<double>(const Parameters::ParamName& name,
                                                          double& param) const;
    // Explicit specialization for std::string
    template <>
    bool Parameters::getParameter<std::string>(const Parameters::ParamName& name,
                                               std::string& param) const;
} // namespace wbt

// VECTOR
namespace wbt {
    // Explicit declaration for numeric types
    extern template bool Parameters::getParameter<int>(const Parameters::ParamName& name,
                                                       std::vector<int>& param) const;
    extern template bool Parameters::getParameter<bool>(const Parameters::ParamName& name,
                                                        std::vector<bool>& param) const;
    extern template bool Parameters::getParameter<double>(const Parameters::ParamName& name,
                                                          std::vector<double>& param) const;
    extern template bool
    Parameters::getParameter<std::string>(const Parameters::ParamName& name,
                                          std::vector<std::string>& param) const;
} // namespace wbt

// ==============
// STOREPARAMETER
// ==============

// SCALAR
namespace wbt {
    // Explicit declaration for numeric types
    extern template bool Parameters::storeParameter<int>(const int& param,
                                                         const ParameterMetadata& paramMetadata);
    extern template bool Parameters::storeParameter<bool>(const bool& param,
                                                          const ParameterMetadata& paramMetadata);
    extern template bool Parameters::storeParameter<double>(const double& param,
                                                            const ParameterMetadata& paramMetadata);
    // Explicit specialization for std::string
    template <>
    bool Parameters::storeParameter<std::string>(const std::string& param,
                                                 const ParameterMetadata& paramMetadata);
} // namespace wbt

// VECTOR
namespace wbt {
    // Explicit declaration for numeric types
    extern template bool Parameters::storeParameter<int>(const std::vector<int>& param,
                                                         const ParameterMetadata& paramMetadata);
    extern template bool Parameters::storeParameter<bool>(const std::vector<bool>& param,
                                                          const ParameterMetadata& paramMetadata);
    extern template bool Parameters::storeParameter<double>(const std::vector<double>& param,
                                                            const ParameterMetadata& paramMetadata);
    extern template bool
    Parameters::storeParameter<std::string>(const std::vector<std::string>& param,
                                            const ParameterMetadata& paramMetadata);
} // namespace wbt

// PARAMETER
namespace wbt {
    // Explicit declaration for numeric types
    extern template bool Parameters::storeParameter<int>(const Parameter<int>& parameter);
    extern template bool Parameters::storeParameter<bool>(const Parameter<bool>& parameter);
    extern template bool Parameters::storeParameter<double>(const Parameter<double>& parameter);
    extern template bool
    Parameters::storeParameter<std::string>(const Parameter<std::string>& parameter);
} // namespace wbt

#endif // WBT_PARAMETERS_H
