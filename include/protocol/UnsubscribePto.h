////////////////////////////////////////////////////////////////////////////////
//
// Unsubscribe.h
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

#ifndef UNSUBSCRIBEPTO_H_
#define UNSUBSCRIBEPTO_H_


#include <cstdint>
#include <string>

#include "BrokerProtocolConstant.h"
#include "IMessage.h"

namespace lightmq {
namespace protocol {

class UnsubscribePto : public protocol::IMessage {
public:

    UnsubscribePto() = default;
    ~UnsubscribePto() = default;

    void parse(dep::ByteBuffer& buffer) override;

    void writeTo(dep::ByteBuffer& buffer) override;

    void setSubscriptionID(uint16_t subscriptionID) { m_subscriptionID = subscriptionID; };
    uint16_t getSubscriptionID() const { return m_subscriptionID; };

    void setReceiptID(uint16_t receiptID) { m_receiptID = receiptID; };
    uint16_t getReceiptID() const { return m_receiptID; };

    void setDestination(const std::string& destination) { m_destination = destination; };
    std::string getDestination() const { return m_destination; };

    void setRequestID(uint64_t id) { m_requestID = id; };
    uint64_t getRequestID() const { return m_requestID; };

    uint8_t getMessageType() override { return protocol::BrokerProtocolConstant::MESSAGE_TYPE_UNSUBSCRIBE; };
private:
    uint16_t    m_subscriptionID;
    uint16_t    m_receiptID;
    std::string m_destination;
    uint64_t    m_requestID{ 0 };

};

}} // namespace lightmq { namespace protocol {


#endif /* UNSUBSCRIBEPTO_H_ */
