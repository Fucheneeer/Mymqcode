////////////////////////////////////////////////////////////////////////////////
//
// MessagePull.h
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
#pragma once


#include <cstdint>
#include <string>

#include "BrokerProtocolConstant.h"
#include "IMessage.h"

#define MESSAGEPULL_PARAM_LIST\
             1, m_destination,\
             2, m_timeout\

namespace lightmq { namespace protocol {


class MessagePull : public protocol::IMessage  
{
public:
    MessagePull() = default;
    MessagePull(const MessagePull &pull);
    MessagePull(const std::string &destination, int64_t timeout);
    ~MessagePull() = default;

    void parse(dep::ByteBuffer& buffer) override;

    void writeTo(dep::ByteBuffer& buffer) override;
    virtual uint8_t getMessageType()
    {
        return BrokerProtocolConstant::MESSAGE_TYPE_MESSAGEPULL;
    }
    
    void setDestination(const std::string &destination);
    std::string getDestination() const;

    void setTimeout(int64_t timeout);
    int64_t getTimeout() const;

protected:
    std::string m_destination;
    uint64_t m_timeout = {0}; // milliseconds;
};

}}
