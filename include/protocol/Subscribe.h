////////////////////////////////////////////////////////////////////////////////
//
// Subscribe.h
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

#ifndef SUBSCRIBE_H_
#define SUBSCRIBE_H_


#include <cstdint>
#include <string>

#include "BrokerProtocolConstant.h"
#include "IMessage.h"

namespace lightmq { namespace protocol {

// class dep::ByteBuffer;

class Subscribe : public protocol::IMessage {
public:
    friend class SubscribePto;

    Subscribe();
    ~Subscribe() = default;

    void parse(dep::ByteBuffer& buffer) override;

    void writeTo(dep::ByteBuffer& buffer) override;

    void setSubscriptionID(uint16_t subscriptionID) {m_subscriptionID=subscriptionID;} ;
    uint16_t getSubscriptionID() const {return m_subscriptionID;};

    void setDestination(const std::string& destination) {m_destination=destination;};
    const std::string& getDestination() const {return m_destination;};

    void setReceiptID(uint16_t receiptID) {m_receiptID=receiptID;};
    uint16_t getReceiptID() const {return m_receiptID;};

    void setAckMode(uint8_t ackMode) {m_ackMode=ackMode;};
    uint8_t getAckMode() const {return m_ackMode;};

    void setSubscriptionName(const std::string& subscriptionName) {m_subscriptionName = subscriptionName;}
    const std::string& getSubscriptionName() const {return m_subscriptionName;}

    void setAckWindowSize(uint32_t ackWindowSize) {m_ackWindowSize=ackWindowSize;}
    uint32_t getAckWindowSize() const {return m_ackWindowSize;}

    void setMessageMatcher(const std::string& messageMatcher) {m_messageMatcher = messageMatcher;}
    const std::string& getMessageMatcher() const {return m_messageMatcher;}
    const std::string& getSubNodeID() const;

	void setIsGroupSubscription(bool isGroupSubscription) { m_isGroupSubscription = isGroupSubscription; }
	bool getIsGroupSubscription() const { return m_isGroupSubscription; }

    uint8_t getMessageType() override {return protocol::BrokerProtocolConstant::MESSAGE_TYPE_SUBSCRIBE;};
private:
    uint16_t m_subscriptionID;
    std::string m_destination;
    uint16_t m_receiptID;
    uint8_t m_ackMode;
    std::string m_subscriptionName;
    uint32_t m_ackWindowSize;
    std::string m_messageMatcher;
    bool m_isGroupSubscription{ false };

};

}} // namespace lightmq { namespace protocol {


#endif /* SUBSCRIBE_H_ */
