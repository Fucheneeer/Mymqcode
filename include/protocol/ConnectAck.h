////////////////////////////////////////////////////////////////////////////////
//
// ConnectAck.h
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

#ifndef CONNECTACK_H_
#define CONNECTACK_H_


#include <string>
#include <cstdint>

#include "BrokerProtocolConstant.h"
#include "IMessage.h"
#include "BrokerTypes.h"

namespace lightmq { namespace protocol {

// class dep::ByteBuffer;

class ConnectAck : public protocol::IMessage {
public:
    friend class ConnectAckPto;
    ConnectAck() =default;
    virtual ~ConnectAck() = default;

    virtual void parse(dep::ByteBuffer& buffer) override;
    virtual void writeTo(dep::ByteBuffer& buffer) override;

    void setClientID(const std::string& id) { m_clientID = id; }
    const std::string& getClientID() const { return m_clientID; }

    void setResultCode(ResultCode code) { m_resultCode = code; }
    ResultCode getResultCode() const { return m_resultCode; };

    void setUserID(const std::string& userID) { m_userID = userID; }
    const std::string& getUserID() const { return m_userID; }

    uint8_t getMessageType() override {return protocol::BrokerProtocolConstant::MESSAGE_TYPE_CONNECT_ACK;};

private:
    std::string m_clientID;
    std::string m_userID;
    ResultCode m_resultCode;
};



}} // namespace lightmq { namespace protocol {






#endif /* CONNECTACK_H_ */
