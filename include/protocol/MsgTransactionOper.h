////////////////////////////////////////////////////////////////////////////////
//
// MsgTransactionOper.h
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
#ifndef MSGTRANSACTIONOPER_H_
#define MSGTRANSACTIONOPER_H_
#include <string>
#include <vector>
#include "BrokerProtocolConstant.h"
#include "BrokerTypes.h"
#include "IMessage.h"
#include "MsgChunk.h"

namespace dep {	class ByteBuffer; }

namespace lightmq { namespace protocol {

enum class TransactionOper : char {
	START_TRANS       = 'S',

	//  operation for local transaction
	COMMIT_TRANS      = 'M',
	ROLLBACK_TRANS    = 'R',

	//  operation for distribute transaction
	//  PRE_COMMIT CAND BE: NOTIFY_COMMIT, NOTIFY_ROLLBACK, OR ROLLBACK_TRANS if transaction is alive in client.
	PRE_COMMIT_TRANS  = 'P',
	NOTIFY_COMMIT     = 'm',
	NOTIFY_ROLLBACK   = 'r',


	DISCARD_TRANS     = 'D'
};

inline std::string to_string(TransactionOper transOper) {
	switch (transOper) {
		case TransactionOper::START_TRANS: return "START_TRANS";
		case TransactionOper::COMMIT_TRANS: return "COMMIT_TRANS";
		case TransactionOper::ROLLBACK_TRANS: return "ROLLBACK_TRANS";
		case TransactionOper::PRE_COMMIT_TRANS: return "PRE_COMMIT_TRANS";
		case TransactionOper::NOTIFY_COMMIT: return "NOTIFY_COMMIT";
		case TransactionOper::NOTIFY_ROLLBACK: return "NOTIFY_ROLLBACK";
		case TransactionOper::DISCARD_TRANS: return "DISCARD_TRANS";
		default: return "UNKNOWED_TRANS_OPER";
	}
}

struct ConsumerAckInfo {
	DestinationMessageID destinationMessageID;
	std::string subscriptionName;
	uint16_t subscriptionId;

	ConsumerAckInfo(const std::string &subsName, const std::string &destination, MessageID brokerMsgID, uint16_t subsID=0)
		: destinationMessageID(destination, brokerMsgID)
		, subscriptionName(subsName)
		, subscriptionId(subsID)
	{}

	ConsumerAckInfo()
		: subscriptionId(0)
	{}

	inline bool compareWithoutSubscriptionID(const ConsumerAckInfo &ackInfo) const {
		return destinationMessageID == ackInfo.destinationMessageID
			&& subscriptionName == ackInfo.subscriptionName;
	}
};

struct CmpConsumerAckInfoWithoutSubscriptionId {
	inline bool operator()(const lightmq::protocol::ConsumerAckInfo &lv, const lightmq::protocol::ConsumerAckInfo &rv) const {
		return lv.compareWithoutSubscriptionID(rv);
	}
};

enum class TransactionMessageSendResult: uint8_t {
	SUCCESS = 'S',
	FAILED  = 'F'
};


//  Transaction messages to commit.
struct TransactionMessageInfo {
	std::string destinationID;
	TransactionMessageSeq messageSeq;          //  Message sequence in Transaction
	TransactionMessageSendResult sendResult;   //  Message send result in client, send failed message will not commit.
};

}} // namespace lightmq { namespace protocol {

namespace std {
    template<>
    struct hash<lightmq::protocol::ConsumerAckInfo> {
        inline size_t operator()(const lightmq::protocol::ConsumerAckInfo &rv) const {
            size_t hashValue = std::hash<lightmq::DestinationMessageID>()(rv.destinationMessageID);
            hash_combine(hashValue, rv.subscriptionName);
            return hashValue;
        }
    };
}

namespace lightmq { namespace protocol {

/*  Local transaction operate message:
 *  operate: char
 *  transactSessionID: string
 *  transID: string
 *
 *  operate block:
 *  rollback:  ackList
 *  commit: ackList, destinationProividerMessageIDs
 *
 */
class MsgTransactionOper : public protocol::MessageBase {
public:
	static const int KEY_BASIC_INFO = MsgChunkInnerKey::MAX_KEY_COUNT + 0;
	static const int KEY_CONSUMER_ACK_LIST = MsgChunkInnerKey::MAX_KEY_COUNT + 1;
	static const int KEY_TRANSACTION_MSGID_LIST = MsgChunkInnerKey::MAX_KEY_COUNT + 2;
	static const int KEY_PROVIDER_MSGID_LIST = MsgChunkInnerKey::MAX_KEY_COUNT + 3;

	struct MsgChunkBasicInfo: public IMsgChunk {
		bool parseFromChunkBuffer(dep::ByteBuffer &chunkBuffer, MsgChunkType chunkType, uint8_t chunkKey, uint32_t index, uint32_t multiCount) override;
		bool writeToChunkBuffer(dep::ByteBuffer &chunkBuffer, uint32_t index) override;
		bool hasValue() const override { return m_transID.m_transID > 0; }
		void clearChunkValue() override {m_transID = TransactionID();}

			MsgChunkBasicInfo()
				: IMsgChunk(MsgChunkType::UserDefine,
				            KEY_BASIC_INFO,
				            MsgChunkAttr(MsgChunkAttr::IS_SMALL))
		{}

