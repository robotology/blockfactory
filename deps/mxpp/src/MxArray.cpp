/*
 * Copyright (C) 2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * GNU Lesser General Public License v2.1 or any later version.
 */

#include "mxpp/MxArray.h"

#include <cassert>
#include <matrix.h>

using namespace mxpp;

class MxArray::Impl
{
public:
    struct MxMetadata
    {
        mxClassID id;
        bool isScalar;
        size_t rows;
        size_t cols;
        size_t nElem;
        size_t nDims;
        std::vector<size_t> dims;
    } md;

    const mxArray* mx = nullptr;
    bool validate;

    // TODO: https://it.mathworks.com/help/matlab/apiref/mxgetscalar.html returns a double always
    bool asScalar(double& d);
    bool validateClassId(mxClassID id1, mxClassID id2);
    Impl* clone() { return new Impl(*this); }

    Impl() = delete;
    Impl(const mxArray* _mx, bool _validate)
        : mx(_mx)
        , validate(_validate)
    {}
};

bool MxArray::Impl::asScalar(double& d)
{
    if (!mx) {
        return false;
    }

    if (!mxIsScalar(mx)) {
        return false; // 1x1
    }

    if (!mxIsNumeric(mx)) {
        return false; // Types: https://it.mathworks.com/help/matlab/apiref/mxisnumeric.html
    }

    // Cast to double since even a mxINT8_CLASS is returned as double:
    // https://it.mathworks.com/help/matlab/apiref/mxgetscalar.html
    d = static_cast<double>(mxGetScalar(mx));
    return true;
}

bool MxArray::Impl::validateClassId(mxClassID id1, mxClassID id2)
{
    if (validate) {
        return id1 == id2;
    }
    else {
        return true;
    }
}

// PUBLIC METHODS
// ==============

// Constructors
// ============

MxArray::MxArray(const mxArray* mx, bool validate)
    : pImpl(std::make_unique<Impl>(mx, validate))
{
    assert(pImpl->mx);

    // Get the ID
    pImpl->md.id = mxGetClassID(pImpl->mx);
    assert(pImpl->md.id != mxVOID_CLASS);
    assert(pImpl->md.id != mxUNKNOWN_CLASS);

    // Get the other metadata
    pImpl->md.isScalar = mxIsScalar(pImpl->mx);
    pImpl->md.rows = static_cast<unsigned>(mxGetN(pImpl->mx));
    pImpl->md.cols = static_cast<unsigned>(mxGetM(pImpl->mx));
    pImpl->md.nElem = static_cast<unsigned>(mxGetNumberOfElements(pImpl->mx));
    pImpl->md.nDims = static_cast<unsigned>(mxGetNumberOfDimensions(pImpl->mx));

    if (pImpl->md.isScalar) {
        assert(pImpl->md.rows == pImpl->md.cols == pImpl->md.nElem == 1);
    }

    // TODO: only 2 dims currently supported
    assert(pImpl->md.nDims <= 2);
    assert(pImpl->md.rows * pImpl->md.cols == pImpl->md.nElem);

    const size_t* size = mxGetDimensions(pImpl->mx);
    for (unsigned dim = 0; dim < pImpl->md.nDims; ++dim) {
        pImpl->md.dims.push_back(static_cast<unsigned>(size[dim]));
    }
    assert(pImpl->md.dims.size() == 2);
}

MxArray::~MxArray() = default;

MxArray::MxArray(const MxArray& other)
    : pImpl(other.pImpl->clone())
{}

void MxArray::enableClassIDValidation()
{
    pImpl->validate = true;
}

// STRING / CHARS
// ==============

bool MxArray::asString(std::string& s)
{
    if (!pImpl->mx) {
        return false;
    }
    if (pImpl->md.id != mxCHAR_CLASS) {
        return false;
    }
    char* buffer = mxArrayToString(pImpl->mx);
    s = std::string(buffer);
    mxFree(buffer);
    return true;
}

// SCALAR TYPES
// ============

// Generic casting
// ---------------

bool MxArray::asInt(int& i)
{
    double buffer;
    if (!pImpl->asScalar(buffer)) {
        return false;
    }
    i = static_cast<int>(buffer);
    return true;
}

bool MxArray::asUInt(unsigned& i)
{
    double buffer;
    if (!pImpl->asScalar(buffer)) {
        return false;
    }
    i = static_cast<unsigned>(buffer);
    return true;
}

// Specific casting
// ----------------

bool MxArray::asInt32(int32_t& i)
{
    double buffer;
    if (!pImpl->asScalar(buffer)) {
        return false;
    }
    i = static_cast<int32_t>(buffer);
    return pImpl->validateClassId(pImpl->md.id, mxINT32_CLASS);
}

bool MxArray::asDouble(double& d)
{
    bool ok = pImpl->asScalar(d);
    return ok && pImpl->validateClassId(pImpl->md.id, mxDOUBLE_CLASS);
}

bool MxArray::asBool(bool& b)
{
    if (!pImpl->mx) {
        return false;
    }
    if (!mxIsLogicalScalar(pImpl->mx)) {
        return false;
    }
    b = mxIsLogicalScalarTrue(pImpl->mx);
    return true;
}

// COMPOSITE DATA TYPES
// ====================

bool MxArray::asMxStruct(MxStruct& s)
{
    if (!pImpl->mx) {
        return false;
    }
    if (pImpl->md.id != mxSTRUCT_CLASS) {
        return false;
    }

    for (unsigned i = 0; i < mxGetNumberOfFields(pImpl->mx); ++i) {
        const char* fieldName = mxGetFieldNameByNumber(pImpl->mx, i);
        // TODO multidimensional struct
        mxArray* fieldContent = mxGetFieldByNumber(pImpl->mx, 0, i);
        if (!fieldName) {
            return false;
        }
        if (!fieldContent) {
            return false;
        }
        s[std::string(fieldName)] = std::make_shared<MxArray>(fieldContent);
    }
    return true;
}

bool MxArray::asMxCell(MxCell& cell)
{
    if (!pImpl->mx) {
        return false;
    }
    if (pImpl->md.id != mxCELL_CLASS) {
        return false;
    }

    // TODO: AnyCell then will have a operator()(3,4) method;
    for (unsigned i = 0; i < mxGetNumberOfElements(pImpl->mx); ++i) {
        mxArray* cellContent = mxGetCell(pImpl->mx, i);
        if (!cellContent) {
            return false;
        }
        cell.push_back(std::make_shared<MxArray>(cellContent));
    }
    return true;
}

// MATRIX
// ======

// VECTOR
// ======

// TODO:
// Test with other types (uint8) https://it.mathworks.com/help/matlab/apiref/mxgetdata.html
// (Description)
bool MxArray::asVectorDouble(std::vector<double>& vec)
{
    if (!pImpl->mx) {
        return false;
    }
    if (!mxIsDouble(pImpl->mx)) {
        return false;
    }

    if (pImpl->md.rows > 1 && pImpl->md.cols > 1) {
        return false;
    }

    // TODO add method for complex vectors (and move the check into md)
    if (mxIsComplex(pImpl->mx)) {
        return false;
    }

    double* buffer = mxGetPr(pImpl->mx);
    if (!buffer) {
        return false;
    }

    vec.reserve(pImpl->md.rows * pImpl->md.cols);
    vec.assign(buffer, buffer + pImpl->md.rows * pImpl->md.cols);
    return true;
}
