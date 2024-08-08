////////////////////////////////////////////////////////////////////////////////
//
// SubscribePto.h
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


#include <cstdint>
#include <string>

#include "BrokerProtocolConstant.h"
#include "IMessage.h"
#include "Subscribe.h"

namespace lightmq {
namespace protocol {

// class dep::ByteBuffer;

class SubscribePto : public protocol::IMessage {
public:
    SubscribePto() = default;
    SubscribePto(Subscribe& sub);
    ~SubscribePto() = default;

    void parse(dep::ByteBuffer& buffer) override;

    void writeTo(dep::ByteBuffer& buffer) override;

    void setSubscriptionID(uint16_t subscriptionID) { m_subscriptionID = subscriptionID; };
    uint16_t getSubscriptionID() const { return m_subscriptionID; };

    void setDestination(const std::string& destination) { m_destination = destination; };
    const std::string& getDestination() const { return m_destination; };

    void setReceiptID(uint16_t receiptID) { m_receiptID = receiptID; };
    uint16_t getReceiptID() const { return m_receiptID; };

    void setAddressType(uint8_t addressType) { m_addressType = addressType; };
    uint8_t getAddressType() const { return m_addressType; };

    void setAckMode(uint8_t ackMode) { m_ackMode = ackMode; };
    uint8_t getAckMode() const { return m_ackMode; };

    void setSubscriptionName(const std::string& subscriptionName) { m_subscriptionName = subscriptionName; }
    const std::string& getSubscriptionName() const { return m_subscriptionName; }

    void setAckWindowSize(uint32_t ackWindowSize) { m_ackWindowSize = ackWindowSize; }
    uint32_t getAckWindowSize() const { return m_ackWindowSize; }

    void setMessageMatcher(const std::string& messageMatcher) { m_messageMatcher = messageMatcher; }
    const std::string& getMessageMatcher() const { return m_messageMatcher; }
    void setSubNodeID(const std::string& nodeID) { m_subNodeID = nodeID; }
    const std::string& getSubNodeID() const { return m_subNodeID; };

    void setIsGroupSubscription(bool isGroupSubscription) { m_isGroupSubscription = isGroupSubscription; }
    bool getIsGroupSubscription() const { return m_isGroupSubscription; }

    void setMessageListener(const std::string& messageListener) { m_messageListener = messageListener; }
    const std::string& getMessageListener() const { return m_messageListener; }

    void setRequestID(uint64_t id) { m_requestID = id; };
    uint64_t getRequestID() const { return m_requestID; };

    void setTransAckCacheSize(uint32_t transCacheSize) {m_transAckCacheSize = transCacheSize;}
    uint32_t getTransAckCacheSize() const {return m_transAckCacheSize;}

    uint8_t getMessageType() override { return protocol::BrokerProtocolConstant::MESSAGE_TYPE_SUBSCRIBE_PTO; };

private:
    uint16_t            m_subscriptionID{ 0 };
    std::string         m_destination;
    uint16_t            m_receiptID{ 0 };

    uint8_t             m_addressType;
    uint8_t             m_ackMode{ 0 };
    std::string         m_subscriptionName;
    uint32_t            m_ackWindowSize{ 0 };
    std::string         m_messageMatcher;
    std::string         m_subNodeID;
    bool                m_isGroupSubscription{ false };
    std::string         m_messageListener;  //httpÕ∆ÀÕ”√
    uint64_t            m_requestID{ 0 };
    uint32_t            m_transAckCacheSize{0};
};

}}
