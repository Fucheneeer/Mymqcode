////////////////////////////////////////////////////////////////////////////////
//
// ConnectAckPto.h
//
// Copyright (c) 2022 CFETSIT.
// Building 3, 491 YinDong Road, Shanghai, China.
// All rights reserved.
//
// LightMQ is the copyrighted,
// proprietary property of CFETSIT which retain all right, title and interest therein.
//
// Description:
// ConnectAck extension
////////////////////////////////////////////////////////////////////////////////

#ifndef CONNECTACKPTO_H_
#define CONNECTACKPTO_H_


#include <string>
#include <cstdint>

#include "BrokerProtocolConstant.h"
#include "IMessage.h"
#include "BrokerTypes.h"
#include "ConnectAck.h"

namespace lightmq { namespace protocol {

// class dep::ByteBuffer;

class ConnectAckPto : public protocol::IMessage {
public:
    ConnectAckPto() =default;
    ConnectAckPto(ConnectAck &ack);
    virtual ~ConnectAckPto() = default;

    virtual void parse(dep::ByteBuffer& buffer) override;
    virtual void writeTo(dep::ByteBuffer& buffer) override;

    void setClientID(const std::string& id) { m_clientID = id; }
    const std::string& getClientID() const { return m_clientID; }

    void setResultCode(ResultCode code) { m_resultCode = code; }
    ResultCode getResultCode() const { return m_resultCode; };

    void setUserID(const std::string& userID) { m_userID = userID; }
    const std::string& getUserID() const { return m_userID; }

    void setTimestamp(uint64_t timestamp) { m_timestamp = timestamp; }
    uint64_t getTimestamp() const { return m_timestamp; }

    void setRequestID(uint64_t requestID) { m_requestID = requestID; }
    uint64_t getRequestID() const { return m_requestID; }

    uint8_t getMessageType() override {return protocol::BrokerProtocolConstant::MESSAGE_TYPE_CONNECT_ACK_PTO;};

private:
    std::string     m_clientID;
    std::string     m_userID;
    ResultCode      m_resultCode{0};
    uint64_t        m_timestamp{0};  // server timestamp
    uint64_t        m_requestID{0};
};



}} // namespace lightmq { namespace protocol {


#endif /* CONNECTACKPTO_H_ */
