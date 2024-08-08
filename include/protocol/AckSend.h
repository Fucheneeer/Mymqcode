////////////////////////////////////////////////////////////////////////////////
//
// AckSend.h
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

#ifndef ACKSEND_H_
#define ACKSEND_H_

#include "IMessage.h"
#include "BrokerProtocolConstant.h"
#include "BrokerTypes.h"


namespace lightmq { namespace protocol {

// class dep::ByteBuffer;

class AckSend : public protocol::IMessage {
public:

    AckSend() =default;
    virtual ~AckSend() = default;

    void parse(dep::ByteBuffer& buffer) override;

    void writeTo(dep::ByteBuffer& buffer) override;

    void setDestination(const std::string& destination) { m_destination = destination; }
    const std::string& getDestination() const { return m_destination; }

    void setSessionID(SessionID sessionID) {m_sessionID=sessionID;};
    SessionID getSessionID() const {return m_sessionID;};

    void setSequence(uint64_t sequence) {m_sequence=sequence;};
    uint64_t getSequence() const {return m_sequence;};

    void setEndSequence(uint64_t sequence) {m_endSequence=sequence;};
    uint64_t getEndSequence() const {return m_endSequence;};

    void setResultCode(ResultCode code) {m_resultCode=code;}
    ResultCode getResultCode() const {return m_resultCode;};

    uint8_t getMessageType() override {return protocol::BrokerProtocolConstant::MESSAGE_TYPE_ACKSEND;};
private:
    std::string m_destination;
    SessionID m_sessionID{0};
    uint64_t m_sequence{0};
    uint64_t m_endSequence{0};
    ResultCode m_resultCode{0};
};


}} // namespace lightmq { namespace protocol {

#endif /* ACKSEND_H_ */
