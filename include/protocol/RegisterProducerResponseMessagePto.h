////////////////////////////////////////////////////////////////////////////////
//
// RegisterProducerRspMessagePto.h
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

#ifndef REGISTER_PRODUCER_RSP_MESSAGE_PTO_H_
#define REGISTER_PRODUCER_RSP_MESSAGE_PTO_H_

#include <cstdint>
#include <string>
#include <memory>
#include <functional>

#include "BrokerProtocolConstant.h"
#include "IMessage.h"
#include "BrokerTypes.h"

namespace lightmq { namespace protocol {

// class dep::ByteBuffer;

class RegisterProducerResponseMessagePto : public protocol::IMessage {
public:

    RegisterProducerResponseMessagePto();
    virtual ~RegisterProducerResponseMessagePto();
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

    void setDefaultTTL(uint64_t ttl) { m_defaultTTL = ttl; }
    uint64_t getDefaultTTL() const { return m_defaultTTL; }

    uint8_t getMessageType() override {return protocol::BrokerProtocolConstant::MESSAGE_TYPE_REGISTER_PRODUCER_RESPONSE_MESSAGE_PTO;};

private:
    uint16_t m_receiptID{0};
    ResultCode m_resultCode;
    SessionID m_sessionID;
    uint64_t m_sequence{0};
    uint64_t m_defaultTTL{0};
};

}} // namespace lightmq { namespace protocol {


#endif /* REGISTER_PRODUCER_RSP_MESSAGE_PTO_H_ */
