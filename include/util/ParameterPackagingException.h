////////////////////////////////////////////////////////////////////////////////
//
// ParameterPackagingException.h
//
// Copyright (c) 2020 CFETSIT.
// Building 3, 491 YinDong Road, Shanghai, China.
// All rights reserved.
//
// LightMQ is the copyrighted,
// proprietary property of CFETSIT which retain all right, title and interest therein.
//
// Description:
//
////////////////////////////////////////////////////////////////////////////////
#ifndef PARAMETER_PACKAGING_EXCEPTION_H_
#define PARAMETER_PACKAGING_EXCEPTION_H_
#include <string>
#include "DEPException.h"

namespace lightmq{ namespace util{

struct ParameterPackagingException : dep::DEPException {
    explicit ParameterPackagingException(const std::string& message) : dep::DEPException(message) {};
    virtual ~ParameterPackagingException() = default;
};

struct ParameterUnpackagingException : virtual dep::DEPException {
    explicit ParameterUnpackagingException(const std::string& message) : dep::DEPException(message) {};
    virtual ~ParameterUnpackagingException() = default;
};

}}
#endif
