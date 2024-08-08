////////////////////////////////////////////////////////////////////////////////
//
// MsgChunk.h
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
#ifndef PROTOCOL_MSGCHUNK_H_
#define PROTOCOL_MSGCHUNK_H_
#include <functional>
#include "BrokerProtocolConstant.h"
#include "BrokerTypes.h"
#include "IMessage.h"

namespace dep {	class ByteBuffer; }

namespace lightmq { namespace protocol {

enum class MsgChunkType: uint8_t {
	//Table = 'T',
	//Properties = 'P',
	UserDefine = 'U',
	Inner = 'N',
	//SubMessages = 'M',

	//  never modify this for compitable. if chunk type is not passed, it will be DEFAULT
	//  UserDefine is simular to C struct.
	DEFAULT = UserDefine
};

namespace MsgChunkFlag {
	const uint8_t CHUNK_TYPE  = 0x01;
	const uint8_t CHUNK_KEY   = 0x02;
	const uint8_t CHUNK_ARRAY = 0x04;
	const uint8_t CHUNK_SMALL = 0x08;
}

namespace MsgChunkInnerKey {
	const uint8_t KEY_RECEIPT_HEAD = 1;
	const uint8_t KEY_ERROR = 2;
	const uint8_t MAX_KEY_COUNT = 32;
	const uint8_t MAX_KEY = MAX_KEY_COUNT - 1;
}

struct MsgChunkAttr {
	static const uint8_t IS_MULTI = MsgChunkFlag::CHUNK_ARRAY;
	static const uint8_t IS_SMALL = MsgChunkFlag::CHUNK_SMALL;

	inline explicit MsgChunkAttr(uint8_t attr = 0)
		: m_attr(attr)
	{}

	inline bool isMulti() const {
		return m_attr & IS_MULTI;
	}

	inline bool isSmallChunk() const {
		return m_attr & IS_SMALL;
	}

	uint8_t m_attr;
};

struct IMsgChunk {
	virtual bool parseFromChunkBuffer(dep::ByteBuffer &chunkBuffer, MsgChunkType chunkType, uint8_t chunkKey, uint32_t index, uint32_t multiCount) = 0;
	virtual bool writeToChunkBuffer(dep::ByteBuffer &chunkBuffer, uint32_t index) = 0;

	inline MsgChunkType getChunkType() const { return m_chunkType; }
	inline uint8_t getChunkKey() const { return m_chunkKey; }
	inline MsgChunkAttr getChunkAttr() const { return m_chunkAttr; }

	virtual bool hasValue() const { return m_hasValue; }
	virtual void clearChunkValue() { m_hasValue = false; }
	inline void setHasValue(bool hasValue = true) { m_hasValue = hasValue; }

protected:
	IMsgChunk(MsgChunkType chunkType, uint8_t chunkKey, MsgChunkAttr attr)
		: m_chunkType(chunkType)
		, m_chunkKey(chunkKey)
		, m_chunkAttr(attr)
	{}

	virtual ~IMsgChunk() = default;

	MsgChunkType m_chunkType;
	uint8_t m_chunkKey;
	MsgChunkAttr m_chunkAttr;
	bool m_hasValue { false };
};

struct MsgChunkReceiptHead: public IMsgChunk {
	bool parseFromChunkBuffer(dep::ByteBuffer &chunkBuffer, MsgChunkType chunkType, uint8_t chunkKey, uint32_t index, uint32_t multiCount) override;
	bool writeToChunkBuffer(dep::ByteBuffer &chunkBuffer, uint32_t index) override;

	MsgChunkReceiptHead()
		: IMsgChunk(MsgChunkType::Inner,
					MsgChunkInnerKey::KEY_RECEIPT_HEAD,
					MsgChunkAttr(MsgChunkAttr::IS_SMALL))
	{}

	uint32_t m_receiptID { 0 };
	MessageDirection  m_messageDirection { MessageDirection::REQUEST };  //  'R' - Request, 'A' - Receipt(Answer/ACK)
};

struct MsgChunkError: public IMsgChunk {
	bool parseFromChunkBuffer(dep::ByteBuffer &chunkBuffer, MsgChunkType chunkType, uint8_t chunkKey, uint32_t index, uint32_t multiCount) override;
	bool writeToChunkBuffer(dep::ByteBuffer &chunkBuffer, uint32_t index) override;
	bool hasValue() const override { return m_errorCode != BrokerProtocolConstant::RESULT_CODE_NO_ERROR; }

	MsgChunkError()
			: IMsgChunk(MsgChunkType::Inner,
			            MsgChunkInnerKey::KEY_ERROR,
			            MsgChunkAttr(MsgChunkAttr::IS_SMALL))
	{}

