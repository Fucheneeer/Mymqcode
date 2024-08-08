////////////////////////////////////////////////////////////////////////////////
//
// MsgTransactionQuery.h
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
#ifndef MSGTRANSACTIONQUERY_H_
#define MSGTRANSACTIONQUERY_H_
#include "BrokerProtocolConstant.h"
#include "BrokerTypes.h"
#include "IMessage.h"
#include "MsgChunk.h"

namespace dep {	class ByteBuffer; }

namespace lightmq { namespace protocol {

class MsgTransactionQuery: public protocol::MessageBase {
	static const int KEY_QUERY_REQUEST = 1;
	static const int KEY_QUERY_RECEIPT = 2;
	static const int KEY_QUERY_REQUEST_BROKER = 3;
	static const int KEY_QUERY_BROKER_RECEIPT = 4;

public:
	struct MsgChunkTransQueryRequest: public IMsgChunk {
		bool parseFromChunkBuffer(dep::ByteBuffer &chunkBuffer, MsgChunkType chunkType, uint8_t chunkKey, uint32_t index, uint32_t multiCount) override;
		bool writeToChunkBuffer(dep::ByteBuffer &chunkBuffer, uint32_t index) override;
		virtual bool hasValue() const { return m_transID.m_transID != 0; }
		virtual void clearChunkValue() { m_transID = TransactionID(); }

		MsgChunkTransQueryRequest()
			: IMsgChunk(MsgChunkType::UserDefine,
						KEY_QUERY_REQUEST,
						MsgChunkAttr(MsgChunkAttr::IS_SMALL))
		{}

		uint64_t m_transCreateTime { 0 };
		TransactionState m_transState;
		TransactionID m_transID;
		uint32_t m_consumerAckCount { 0 };
		uint32_t m_providerSendCount { 0 };
	};

	struct MsgChunkTransQueryReceipt: public IMsgChunk {
		bool parseFromChunkBuffer(dep::ByteBuffer &chunkBuffer, MsgChunkType chunkType, uint8_t chunkKey, uint32_t index, uint32_t multiCount) override;
		bool writeToChunkBuffer(dep::ByteBuffer &chunkBuffer, uint32_t index) override;

		MsgChunkTransQueryReceipt()
			: IMsgChunk(MsgChunkType::UserDefine,
						KEY_QUERY_RECEIPT,
						MsgChunkAttr(MsgChunkAttr::IS_SMALL))
		{}

		/*uint64_t m_transCreateTime { 0 };
		TransactionState m_transState;
		TransactionID m_transID;
		uint32_t m_consumerAckCount { 0 };
		uint32_t m_providerSendCount { 0 };*/
		TransactionFinalState m_clientEndState;
	};
	
	struct MsgChunkTransQueryRequestBroker: public IMsgChunk {
		bool parseFromChunkBuffer(dep::ByteBuffer &chunkBuffer, MsgChunkType chunkType, uint8_t chunkKey, uint32_t index, uint32_t multiCount) override;
		bool writeToChunkBuffer(dep::ByteBuffer &chunkBuffer, uint32_t index) override;
		virtual bool hasValue() const { return m_transID.m_transID != 0; }
		virtual void clearChunkValue() { m_transID = TransactionID(); }

		MsgChunkTransQueryRequestBroker()
			: IMsgChunk(MsgChunkType::UserDefine,
						KEY_QUERY_REQUEST_BROKER,
						MsgChunkAttr(MsgChunkAttr::IS_SMALL))
		{}

		TransactionID m_transID;
		bool m_requestMessageInfo {false};
		bool m_requestACKInfo {false};
	};

	struct MsgChunkTransQueryBrokerReceipt: public IMsgChunk {
		bool parseFromChunkBuffer(dep::ByteBuffer &chunkBuffer, MsgChunkType chunkType, uint8_t chunkKey, uint32_t index, uint32_t multiCount) override;
		bool writeToChunkBuffer(dep::ByteBuffer &chunkBuffer, uint32_t index) override;

		MsgChunkTransQueryBrokerReceipt()
			: IMsgChunk(MsgChunkType::UserDefine,
						KEY_QUERY_BROKER_RECEIPT,
						MsgChunkAttr(MsgChunkAttr::IS_SMALL))
		{}

		uint64_t m_transCreateTime { 0 };
		TransactionState m_transState {TransactionState::NOT_A_TRANS};
		TransactionID m_transID;
		uint32_t m_consumerAckCount { 0 };
		uint32_t m_providerSendCount { 0 };
	};

public:
	MsgTransactionQuery() = default;
	virtual ~MsgTransactionQuery() = default;

	virtual uint8_t getMessageType() override {return protocol::BrokerProtocolConstant::MESSAGE_TYPE_TRANSACT_QUERY;}

	MsgChunkTransQueryRequest m_chunkRequest;
	MsgChunkTransQueryReceipt m_chunkReceipt;
	MsgChunkTransQueryRequestBroker m_chunkRequestBroker;
	MsgChunkTransQueryBrokerReceipt m_chunkBrokerReceipt;
protected:
	void regMsgChunks(MsgChunkHelper &mch, bool forParse) override;
};

}} // namespace lightmq { namespace protocol {

#endif  //  !MSGTRANSACTIONQUERY_H_
