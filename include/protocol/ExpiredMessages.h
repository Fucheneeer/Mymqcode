////////////////////////////////////////////////////////////////////////////////
//
// DeleteMessages.h
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

#ifndef EXPIRED_MESSAGE_H_
#define EXPIRED_MESSAGE_H_

#include <string>

#include "IMessage.h"
#include "BrokerTypes.h"
#include "BrokerProtocolConstant.h"

namespace lightmq { namespace protocol {

// class dep::ByteBuffer;

class ExpiredMessages : public protocol::IMessage {
public:

	ExpiredMessages() = default;
	virtual ~ExpiredMessages() = default;

	void parse(dep::ByteBuffer& buffer) override;

	void writeTo(dep::ByteBuffer& buffer) override;

	void setDestination(const std::string& destination) { m_destination = destination; }

	const std::string& getDestination() const { return m_destination; }

	void setSessionID(const SessionID& sessionID) { m_sessionID = sessionID; }

	const SessionID getSessionID() const { return m_sessionID; }

	void setSequence(uint64_t sequence) { m_sequence = sequence; }

	uint64_t getSequence() const { return m_sequence; }

	void setEndSequence(uint64_t sequence) { m_endSequence = sequence; }

	uint64_t getEndSequence() const { return m_endSequence; }

	uint8_t getMessageType() override { return protocol::BrokerProtocolConstant::MESSAGE_TYPE_RECOVERY_EXPIRED_MESSAGES; };

private:
	std::string m_destination;
	SessionID m_sessionID;
	uint64_t m_sequence{ 0 };
	uint64_t m_endSequence{ 0 };
};

}} // namespace lightmq { namespace protocol {


#endif /* EXPIRED_MESSAGE_H_ */
