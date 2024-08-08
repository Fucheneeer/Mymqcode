////////////////////////////////////////////////////////////////////////////////
//
// RecoveryMessageForAck.h
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

#ifndef RECOVERYMESSAGEACK_H_
#define RECOVERYMESSAGEACK_H_

#include <vector>

#include "IMessage.h"
#include "BrokerProtocolConstant.h"
#include "BrokerTypes.h"
#include "RecoveryMessage.h"

namespace lightmq { namespace protocol {

// class dep::ByteBuffer;

class RecoveryMessageAck : public protocol::IMessage {
public:
    RecoveryMessageAck() =default;
    virtual ~RecoveryMessageAck() = default;

    virtual void parse(dep::ByteBuffer& buffer) override;
    virtual void writeTo(dep::ByteBuffer& buffer) override;

    void setDestination(const std::string& destination) { m_destination = destination; }
    const std::string& getDestination() const { return m_destination; }

    void setSessionID(uint64_t sessionID) { m_sessionID = sessionID;}
    uint64_t getSessionID() const { return m_sessionID;}

    void setBeginSequence(uint64_t beginSequence) {m_beginSequence = beginSequence;}
    uint64_t getBeginSequence() const {return m_beginSequence;}

    void setEndSequence(uint64_t endSequence) {m_endSequence = endSequence;}
    uint64_t getEndSequence() const {return m_endSequence;}

    uint8_t getMessageType() override {return protocol::BrokerProtocolConstant::MESSAGE_TYPE_RECOVERY_MESSAGE_ACK;};

private:
    std::string m_destination;
    uint64_t m_sessionID;
    uint64_t m_beginSequence;
    uint64_t m_endSequence;
};

}} // namespace lightmq { namespace protocol {


#endif /* RECOVERYMESSAGEACK_H_ */
