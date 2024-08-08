////////////////////////////////////////////////////////////////////////////////
//
// Send.h
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

#ifndef SEND_H_
#define SEND_H_


#include <cstdint>
#include <string>
#include <memory>
#include <unordered_map>
#include <functional>

#include "BrokerProtocolConstant.h"
#include "BrokerTypes.h"
#include "IMessage.h"
#include "MessageUtil.h"
#include "Message.h"

namespace lightmq { namespace protocol {

// class dep::ByteBuffer;

class Send : public protocol::IMessage {
public:
    // typedef std::function<uint32_t(dep::ByteBuffer&, uint64_t currentSequence, bool& hasNext)> WriteBodyFunc;
    //typedef std::function<bool(const std::string& destination, const std::string &providerID, SessionID sessionID, uint64_t sequence, dep::ByteBuffer&, uint32_t headStartPos)> ReadBodyFunc;

    typedef std::function<bool(MessageSerializeAgent& msgAgent, dep::ByteBuffer&)> ReadBodyFunc;

    Send()=default;
    virtual ~Send()=default;

    void parse(dep::ByteBuffer& buffer) override;

    void writeTo(dep::ByteBuffer& buffer) override;

    void setDestination(const std::string& destination) { m_destination = destination; }
    const std::string& getDestination() const { return m_destination; }

	void setTransactionID(uint64_t transID) { m_msgAgent.getTransactionID().m_transID = transID; }
	uint64_t getTransactionID() const { return m_msgAgent.getTransactionID().m_transID; }

	void setTransMessageSeq(uint32_t seq) { m_msgAgent.setTransMessageSeq(seq); }
	uint32_t getTransMessageSeq() const { return m_msgAgent.getTransMessageSeq(); }

	void setTransSessionID(const std::string &transSessionID) { m_msgAgent.getTransactionID().m_transSessionID = transSessionID; }
	const std::string& getTransSessionID() const { return m_msgAgent.getTransactionID().m_transSessionID; }

    void setSessionID(SessionID sessionID) {m_sessionID=sessionID;}
    SessionID getSessionID() {return m_sessionID;}

    void setSequence(uint64_t sequence) { m_sequence = sequence; }
    uint64_t getSequence() const { return m_sequence; }

   // void setEndSequence(uint64_t sequence) { m_endSequence = sequence; } // endSequence calculated based on Body
    uint64_t getEndSequence() const { return m_endSequence; }

    void setClientID(const std::string& clientID) { m_clientID = clientID; }

    void setWriteBodyFunction(util::MessageUtil::WriteStoreDataFunc&& func) { 
        m_writeBodyFunction = func; 
    }
  
    void setReadBodyFunction(std::function<bool(MessageSerializeAgent &msgAgent, dep::ByteBuffer&)>&& function) {
		m_readBodyFunction = function;
	}

    uint8_t getMessageType() override {return protocol::BrokerProtocolConstant::MESSAGE_TYPE_SEND;};

    //void setMessageProperties(const std::shared_ptr<std::unordered_map<std::string,std::string>>& messageProperties) {
    //    m_pMessageProperties = messageProperties;}
    //const std::shared_ptr<std::unordered_map<std::string,std::string>>& getMessageProperties() {
    //    return m_pMessageProperties;}
private:
    std::string m_destination;
    SessionID m_sessionID{0};
    uint64_t m_sequence{0};
    uint64_t m_endSequence{0};
    std::string m_clientID;
    util::MessageUtil::WriteStoreDataFunc m_writeBodyFunction;
	std::function<bool(MessageSerializeAgent& msgAgent, dep::ByteBuffer&)> m_readBodyFunction;
    //std::shared_ptr<std::unordered_map<std::string,std::string>> m_pMessageProperties;

	MessageSerializeAgent m_msgAgent;
};


}} // namespace lightmq { namespace protocol {

#endif /* SEND_H_ */
