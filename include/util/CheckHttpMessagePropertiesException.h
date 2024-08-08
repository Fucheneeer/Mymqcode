////////////////////////////////////////////////////////////////////////////////
//
// CheckHttpMessagePropertiesException.h
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
#ifndef CHECK_HTTP_MESSAGE_PROPERTIES_EXCEPTION_H_
#define CHECK_HTTP_MESSAGE_PROPERTIES_EXCEPTION_H_

#include <stdint.h>
#include "DEPException.h"
#include <string>

namespace lightmq { namespace util {

struct CheckHttpMessagePropertiesException : public dep::DEPException{
    explicit CheckHttpMessagePropertiesException(const std::string& msg, int32_t errorCode);
    virtual ~CheckHttpMessagePropertiesException() = default;

    int32_t m_errorCode;
};



}}
#endif