		TransactionID m_transID;
		TransactionOper m_transOper;
		uint16_t m_operRound;
		uint16_t m_operSequence;
		uint32_t m_consumerAckCount;
		uint32_t m_providerSendCount;
		TransactionState m_transState;
		bool m_operMySession {true};
	};

	struct MsgChunkProviderMessages: public IMsgChunk {
		bool parseFromChunkBuffer(dep::ByteBuffer &chunkBuffer, MsgChunkType chunkType, uint8_t chunkKey, uint32_t index, uint32_t multiCount) override;
		bool writeToChunkBuffer(dep::ByteBuffer &chunkBuffer, uint32_t index) override;
		bool hasValue() const override { return m_providerMsgIDs.size() > 0; }
		void clearChunkValue() override { m_providerMsgIDs.clear(); }

		MsgChunkProviderMessages()
				: IMsgChunk(MsgChunkType::UserDefine,
				            KEY_PROVIDER_MSGID_LIST,
				            MsgChunkAttr(MsgChunkAttr::IS_MULTI))
		{}

		std::vector<ProviderMessageID> m_providerMsgIDs;
	};

	//  TransactionMessageInfo
	struct MsgChunkTransactionMessages: public IMsgChunk {
		bool parseFromChunkBuffer(dep::ByteBuffer &chunkBuffer, MsgChunkType chunkType, uint8_t chunkKey, uint32_t index, uint32_t multiCount) override;
		bool writeToChunkBuffer(dep::ByteBuffer &chunkBuffer, uint32_t index) override;
		bool hasValue() const override { return m_transMessages.size() > 0; }
		void clearChunkValue() override { m_transMessages.clear(); }

		MsgChunkTransactionMessages()
				: IMsgChunk(MsgChunkType::UserDefine,
				            KEY_TRANSACTION_MSGID_LIST,
				            MsgChunkAttr(MsgChunkAttr::IS_MULTI))
		{}

		std::vector<TransactionMessageInfo> m_transMessages;
	};

	struct MsgChunkConsumerAcks: public IMsgChunk {
		bool parseFromChunkBuffer(dep::ByteBuffer &chunkBuffer, MsgChunkType chunkType, uint8_t chunkKey, uint32_t index, uint32_t multiCount) override;
		bool writeToChunkBuffer(dep::ByteBuffer &chunkBuffer, uint32_t index) override;
		bool hasValue() const override { return m_consumerMsgIDs.size() > 0; }
		void clearChunkValue() override { m_consumerMsgIDs.clear(); }

		MsgChunkConsumerAcks()
				: IMsgChunk(MsgChunkType::UserDefine,
				            KEY_CONSUMER_ACK_LIST,
				            MsgChunkAttr(MsgChunkAttr::IS_MULTI))
		{}

		std::vector<ConsumerAckInfo> m_consumerMsgIDs;
	};

public:
	MsgTransactionOper() = default;
	virtual ~MsgTransactionOper() = default;

	virtual uint8_t getMessageType() override {return protocol::BrokerProtocolConstant::MESSAGE_TYPE_TRANSACT_OPERATE;}

	//uint64_t getTransID() const { return m_transID; }
	//const std::string &getTransSessionID() const { return m_transSessionID; }
	const TransactionID &getTransID() const { return m_chunkBasicInfo.m_transID; }
	TransactionOper getTransOper() const { return m_chunkBasicInfo.m_transOper; }


	void setTransactID(const std::string &sessionID, uint64_t transID) {
		m_chunkBasicInfo.m_transID.setTransactionID(sessionID, transID);
	}

	void setTransactID(const TransactionID &transID) {
		m_chunkBasicInfo.m_transID = transID;
	}

	void setTransactID(TransactionID &&transID) {
		m_chunkBasicInfo.m_transID = std::move(transID);
	}

	void setTransOperRequest(TransactionOper oper, uint16_t operRound, uint16_t operSeq) {
		m_chunkBasicInfo.m_transOper = oper;
		m_receiptHead.m_messageDirection = MessageDirection::REQUEST;
		m_receiptHead.setHasValue();
		m_chunkBasicInfo.m_operRound = operRound;
		m_chunkBasicInfo.m_operSequence = operSeq;
	}

	void setTransState(TransactionState transState) {
		m_chunkBasicInfo.m_transState = transState;
	}

	TransactionState getTransState() {
		return m_chunkBasicInfo.m_transState;
	}

	void makeResponse() {
		m_receiptHead.m_messageDirection = MessageDirection::RECEIPT;
		m_receiptHead.setHasValue();
		//m_chunkProviderMessages.m_providerMsgIDs.clear();
		m_chunkTransMessages.clearChunkValue();
		m_chunkConsumerAcks.clearChunkValue();
	}

	//MsgChunkProviderMessages m_chunkProviderMessages;
	MsgChunkTransactionMessages m_chunkTransMessages;
	MsgChunkConsumerAcks m_chunkConsumerAcks;
	MsgChunkBasicInfo m_chunkBasicInfo;

protected:
	void regMsgChunks(MsgChunkHelper &mch, bool forParse) override;

private:


	//  the following for receipt
	//lightmq::ResultCode m_resultCode;
};

}} // namespace lightmq { namespace protocol {

#endif  //  !MSGTRANSACTIONOPER_H_
