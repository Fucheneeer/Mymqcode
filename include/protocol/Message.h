////////////////////////////////////////////////////////////////////////////////
//
// Message.h
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

#ifndef MESSAGE_H_
#define MESSAGE_H_

#include <cstdint>
#include <string>
#include <memory>

#include "BrokerProtocolConstant.h"
#include "IMessage.h"
#include "SessionScheduleUtility.h"
#include "MessageUtil.h"
#include <unordered_map>
#include "VariantMap.h"
#include "LightmqMessageHead.h"

namespace lightmq { namespace protocol {

struct MessageSerializeAgent {
	enum FieldMask {
		FIELD_MASK_remoteBrokerID        = 0X00000001,
		FIELD_MASK_remoteBrokerMessageID = 0X00000002,
		FIELD_MASK_beginSequence         = 0X00000004,
		FIELD_MASK_REMOTE_BROKER         = 0X0000000F,
		FIELD_MASK_transactionID         = 0X00000030,
		FIELD_MASK_transMessageSeq       = 0X00000040,
		FIELD_MASK_TRANSACTION           = 0X000000F0,
		FIELD_MASK_providerID            = 0X00000100,
		FIELD_MASK_providerMessageID     = 0X00000200,
		FIELD_MASK_PROVIDER_INFO         = 0X00000F00,
		FILED_MASK_HEAD_PROPERTIES       = 0X00001000,
		FILED_MASK_HEAD_ALL              = 0X00002000,
		//FIELD_MASK_ALL                   = 0XFFFFFFFF,     
        FIELD_MASK_ClientMessage = FIELD_MASK_TRANSACTION| FIELD_MASK_PROVIDER_INFO | FILED_MASK_HEAD_PROPERTIES,
        FIELD_MASK_HAMessage = FIELD_MASK_TRANSACTION | FIELD_MASK_PROVIDER_INFO | FILED_MASK_HEAD_PROPERTIES,
        FIELD_MASK_BrokerMessage = FIELD_MASK_PROVIDER_INFO | FILED_MASK_HEAD_ALL,
        FIELD_MASK_LinkBrokerMessage = FIELD_MASK_TRANSACTION | FIELD_MASK_REMOTE_BROKER | FIELD_MASK_PROVIDER_INFO | FILED_MASK_HEAD_ALL,
        FIELD_MASK_TransOperMessage = FIELD_MASK_PROVIDER_INFO,
	};
    

	void parseMessage(dep::ByteBuffer &buffer, FieldMask fieldMask);
	void writeMessageOutHead(dep::ByteBuffer &buffer);

	void setProviderMessageID(dep::ByteBuffer &buffer, MessageID providerMsgID);


	void clearContent();

	util::VariantMap* getProperties();

	std::string getRemoteBrokerID() const { return m_remoteBrokerInfo.m_remoteBrokerID; }
	MessageID getRemoteBrokerMessageID() const { return m_remoteBrokerInfo.m_remoteBrokerMessageID; }
	void setRemoteBrokerInfo(const std::string &brokerID, const MessageID &msgID) {
		m_remoteBrokerInfo.m_remoteBrokerID = brokerID;
		m_remoteBrokerInfo.m_remoteBrokerMessageID = msgID;
		m_remoteBrokerInfo.m_hasRemoteBrokerInfo = true;
	}

	MessageID getBrokerMessageID() const { return m_brokerMessageID; }

	TransactionID &getTransactionID() { return m_transInfo.m_transID; }
    const TransactionID& getTransactionID() const { return m_transInfo.m_transID; }
    uint32_t  getTransMessageSeq() const { return m_transInfo.m_transMessageSeq; }
	void  setTransMessageSeq(uint32_t seq) { m_transInfo.m_transMessageSeq = seq; }
    uint32_t getBuffferHeadPosition() const {
        return  m_bufPos.m_bufPosHead;
    }
    uint32_t getBuffferValuePosition() const {
        return  m_bufPos.m_bufPosValue;
    }
    uint32_t getBuffferProviderIDPosition() const {
        return  m_bufPos.m_bufPosProviderMessageID;
    }  

	//  Message parse怣懅
	uint16_t  m_subscriptionID { 0 };
	MessageID m_brokerMessageID;

	//Send Parse怣懅
	std::string m_destination;
	//std::shared_ptr<std::unordered_map<std::string, std::string>> m_messageProperty;
	util::LightmqMessageHead m_header;
	//util::VariantMap m_messageProperty;

