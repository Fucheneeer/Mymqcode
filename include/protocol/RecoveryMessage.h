////////////////////////////////////////////////////////////////////////////////
//
// RecoveryMessage.h
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

#ifndef RECOVERYMESSAGE_H_
#define RECOVERYMESSAGE_H_

#include <cstdint>
#include <string>
#include <memory>
#include <vector>

#include "BrokerProtocolConstant.h"
#include "IMessage.h"
#include "Message.h"
#include "SessionScheduleUtility.h"
#include <unordered_map>

namespace lightmq { namespace protocol {

// class dep::ByteBuffer;
class RecoveryMessage : public protocol::IMessage {
public:
    typedef std::function<uint64_t(dep::ByteBuffer&, uint64_t& msgCount)> WriteBodyFunc;
    typedef std::function<void(const std::string& destination, SessionID sessionID, uint64_t sequence, bool isRemoteMessage, bool isDeletedMessage, uint64_t beginSequence, dep::ByteBuffer&) > ReadBodyFunc;

    RecoveryMessage()=default;
    virtual ~RecoveryMessage()=default;

    void parse(dep::ByteBuffer& buffer) override;

    void writeTo(dep::ByteBuffer& buffer) override;


    void setDestination(const std::string& destination) { m_destination = destination; }
    const std::string& getDestination() const { return m_destination; }

    void setBeginSequence(uint64_t beginSequence) { m_beginSequence = beginSequence; }
    uint64_t getBeginSequence() const { return m_beginSequence; }

	void setSequence(uint64_t sequence) { m_sequence = sequence; }
	uint64_t getSequence() const { return m_sequence; }
  
	//  void setEndSequence(uint64_t sequence) { m_endSequence = sequence; }// endSequence calculated based on Body
    uint64_t getEndSequence() const { return m_endSequence; }

    void setSessionID(SessionID sessionID) { m_sessionID= sessionID;}
    SessionID getSessionID() const {return m_sessionID;}

	void setDeletedMessageSeqs(std::vector<uint64_t> deletedMessageSeqs) { 
			m_deletedMessageSeqs.insert(deletedMessageSeqs.begin(), deletedMessageSeqs.end());
	}
	std::unordered_set<uint64_t> getDeletedMessageSeqs() const { return m_deletedMessageSeqs; }

	void setRemoteMessageSeqs(std::vector<uint64_t> remoteMessageSeqs) { 
			m_remoteMessageSeqs.insert(remoteMessageSeqs.begin(), remoteMessageSeqs.end());
	}
	std::unordered_set<uint64_t> getRemoteMessageSeqs() const { return m_remoteMessageSeqs; }

    void setExpiredMessageSeqs(std::vector<uint64_t> expiredMessageSeqs) { 
        m_expiredMessageSeqs.insert(m_expiredMessageSeqs.begin(), m_expiredMessageSeqs.end());
	}
	std::unordered_set<uint64_t> getExpiredMessageSeqs() const { return m_expiredMessageSeqs; }

	// void setMessageProperties(uint64_t sequence, const std::unordered_map<std::string, std::string>* messageProperties) {
	// 	m_messageProperties[sequence] = messageProperties;

	// }

    void setWriteBodyFunction(std::function<uint64_t(dep::ByteBuffer&, uint64_t& msgCount)>&& function) {m_writeBodyFunction = function;};
	void setReadBodyFunction(std::function<void(MessageSerializeAgent &msgAgent, dep::ByteBuffer&) > && function) {
		m_readBodyFunction = function;
	}

    uint8_t getMessageType() override {return protocol::BrokerProtocolConstant::MESSAGE_TYPE_RECOVERY_MESSAGE;};

private:
    std::string m_destination;
	uint64_t m_beginSequence{0};
    uint64_t m_sequence{0};
    uint64_t m_endSequence{0};
    SessionID m_sessionID{0};
	std::unordered_set<uint64_t> m_deletedMessageSeqs;
	std::unordered_set<uint64_t> m_remoteMessageSeqs;
    std::unordered_set<uint64_t> m_expiredMessageSeqs;
	// std::unordered_map<uint64_t, const std::unordered_map<std::string, std::string>*> m_messageProperties;

    std::function<uint64_t(dep::ByteBuffer&, uint64_t& msgCount)> m_writeBodyFunction{nullptr};
	std::function<void(MessageSerializeAgent &msgAgent, dep::ByteBuffer&)> m_readBodyFunction{nullptr};
};


class EndOfRecoveryForAddress: public protocol::IMessage {
public:

    EndOfRecoveryForAddress() =default;
    virtual ~EndOfRecoveryForAddress() = default;

    virtual void parse(dep::ByteBuffer& buffer) override;
    virtual void writeTo(dep::ByteBuffer& buffer) override;

    void setDestination(const std::string& destination) { m_destination = destination; }
    const std::string& getDestination() const { return m_destination; }

    uint8_t getMessageType() override {return protocol::BrokerProtocolConstant::MESSAGE_TYPE_END_OF_RECOVERY_FOR_ADDRESS;};

private:

    std::string m_destination;
};


class EndOfRecovery : public protocol::IMessage {
public:

    EndOfRecovery() =default;
    virtual ~EndOfRecovery() = default;

    virtual void parse(dep::ByteBuffer& buffer) override;
    virtual void writeTo(dep::ByteBuffer& buffer) override;

    uint8_t getMessageType() override {return protocol::BrokerProtocolConstant::MESSAGE_TYPE_END_OF_RECOVERY;};

private:

};

class StartRecovery : public protocol::IMessage {
public:

    StartRecovery() =default;
    virtual ~StartRecovery() = default;

    virtual void parse(dep::ByteBuffer& buffer) override;
    virtual void writeTo(dep::ByteBuffer& buffer) override;

    uint8_t getMessageType() override {return protocol::BrokerProtocolConstant::MESSAGE_TYPE_START_RECOVERY;};

private:

};


}} // namespace lightmq { namespace protocol {


#endif /* RECOVERYMESSAGE_H_ */