	uint8_t m_errorCode { 0 };
	std::string m_errorDesc;
};


class MsgChunkHelper {
public:
	//static void emptyParseHandleFunc(dep::ByteBuffer &buffer, MsgChunkType chunkType, uint8_t chunkKey, uint32_t index, uint32_t multiCount) {}
	//static bool emptyParseExitFunc(MsgChunkType chunkType, uint8_t chunkKey, uint32_t index, uint32_t multiCount) { return false; }

	typedef void ParseHandleFunc (dep::ByteBuffer &buffer, MsgChunkType chunkType, uint8_t chunkKey, uint32_t index, uint32_t multiCount);
	typedef bool ParseExitFunc (MsgChunkType chunkType, uint8_t chunkKey, uint32_t index, uint32_t multiCount);

	static const std::function<ParseHandleFunc> &emptyParseHandleFunc;
	static const std::function<ParseExitFunc> &emptyParseExitFunc;

	void parseBufferToChunks(dep::ByteBuffer &buffer, 
	                         const std::function<ParseExitFunc> &exitFunc = emptyParseExitFunc,
	                         const std::function<ParseHandleFunc> &f = emptyParseHandleFunc) const;
	void writeChunk(dep::ByteBuffer &buffer, MsgChunkType chunkType, uint8_t chunkKey, MsgChunkAttr chunkAttr, const std::function<bool (dep::ByteBuffer &buffer, uint32_t index)> &f) const;
	void writeChunk(dep::ByteBuffer &buffer, IMsgChunk *msgChunk) const;
	void writeAllChunks(dep::ByteBuffer &buffer) const;

	void regMsgChunk(IMsgChunk *msgChunk, bool forParse)
	{
		m_innerChunks[m_innerChunkCount++] = msgChunk;
		if (forParse)
			msgChunk->clearChunkValue();
	}

	void regInnerChunk(IMsgChunk *msgChunk, bool forParse)
	{
		m_innerChunks[m_innerChunkCount++] = msgChunk;
		if (forParse)
			msgChunk->clearChunkValue();
	}

private:
	void onParseChunkBuffer(const std::function<void (dep::ByteBuffer &buffer, MsgChunkType chunkType, uint8_t chunkKey, uint32_t index, uint32_t multiCount)> &f,
	                        dep::ByteBuffer &chunkBuffer, MsgChunkType chunkType, uint8_t chunkKey, uint32_t index, uint32_t multiCount) const;

	static const int MAX_INNER_CHUNKS = MsgChunkInnerKey::MAX_KEY_COUNT;
	static const int MAX_USER_CHUNKS = 128;
	static const int MAX_CHUNKS = MAX_INNER_CHUNKS + MAX_USER_CHUNKS;
	IMsgChunk *m_innerChunks[MAX_CHUNKS];
	int m_innerChunkCount { 0 };
};


class MessageBase : public protocol::IMessage, public IReceiptableMessage {
public:
	MessageBase() = default;
	virtual ~MessageBase() = default;
	virtual void parse(dep::ByteBuffer &buffer) override;
	virtual void writeTo(dep::ByteBuffer &buffer) override;

public:  //  IReceiptableMessage
	void setReceiptID(ReceiptID rid) override {
		m_receiptHead.m_receiptID = rid;
	}

	ReceiptID getReceiptID() override {
		return m_receiptHead.m_receiptID;
	}

	void setDirection(MessageDirection msgDir) override {
		m_receiptHead.m_messageDirection = msgDir;
		m_receiptHead.setHasValue();
	}

	MessageDirection getDirection() override {
		return m_receiptHead.m_messageDirection;
	}

	lightmq::ResultCode getResultCode() const {
		if (m_receiptError.hasValue())
			return m_receiptError.m_errorCode;
		else
			return BrokerProtocolConstant::RESULT_CODE_NO_ERROR;
	}

	void setResultCode(lightmq::ResultCode resultCode) {
		m_receiptError.m_errorCode = resultCode;
	}

	void makeResponseMessage(const MessageBase &m) {
		m_receiptHead.m_messageDirection = MessageDirection::RECEIPT;
		m_receiptHead.m_receiptID = m.m_receiptHead.m_receiptID;
		m_receiptHead.setHasValue();
	}

protected:

	static const std::function<MsgChunkHelper::ParseExitFunc> &exitOnReceiptHead;

	MsgChunkReceiptHead m_receiptHead;
	MsgChunkError m_receiptError;
	virtual void regMsgChunks(MsgChunkHelper &mch, bool forParse) = 0;
	virtual void writeMessageBodyTo(dep::ByteBuffer& buffer);

	void regInnerChunks(MsgChunkHelper &mch, bool forParse) {
		mch.regInnerChunk(&m_receiptHead, forParse);
		mch.regInnerChunk(&m_receiptError, forParse);
	}
};


}} // namespace lightmq { namespace protocol {

#endif // !PROTOCOL_MSGCHUNK_H_
