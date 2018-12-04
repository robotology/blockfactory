/*
 * Copyright (C) 2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * GNU Lesser General Public License v2.1 or any later version.
 */

#ifndef WBT_PARAMETER_H
#define WBT_PARAMETER_H

#include <string>
#include <vector>

namespace wbt {
    class ParameterMetadata;
    template <typename T>
    class Parameter;
    enum class ParameterType;
} // namespace wbt

/**
 * @brief Defines the types of parameters supported by wbt::Parameter
 * @see wbt::ParameterMetadata, wbt::Parameter
 */
enum class wbt::ParameterType
{
    // Scalar / Vector / Matrix
    INT,
    BOOL,
    DOUBLE,
    STRING,
    // Cell
    CELL_INT,
    CELL_BOOL,
    CELL_DOUBLE,
    CELL_STRING,
    // Struct
    STRUCT_INT,
    STRUCT_BOOL,
    STRUCT_DOUBLE,
    STRUCT_STRING,
    STRUCT_CELL_INT,
    STRUCT_CELL_BOOL,
    STRUCT_CELL_DOUBLE,
    STRUCT_CELL_STRING
};

/**
 * @brief Class for storing parameter metadata
 *
 * A metadata must be constructed with an index and a name, and they cannot be changed afterwards.
 * Only bool, int, double or std::string types are currently supported.
 *
 * @see wbt::Parameter, wbt::ParameterType
 */
class wbt::ParameterMetadata
{
public:
    enum
    {
        DynamicSize = -1
    };

    const unsigned index;
    const std::string name;

    int rows;
    int cols;
    wbt::ParameterType type;

    ParameterMetadata() = delete;
    ~ParameterMetadata() = default;

    ParameterMetadata(const ParameterType& t,
                      const unsigned& ParamIndex,
                      const int& paramRows,
                      const int& paramCols,
                      const std::string& ParamName = {});
    ParameterMetadata(const ParameterMetadata& paramMD);
    ParameterMetadata(ParameterMetadata&& paramMD);

    ParameterMetadata& operator=(const ParameterMetadata& paramMD);
    ParameterMetadata& operator=(ParameterMetadata&& paramMD);
    bool operator==(const ParameterMetadata& rhs) const;
    inline bool operator!=(const ParameterMetadata& rhs) const { return !(*this == rhs); }
};

/**
 * @brief Class for storing a generic parameter
 *
 * A generic parameters can be either a scalar or a vector. Supported types are defined by the
 * wbt::ParameterType enum.Use wbt::ParameterMetadata to set these information.
 *
 * @tparam The type of the container type. For vector parameters, T is the type of an element of the
 *         container.
 * @see wbt::Parameters, wbt::ParameterMetadata
 */
template <typename T>
class wbt::Parameter
{
private:
    using ParamVector = std::vector<T>;

    bool m_isScalar;
    T m_valueScalar;
    ParamVector m_valueVector;
    wbt::ParameterMetadata m_metadata;

public:
    enum class Type;
    Parameter() = delete;
    ~Parameter() = default;

    Parameter(const T& value, const wbt::ParameterMetadata& md)
        : m_isScalar(true)
        , m_valueScalar(value)
        , m_metadata(md)
    {}
    Parameter(const ParamVector& valueVec, const wbt::ParameterMetadata& md)
        : m_valueVector(valueVec)
        , m_isScalar(false)
        , m_metadata(md)
    {}

    bool isScalar() const { return m_isScalar; }
    T getScalarParameter() const { return m_valueScalar; }
    ParamVector getVectorParameter() const { return m_valueVector; }
    wbt::ParameterMetadata getMetadata() const { return m_metadata; }
};

#endif // WBT_PARAMETER_H
