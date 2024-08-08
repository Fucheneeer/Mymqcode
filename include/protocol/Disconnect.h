////////////////////////////////////////////////////////////////////////////////
//
// Disconnect.h
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

#ifndef DISCONNECT_H_
#define DISCONNECT_H_

#include <string>

#include "BrokerProtocolConstant.h"
#include "IMessage.h"

namespace lightmq { namespace protocol {

// class dep::ByteBuffer;

class Disconnect : public protocol::IMessage {
public:

    Disconnect();
    ~Disconnect() = default;

    void parse(dep::ByteBuffer& buffer) override;

    void writeTo(dep::ByteBuffer& buffer) override;

    void setReceiptID(uint16_t id) { m_receiptID = id; }

    uint16_t getReceiptID() const { return m_receiptID; }

    uint8_t getMessageType() override {return protocol::BrokerProtocolConstant::MESSAGE_TYPE_DISCONNECT;};

private:
    uint16_t m_receiptID;

};

}} // namespace lightmq { namespace protocol {

#endif /* DISCONNECT_H_ */
