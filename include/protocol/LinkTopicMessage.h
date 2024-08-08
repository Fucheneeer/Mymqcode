////////////////////////////////////////////////////////////////////////////////
//
// LinkTopicMessage.h
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

#ifndef LINK_TOPIC_MESSAGE_H_
#define LINK_TOPIC_MESSAGE_H_

#include <cstdint>
#include <string>
#include <memory>

#include "BrokerProtocolConstant.h"
#include "Message.h"
#include "SessionScheduleUtility.h"
#include <unordered_map>

namespace lightmq { namespace protocol {

// class dep::ByteBuffer;
class LinkTopicMessage : public Message {
public:
    typedef std::function<void(uint16_t subscriptionID, SessionID sessionID, uint64_t sequence, const std::string& providerClientID, MessageID providerMessageID, dep::ByteBuffer&)> ReadBodyFunc;

    LinkTopicMessage();
    virtual ~LinkTopicMessage();

    // void setMessageProperties(std::unordered_map<uint64_t, const std::unordered_map<std::string, std::string>*>&& properties) { 
    //     m_messageProperties.clear();
    //     m_messageProperties = std::move(properties);
    // }

    virtual uint8_t getMessageType() override {return protocol::BrokerProtocolConstant::MESSAGE_TYPE_LINK_TOPIC_MESSAGE;}


    // virtual uint32_t getWriteMessageOffsetLength() const override;


    virtual void writeExtendToBuffer(dep::ByteBuffer& buffer) const override;


    virtual void readExtendFromBuffer(dep::ByteBuffer& buffer) override;


    // virtual void callReadBodyFunc(MessageSerializeAgent &msgAgent, dep::ByteBuffer& buffer) override;

    // void setReadBodyFunction(std::function<void(MessageSerializeAgent &msgAgent, const std::unordered_map<uint64_t, std::shared_ptr<std::unordered_map<std::string, std::string>>>&, dep::ByteBuffer&)>&& function) { m_readLinkTopicBodyFunction = function; }

private:
	// std::unordered_map<uint64_t, const std::unordered_map<std::string, std::string>*> m_messageProperties;
	// std::unordered_map<uint64_t, std::shared_ptr<std::unordered_map<std::string, std::string>>> m_readMessageProperties;

    //std::function<void(MessageSerializeAgent &msgAgent, const std::unordered_map<uint64_t, std::shared_ptr<std::unordered_map<std::string, std::string>>>&, dep::ByteBuffer&)> m_readLinkTopicBodyFunction;
};

}} // namespace lightmq { namespace protocol {

#endif /* LINK_QUEUE_MESSAGE_H_ */
