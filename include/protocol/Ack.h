////////////////////////////////////////////////////////////////////////////////
//
// Ack.h
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

#ifndef ACK_H_
#define ACK_H_

#include <string>

#include "BrokerProtocolConstant.h"
#include "IMessage.h"
#include "SessionScheduleUtility.h"

namespace lightmq { namespace protocol {

// class dep::ByteBuffer;

class Ack : public protocol::IMessage {
public:

    Ack() =default;
    virtual ~Ack() = default;

    void parse(dep::ByteBuffer& buffer) override;

    void writeTo(dep::ByteBuffer& buffer) override;

    void setSessionID(SessionID sessionID) {m_sessionID=sessionID;}
    SessionID getSessionID() const {return m_sessionID;}

    void setSequence(uint64_t sequence) {m_sequence=sequence;};
    uint64_t getSequence() const {return m_sequence;};

    void setEndSequence(uint64_t sequence) {m_endSequence=sequence;};
    uint64_t getEndSequence() const {return m_endSequence;};

    void setSubscriptionID(uint16_t subscriptionID) {m_subscriptionID=subscriptionID;};
    uint16_t getSubscriptionID() const {return m_subscriptionID;};

    void setDestination(const std::string& destination) { m_destination = destination; }
    const std::string& getDestination() const { return m_destination; }

    uint8_t getMessageType() {return protocol::BrokerProtocolConstant::MESSAGE_TYPE_ACK;};

private:
    uint64_t m_sequence{0};
    uint64_t m_endSequence{0};
    uint16_t m_subscriptionID{0};
    SessionID m_sessionID{0};
    std::string m_destination;
};


}} // namespace lightmq { namespace protocol {




#endif /* ACK_H_ */
