////////////////////////////////////////////////////////////////////////////////
//
// RecoveryMessageSessionChange.h
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

#ifndef RECOVERYMESSAGESESSIONCHANGE_H_
#define RECOVERYMESSAGESESSIONCHANGE_H_


#include <string>

#include "BrokerProtocolConstant.h"
#include "IMessage.h"
#include "SessionScheduleUtility.h"

namespace lightmq { namespace protocol {

// class dep::ByteBuffer;

class RecoveryMessageSessionChange : public protocol::IMessage {
public:

    RecoveryMessageSessionChange() =default;
    virtual ~RecoveryMessageSessionChange() = default;

    void parse(dep::ByteBuffer& buffer) override;

    void writeTo(dep::ByteBuffer& buffer) override;

    void setOldestSessionID(SessionID sessionID) {m_oldestSessionID=sessionID;}
    SessionID getOldestSessionID() const {return m_oldestSessionID;}

    void setLatestSessionID(SessionID sessionID) {m_latestSessionID=sessionID;}
    SessionID getLatestSessionID() const {return m_latestSessionID;}


    uint8_t getMessageType() {return protocol::BrokerProtocolConstant::MESSAGE_TYPE_RECOVERY_MESSAGE_SESSION_CHANGE;};

private:
    SessionID m_latestSessionID{0};
    SessionID m_oldestSessionID{0};
};


}} // namespace lightmq { namespace protocol {


#endif /* RECOVERYMESSAGESESSIONCHANGE_H_ */
