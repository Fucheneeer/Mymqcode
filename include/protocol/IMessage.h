////////////////////////////////////////////////////////////////////////////////
//
// IMessage.h
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

#ifndef IMESSAGE_H_
#define IMESSAGE_H_

#include "ByteBuffer.h"

namespace lightmq { namespace util{
    class LightmqMessageHead;
} }

namespace lightmq { namespace protocol {

class IMessage {
public:
    IMessage()=default;
    virtual ~IMessage()=default;
    virtual void parse(dep::ByteBuffer& buffer)=0;
    virtual void writeTo(dep::ByteBuffer& buffer)=0;
    virtual uint8_t getMessageType() = 0;
};

typedef uint32_t ReceiptID;

enum class MessageDirection: uint8_t {
	REQUEST = 'R',
	RECEIPT = 'A'
};

struct IReceiptableMessage {
	virtual void setReceiptID(ReceiptID rid) = 0;
	virtual ReceiptID getReceiptID() = 0;

	virtual void setDirection(MessageDirection msgDir) = 0;
	virtual MessageDirection getDirection() = 0;
};

}} // namespace lightmq { namespace protocol {

#endif /* IMESSAGE_H_ */
