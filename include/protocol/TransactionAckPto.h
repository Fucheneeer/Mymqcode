////////////////////////////////////////////////////////////////////////////////
//
// TransactionAckPto.h
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

namespace lightmq { namespace protocol {

// class dep::ByteBuffer;

class TransactionAckPto : public protocol::IMessage  
{
public:
    TransactionAckPto() = default;
    ~TransactionAckPto() = default;

    void parse(dep::ByteBuffer& buffer) override;

    void writeTo(dep::ByteBuffer& buffer) override;

    void setSubscriptionID(uint16_t subscriptionID) {m_subscriptionID=subscriptionID;} ;
    uint16_t getSubscriptionID() const {return m_subscriptionID;};

    void setDestination(const std::string& destination) {m_destination=destination;};
    const std::string& getDestination() const {return m_destination;};

    uint8_t getMessageType() override {return protocol::BrokerProtocolConstant::MESSAGE_TYPE_TRANSACTION_ACK_PTO;};

private:
    uint16_t m_subscriptionID{0};
    std::string m_destination;
};

}}
