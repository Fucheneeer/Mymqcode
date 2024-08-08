////////////////////////////////////////////////////////////////////////////////
//
// AckPto.h
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

#ifndef ACK_PTO_H_
#define ACK_PTO_H_

#include <string>

#include "BrokerProtocolConstant.h"
#include "IMessage.h"
#include "SessionScheduleUtility.h"

namespace lightmq { namespace protocol {

// class dep::ByteBuffer;

class AckPto : public protocol::IMessage {
public:
    typedef uint8_t ACK_TYPE;
    AckPto() =default;
    virtual ~AckPto() = default;

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

    uint8_t getMessageType() {return protocol::BrokerProtocolConstant::MESSAGE_TYPE_ACK_PTO;};

    ACK_TYPE getType() const{ return m_type;}
    void setType(ACK_TYPE type){ m_type = type; }
private:
    uint64_t m_sequence{0};
    uint64_t m_endSequence{0};
    uint16_t m_subscriptionID{0};
    SessionID m_sessionID{0};   // uint64_t
    std::string m_destination;
    ACK_TYPE m_type{0};

};


}} // namespace lightmq { namespace protocol {




#endif /* ACK_PTO_H_ */
