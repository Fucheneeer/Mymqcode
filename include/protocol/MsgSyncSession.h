////////////////////////////////////////////////////////////////////////////////
//
// MsgSyncSession.h
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
#ifndef MSGSYNCSESSION_H_
#define MSGSYNCSESSION_H_

#include <vector>
#include "BrokerProtocolConstant.h"
#include "BrokerTypes.h"
#include "IMessage.h"
#include "MsgChunk.h"

namespace dep {	class ByteBuffer; }

namespace lightmq { namespace protocol {


class MsgSyncSession : public protocol::MessageBase {
	static const int KEY_BASIC_INFO = MsgChunkInnerKey::MAX_KEY_COUNT + 0;
	static const int KEY_SESSION_LIST = MsgChunkInnerKey::MAX_KEY_COUNT + 1;

public:
	struct SessionSyncItem {
		uint8_t sessionType; //  'T' - Transaction Session
		uint8_t syncAction;  //  'S' - SYNC, 'A' - ADD, 'D' - DELETE.
		std::string sessionID;
	};

	struct MsgChunkSyncItems: public IMsgChunk {
		bool parseFromChunkBuffer(dep::ByteBuffer &chunkBuffer, MsgChunkType chunkType, uint8_t chunkKey, uint32_t index, uint32_t multiCount) override;
		bool writeToChunkBuffer(dep::ByteBuffer &chunkBuffer, uint32_t index) override;
		bool hasValue() const override { return m_sessionSyncItems.size() > 0; }
		void clearChunkValue() override { m_sessionSyncItems.clear(); }

		MsgChunkSyncItems()
				: IMsgChunk(MsgChunkType::UserDefine,
				            KEY_SESSION_LIST,
				            MsgChunkAttr(MsgChunkAttr::IS_MULTI))
		{}

		std::vector<SessionSyncItem> m_sessionSyncItems;
	};

public:
	MsgSyncSession() = default;
	virtual ~MsgSyncSession() = default;

	virtual uint8_t getMessageType() override {return protocol::BrokerProtocolConstant::MESSAGE_TYPE_SYNC_SESSION;}

	MsgChunkSyncItems m_chunkSyncItems;

protected:
	void regMsgChunks(MsgChunkHelper &mch, bool forParse) override;
};


}} // namespace lightmq { namespace protocol {

#endif  //  !MSGSYNCSESSION_H_