	//  RecoverMessage怣懅
	bool m_isDeletedMessage { false };
	bool m_isRemoteMessage { false };
	bool m_isExpiredMessage { false };

	uint64_t m_beginSequence { 0 };

	//  provider messageID info
	std::string m_providerID;
	MessageID m_providerMessageID;

	uint32_t m_messageLength;

	//  Position info
	util::MessageBufferPosition m_bufPos; 

	void relinkMessagePos(int posChange) { m_bufPos.relinkMessagePos(posChange);}

	bool m_hasNextMessage { true };  //  Only Send::parse() implement.

//private:
	//  remote broker messageID info
	util::ExtendBmpRemoteBrokerInfo m_remoteBrokerInfo;

	//  TransactionInfo
    util::ExtendBmpTransactionInfo m_transInfo;
};

// class dep::ByteBuffer;
class Message : public protocol::IMessage {
public:
    typedef std::function<void(uint16_t subscriptionID, SessionID sessionID, uint64_t sequence, const std::string& providerClientID, MessageID providerMessageID, const std::string& remoteBrokerID, MessageID remoteBrokerMessageID, dep::ByteBuffer&)> ReadBodyFunc;
	static const int EXTEND_BMP_REMOTE_BROKER_INFO = 0x01;
	static const int EXTEND_BMP_TRANSACTION_INFO   = 0x02;

public:

    Message();
    virtual ~Message();

    void parse(dep::ByteBuffer& buffer) override;

    void writeTo(dep::ByteBuffer& buffer) override;

    void setSubscriptionID(uint16_t id) {m_subscriptionID=id;}
    uint16_t getSubscriptionID() const {return m_subscriptionID;};

    void setDestination(const std::string& destination) { m_destination = destination; }
    const std::string& getDestination() const { return m_destination; }

    //void setSequence(uint64_t sequence) { m_sequence = sequence; }
    uint64_t getSequence() const { return m_sequence; }

  //  void setEndSequence(uint64_t sequence) { m_endSequence = sequence; }// endSequence calculated based on Body
    uint64_t getEndSequence() const { return m_endSequence; }

    void setSessionID(SessionID sessionID) { m_sessionID= sessionID;}
    SessionID getSessionID() const {return m_sessionID;}

    bool getIsRetransmit() const {return m_isRetransmit;}
    void setIsRetransmit(bool isRetransmit) {m_isRetransmit=isRetransmit;};

	// std::string getRemoteBrokerID1() const { return m_rbInfo.m_remoteBrokerID; }
	// MessageID getRemoteBrokerMessageID1() const { return m_rbInfo.m_remoteBrokerMessageID; }

    void setWriteBodyFunction(std::function<uint64_t(dep::ByteBuffer&,  uint64_t& startSeq, uint64_t& endSeq)>&& function) {m_writeBodyFunction=function;};
    void setReadBodyFunction(std::function<void(MessageSerializeAgent &msgAgent, dep::ByteBuffer&)>&& function) {m_readBodyFunction=function;}

    uint8_t getMessageType() override {return protocol::BrokerProtocolConstant::MESSAGE_TYPE_MESSAGE;};

    virtual uint32_t getWriteMessageOffsetLength() const{ return 0; }

    virtual void writeExtendToBuffer  (dep::ByteBuffer& buffer) const {}

    virtual void readExtendFromBuffer(dep::ByteBuffer& buffer) {}

    virtual void callReadBodyFunc(MessageSerializeAgent &msgAgent, dep::ByteBuffer& buffer);



	//const std::string& getCurrentParseTransactionID() {
	//	return m_transactionID;
	//}
	
protected:
    uint16_t m_subscriptionID;
    std::string m_destination;
    uint64_t m_sequence;
    uint64_t m_endSequence;
    SessionID m_sessionID;
    bool m_isRetransmit;
    std::function<uint64_t(dep::ByteBuffer&, uint64_t& startSeq, uint64_t& endSeq)> m_writeBodyFunction;
    std::function<void(MessageSerializeAgent &msgAgent, dep::ByteBuffer&)> m_readBodyFunction; 

	MessageSerializeAgent m_msgAgent;
};


}} // namespace lightmq { namespace protocol {


#endif /* MESSAGE_H_ */
