/*
 * Copyright (C) 2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * GNU Lesser General Public License v2.1 or any later version.
 */

#include "BlockFactory/Core/Parameter.h"

using namespace wbt;

const std::string defaultParamPrefix = "ParamIndex_";

ParameterMetadata::ParameterMetadata(const ParameterType& t,
                                     const unsigned& paramIndex,
                                     const int& paramRows,
                                     const int& paramCols,
                                     const std::string& paramName)
    : index(paramIndex)
    , name(paramName.empty() ? (defaultParamPrefix + std::to_string(index)) : paramName)
    , rows(paramRows)
    , cols(paramCols)
    , type(t)
{}

ParameterMetadata::ParameterMetadata(const ParameterMetadata& paramMD)
    : index(paramMD.index)
    , name(paramMD.name.empty() ? (defaultParamPrefix + std::to_string(index)) : paramMD.name)
    , rows(paramMD.rows)
    , cols(paramMD.cols)
    , type(paramMD.type)
{}

ParameterMetadata::ParameterMetadata(ParameterMetadata&& other)
    : index(other.index)
    , name(other.name.empty() ? (defaultParamPrefix + std::to_string(index)) : other.name)
    , rows(other.rows)
    , cols(other.cols)
    , type(other.type)
{}

ParameterMetadata& ParameterMetadata::operator=(const ParameterMetadata& other)
{
    *this = ParameterMetadata(other.type, other.index, other.rows, other.cols, other.name);
    return *this;
}

ParameterMetadata& ParameterMetadata::operator=(ParameterMetadata&& other)
{
    ParameterMetadata paramMD(other.type, other.index, other.rows, other.cols, other.name);
    *this = paramMD;
    return *this;
}

bool ParameterMetadata::operator==(const ParameterMetadata& rhs) const
{
    bool ok = true;
    ok = ok && (this->index == rhs.index);
    ok = ok && (this->name == rhs.name);
    ok = ok && (this->rows == rhs.rows);
    ok = ok && (this->cols == rhs.cols);
    ok = ok && (this->type == rhs.type);
    return ok;
}
