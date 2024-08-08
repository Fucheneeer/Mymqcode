////////////////////////////////////////////////////////////////////////////////
//
// MsgLocalTransaction.h
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

#ifndef MSGLOCALTRANSACTION_H_
#define MSGLOCALTRANSACTION_H_


#include <cstdint>
#include <string>
#include <memory>
#include <unordered_map>
#include <functional>

#include "BrokerProtocolConstant.h"
#include "BrokerTypes.h"
#include "IMessage.h"
#include "MsgChunk.h"

namespace dep {	class ByteBuffer; }

namespace lightmq { namespace protocol {

//  Local transaction message:
//  Length
//	type
class MsgLocalTransaction : public protocol::MessageBase {
public:

	MsgLocalTransaction()=default;
    virtual ~MsgLocalTransaction()=default;

    void parse(dep::ByteBuffer& buffer) override;

	typedef void WriteFunction(dep::ByteBuffer &buffer, int currentIndex, bool& hasNext);
	typedef bool ReadFunction (dep::ByteBuffer &buffer, uint8_t msgType);

    void setWriteBodyFunction(std::function<WriteFunction> &&function) {
		m_writeBodyFunction = function;
	}
    void setReadBodyFunction(std::function<ReadFunction>&& function) {m_readBodyFunction=function;}


	void writeTo(dep::ByteBuffer& buffer, std::function<WriteFunction>&& function) {
		setWriteBodyFunction(std::move(function));
		writeTo(buffer);
	}

	void parse(dep::ByteBuffer& buffer, std::function<ReadFunction>&& function) {
		setReadBodyFunction(std::move(function));
		parse(buffer);
	}

    uint8_t getMessageType() override {return protocol::BrokerProtocolConstant::MESSAGE_TYPE_LOCAL_TRANSACT;};

protected:
	void writeMessageBodyTo(dep::ByteBuffer& buffer) override;

private:

	void parseSubMessages(dep::ByteBuffer& buffer);

    std::function<WriteFunction> m_writeBodyFunction;
    std::function<ReadFunction> m_readBodyFunction;
};


}} // namespace lightmq { namespace protocol {

#endif /* MSGLOCALTRANSACTION_H_ */
