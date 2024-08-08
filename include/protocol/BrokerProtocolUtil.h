////////////////////////////////////////////////////////////////////////////////
//
// BrokerProtocolUtil.h
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

#ifndef BROKERPROTOCOLUTIL_H_
#define BROKERPROTOCOLUTIL_H_

#include <memory>
#include <cstdint>

#include "ByteBuffer.h"

namespace lightmq { namespace protocol {

class BrokerProtocolUtil {
public:
    BrokerProtocolUtil() =delete; // abstract object for static method only

    static void parseVariableType(std::string& result, dep::ByteBuffer& buffer, const char* exception);
    static bool parseVariableTypeOptional(std::string& result, dep::ByteBuffer& buffer);

    static dep::ByteBuffer* parseBody(dep::ByteBuffer& buffer);

    static bool writeVariableType(dep::ByteBuffer& buffer, const std::string& value);
    static bool writeBodyType(dep::ByteBuffer& buffer, const dep::ByteBuffer* body);

    static const char *getMessageTypeName(uint8_t msgType);
};

}} // namespace lightmq { namespace protocol {

#endif /* BROKERPROTOCOLUTIL_H_ */
