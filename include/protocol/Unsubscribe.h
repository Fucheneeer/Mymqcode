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

#ifndef UNSUBSCRIBE_H_
#define UNSUBSCRIBE_H_


#include <cstdint>
#include <string>

#include "BrokerProtocolConstant.h"
#include "IMessage.h"

namespace lightmq { namespace protocol {

// class dep::ByteBuffer;
class Unsubscribe : public protocol::IMessage {
public:

    Unsubscribe();
    ~Unsubscribe() = default;

    void parse(dep::ByteBuffer& buffer) override;

    void writeTo(dep::ByteBuffer& buffer) override;

    void setSubscriptionID(uint16_t subscriptionID) {m_subscriptionID=subscriptionID;} ;
    uint16_t getSubscriptionID() const {return m_subscriptionID;};

    void setReceiptID(uint16_t receiptID) {m_receiptID=receiptID;};
    uint16_t getReceiptID() const {return m_receiptID;};

    uint8_t getMessageType() override {return protocol::BrokerProtocolConstant::MESSAGE_TYPE_UNSUBSCRIBE;};
private:
    uint16_t m_subscriptionID;
    uint16_t m_receiptID;

};

}} // namespace lightmq { namespace protocol {


#endif /* UNSUBSCRIBE_H_ */
