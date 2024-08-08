////////////////////////////////////////////////////////////////////////////////
//
// ReceiptLastMessage.h
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

#ifndef RECEIPTLASTMESSAGE_H_
#define RECEIPTLASTMESSAGE_H_

#include <cstdint>
#include <string>
#include <memory>
#include <functional>

#include "BrokerProtocolConstant.h"
#include "IMessage.h"
#include "BrokerTypes.h"

namespace lightmq { namespace protocol {

// class dep::ByteBuffer;

class ReceiptLastMessage : public protocol::IMessage {
public:
    ReceiptLastMessage();
    virtual ~ReceiptLastMessage();

    void parse(dep::ByteBuffer& buffer) override;

    void writeTo(dep::ByteBuffer& buffer) override;


    void setReceiptID(uint16_t id) { m_receiptID = id; }
    uint16_t getReceiptID() const { return m_receiptID; }

    void setResultCode(ResultCode code) {m_resultCode=code;}
    ResultCode getResultCode() const {return m_resultCode;};

    void setSessionID(SessionID sessionID) {m_sessionID=sessionID;}
    SessionID getSessionID() {return m_sessionID;}

    void setSequence(uint64_t sequence) { m_sequence = sequence; }
    uint64_t getSequence() const { return m_sequence; }

    uint8_t getMessageType() override {return protocol::BrokerProtocolConstant::MESSAGE_TYPE_RECEIPT_LAST_MESSAGE;};

    void setWriteBodyFunction(std::function<uint64_t(dep::ByteBuffer&)>&& function) {m_writeBodyFunction = function;};
    void setReadBodyFunction(std::function<void(SessionID sessionID, uint64_t sequence, dep::ByteBuffer&)>&& function) {m_readBodyFunction = function;}

private:
    uint16_t m_receiptID;
    ResultCode m_resultCode;
    SessionID m_sessionID;
    uint64_t m_sequence;
    std::function<uint64_t(dep::ByteBuffer&)> m_writeBodyFunction;
    std::function<void(SessionID sessionID, uint64_t sequence, dep::ByteBuffer&)> m_readBodyFunction;
};

}} // namespace lightmq { namespace protocol {


#endif /* RECEIPTLASTMESSAGE_H_ */
