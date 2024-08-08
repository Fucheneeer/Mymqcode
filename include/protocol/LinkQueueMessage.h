////////////////////////////////////////////////////////////////////////////////
//
// LinkQueueMessage.h
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

#ifndef LINK_QUEUE_MESSAGE_H_
#define LINK_QUEUE_MESSAGE_H_

#include <cstdint>
#include <string>
#include <memory>

#include "BrokerProtocolConstant.h"
#include "Message.h"
#include "SessionScheduleUtility.h"

namespace lightmq { namespace protocol {

// class dep::ByteBuffer;
class LinkQueueMessage : public Message {
public:

    LinkQueueMessage();
    virtual ~LinkQueueMessage();

    void setRemoteClientID(const std::string& remoteClientID) { m_remoteClientID = remoteClientID; }

    const std::string& getRemoteClientID() { return m_remoteClientID; }

    virtual uint8_t getMessageType() override {return protocol::BrokerProtocolConstant::MESSAGE_TYPE_LINK_QUEUE_MESSAGE;}


    virtual uint32_t getWriteMessageOffsetLength() const override;


    virtual void writeExtendToBuffer(dep::ByteBuffer& buffer) const override;


    virtual void readExtendFromBuffer(dep::ByteBuffer& buffer) override;

private:
    std::string m_remoteClientID;
};

}} // namespace lightmq { namespace protocol {

#endif /* LINK_QUEUE_MESSAGE_H_ */
