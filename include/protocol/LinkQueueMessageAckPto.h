////////////////////////////////////////////////////////////////////////////////
//
// LinkQueueMessageAckPto.h
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

#ifndef LINQUEUE_MESSAGE_ACK_PTO_H_
#define LINQUEUE_MESSAGE_ACK_PTO_H_

#include <string>

#include "BrokerProtocolConstant.h"
#include "IMessage.h"
#include "SessionScheduleUtility.h"

namespace lightmq { namespace protocol {

// class dep::ByteBuffer;

class LinkQueueMessageAckPto : public protocol::IMessage {
public:

	LinkQueueMessageAckPto() =default;
    virtual ~LinkQueueMessageAckPto() = default;

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

	void setClientID(const std::string& clientID) { m_clientID = clientID; }
	const std::string& getClientID() const { return m_clientID; }

    uint8_t getMessageType() {return protocol::BrokerProtocolConstant::MESSAGE_TYPE_LINK_QUEUE_MESSAGE_ACK_PTO;};

    uint8_t getType() const{ return m_type;}
    void setType(uint8_t type){ m_type = type; }

private:
    uint64_t m_sequence{0};
    uint64_t m_endSequence{0};
    uint16_t m_subscriptionID{0};
    SessionID m_sessionID{0};
    std::string m_destination;
	std::string m_clientID;
    uint8_t m_type;
};


}} // namespace lightmq { namespace protocol {



#endif /* LINQUEUE_MESSAGE_ACK_PTO_H_ */
