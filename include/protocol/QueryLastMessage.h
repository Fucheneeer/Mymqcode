////////////////////////////////////////////////////////////////////////////////
//
// QueryLastMessage.h
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

#ifndef QUERYLASTMESSAGE_H_
#define QUERYLASTMESSAGE_H_


#include <string>
#include <cstdint>

#include "BrokerProtocolConstant.h"
#include "IMessage.h"

namespace lightmq { namespace protocol {

// class dep::ByteBuffer;

class QueryLastMessage : public protocol::IMessage {
public:
    QueryLastMessage()=default;
    virtual ~QueryLastMessage() = default;

    virtual void parse(dep::ByteBuffer& buffer) override;

    virtual void writeTo(dep::ByteBuffer& buffer) override;

    void setReceiptID(uint16_t receiptID) {m_receiptID=receiptID;};
    uint16_t getReceiptID() const {return m_receiptID;};

    void setDestination(const std::string& destination) { m_destination = destination; }
    const std::string& getDestination() const { return m_destination; }

    uint8_t getMessageType() override {return protocol::BrokerProtocolConstant::MESSAGE_TYPE_QUERY_LAST_MESSAGE;};

private:
    uint16_t m_receiptID{0};
    std::string m_destination;
};


}} // namespace lightmq { namespace protocol {


#endif /* QUERYLASTMESSAGE_H_ */
