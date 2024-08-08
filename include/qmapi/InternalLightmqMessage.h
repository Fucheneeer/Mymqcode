////////////////////////////////////////////////////////////////////////////////
//
// InternalLightmqMessage.h
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
#pragma once
#include <memory>
#include "ByteBuffer.h"
#include "LightmqMessageHead.h"
#include "BrokerTypes.h"

namespace lightmq { namespace qmapi {

class LightmqMessage;
class InternalLightmqMessage;
typedef std::unique_ptr<InternalLightmqMessage> InternalMessagePtr;

class InternalLightmqMessage
{
public:
    InternalLightmqMessage();
    InternalLightmqMessage(const LightmqMessage &msg);
    InternalLightmqMessage(dep::ByteBuffer &buf);

    dep::ByteBuffer* getFinalBuffer(); 
    void setFinalBuffer(dep::ByteBuffer* buf);

    size_t size() const;
    InternalMessagePtr makeCopy() const;
    util::LightmqMessageHead& getHead();

    size_t writeTo(dep::ByteBuffer &buf) const;
    void toLightmqMessage(LightmqMessage &msg) const;

    bool checkFinalChanged() const;
    void checkKeepData();
private:
    void buildInternalBuffer(const LightmqMessage &msg);

private:
    dep::ByteBuffer m_internalBuffer; // read only
    dep::ByteBuffer *m_finalBuffer;
	util::LightmqMessageHead m_lightmqMessageHead;
    dep::ByteBuffer m_keepData;
};


}}
