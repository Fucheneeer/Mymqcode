////////////////////////////////////////////////////////////////////////////////
//
// ConsumerRecoveryAck.h
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

#ifndef RECOVERYCONSUMERACK_H_
#define RECOVERYCONSUMERACK_H_

#include <string>
#include <memory>
#include <vector>

#include "IMessage.h"
#include "BrokerTypes.h"
#include "BrokerProtocolConstant.h"

namespace lightmq { namespace protocol {


class RecoveryConsumerAck : public protocol::IMessage {
public:
    struct EVENT {
        EVENT() = default;
        EVENT(uint8_t type, SessionID sessID, uint64_t startSeq, uint64_t endSeq) : eventType(type), sessionID(sessID), fromSeq(startSeq), toSeq(endSeq) {

        }
        uint8_t eventType{0};
        SessionID sessionID{0};
        uint64_t fromSeq{0};
        uint64_t toSeq{0};
    };
    RecoveryConsumerAck() =default;
    virtual ~RecoveryConsumerAck() = default;

    void parse(dep::ByteBuffer& buffer) override;

    void writeTo(dep::ByteBuffer& buffer) override;

    void setIsSyncMode(bool isSyncMode) {m_isSyncMode = isSyncMode;}
    bool getIsSyncMode() const {return m_isSyncMode;}

    void setDestination(const std::string& destination) { m_destination = destination; }
    const std::string& getDestination() const { return m_destination; }

    void setIndexFileName(const std::string& indexFileName) { m_indexFileName = indexFileName; }
    const std::string& getIndexFileName() const { return m_indexFileName; }

    void setClientID(const std::string& clientID) {m_clientID=clientID;};
    const std::string& getClientID() const {return m_clientID;};

    void setClientName(const std::string& clientName) {m_clientName=clientName;};
    const std::string& getClientName() const {return m_clientName;};

    void setMessageCount(uint32_t count) {m_messageCount=count;};
     uint32_t getMessageCount() const {return m_messageCount;};

    void setRequestID(uint32_t requestID) {m_requestID=requestID;};
    uint32_t getRequestID() const {return m_requestID;};

    void setIsSequential(bool isSequential) {m_isSequential=isSequential;}
    bool getIsSequential() {return m_isSequential;}

    void addEvent(uint8_t eventType, SessionID sessionID, uint64_t fromSeq, uint64_t toSeq) {m_eventVector.push_back(EVENT(eventType, sessionID, fromSeq, toSeq));}
    const std::vector<EVENT>& getEventVector() const {return m_eventVector;};
    void addSequentialEvent(uint8_t level, SessionID sessionID, uint64_t seq) {m_sequentialEvent.push_back(EVENT(level, sessionID, seq, seq));}
    const std::vector<EVENT>& getSequentialEventVector() const {return m_sequentialEvent;};


    virtual uint8_t getMessageType() override {return protocol::BrokerProtocolConstant::MESSAGE_TYPE_RECOVERY_CONSUMER_ACK;};
private:
    bool m_isSyncMode{0};
    std::string m_destination;
    std::string m_indexFileName;
    std::string m_clientID;
    std::string m_clientName;
    uint32_t m_messageCount{0};
    uint32_t m_requestID{0};
    bool m_isSequential{false};
    std::vector<EVENT> m_eventVector;
    // sequential event中, EVENT::evenType 代表消息级别
    std::vector<EVENT> m_sequentialEvent; // 
    
};


class RecoveryConsumerAckAck : public protocol::IMessage {
public:
    RecoveryConsumerAckAck()=default;
    ~RecoveryConsumerAckAck()=default;

    void parse(dep::ByteBuffer& buffer) override;
    void writeTo(dep::ByteBuffer& buffer) override;

    void setIsSyncMode(bool isSyncMode) {m_isSyncMode = isSyncMode;}
    bool getIsSyncMode() const {return m_isSyncMode;}

    void setRequestID(uint32_t requestID) {m_requestID=requestID;};
    uint32_t getRequestID() const {return m_requestID;};

    uint8_t getMessageType() override {return protocol::BrokerProtocolConstant::MESSAGE_TYPE_RECOVERY_CONSUMER_ACK_ACK;};

private:
    bool m_isSyncMode{0};
    uint32_t m_requestID{0};
};

}} // namespace lightmq { namespace protocol {

#endif /* RECOVERYCONSUMERACK_H_ */
