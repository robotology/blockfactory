/*
 * Copyright (C) 2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * GNU Lesser General Public License v2.1 or any later version.
 */

#ifndef MXANYTYPE_H
#define MXANYTYPE_H

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace mxpp {
    class MxArray;
    using MxArrayPtr = std::shared_ptr<MxArray>;
    using MxCell = std::vector<MxArrayPtr>;
    using MxStructKey = std::string;
    using MxStruct = std::unordered_map<MxStructKey, MxArrayPtr>;
} // namespace mxpp

struct mxArray_tag;
using mxArray = struct mxArray_tag;

/**
 * @brief Slim wrapper of mxArray
 *
 * This class wraps a `mxArray` opaque pointer, which is the fundamental type underlying Matlab
 * data.
 *
 * @see https://mathworks.com/help/matlab/apiref/mxarray.html
 * @note It only supports reading data from a `mxArray*`, not writing.
 */
class mxpp::MxArray
{
private:
    class Impl;
    std::unique_ptr<Impl> pImpl;

public:
    MxArray() = delete;
    explicit MxArray(const mxArray* m, bool validateId = false);

    ~MxArray();
    explicit MxArray(const MxArray& mxAnyType);

    void enableClassIDValidation();

    // Character
    bool asString(std::string& s);

    // Unsigned integer
    bool asUInt(unsigned& i);
    // bool asUInt8(uint8_t& i);
    // bool asUInt16(uint16_t& i);
    // bool asUInt32(uint32_t& i);
    // bool asUInt64(uint64_t& i);

    // Integer
    bool asInt(int& i);
    // bool asInt8(int8_t& i);
    // bool asInt16(int16_t& i);
    bool asInt32(int32_t& i);
    // bool asInt64(int64_t& i);

    // Boolean
    bool asBool(bool& b);

    // Floating point
    bool asDouble(double& d);

    // Struct
    bool asMxStruct(MxStruct& s);

    // Cell
    bool asMxCell(MxCell& cell);

    // Vector
    // TODO: valarray? And matrices?
    bool asVectorDouble(std::vector<double>& vec);

    // TODO: Matrix
};

#endif // MXANYTYPE_H
