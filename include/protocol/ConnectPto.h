////////////////////////////////////////////////////////////////////////////////
//
// ConnectPto.h
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

#ifndef CONNECTPTO_H_
#define CONNECTPTO_H_


#include <string>
#include <cstdint>

#include "Connect.h"
#include "BrokerProtocolConstant.h"
#include "IMessage.h"

namespace lightmq { namespace protocol {

// class dep::ByteBuffer;

class ConnectPto : public protocol::IMessage {
public:

    ConnectPto() =default;
    virtual ~ConnectPto() = default;

    ConnectPto(Connect& connect);
    virtual void parse(dep::ByteBuffer& buffer) override;
    virtual void writeTo(dep::ByteBuffer& buffer) override;

    void setClientID(const std::string& id) { m_clientID = id; }
    const std::string& getClientID() const { return m_clientID; }

    void setHostname(const std::string& hostname) { m_hostname = hostname; }
    const std::string& getHostname() const { return m_hostname; }

    void setPID(uint32_t pid) { m_pid = pid; }
    uint32_t getPID() const { return m_pid; }

    void setUserID(const std::string& userID) { m_userID = userID; }
    const std::string& getUserID() const { return m_userID; }

    void setUserPassword(const std::string& userPassword) { m_userPassword = userPassword; }
    const std::string& getUserPassword() const { return m_userPassword; }

    void setRequestID(uint64_t requestID) { m_requestID = requestID; }
    uint64_t getRequestID() const { return m_requestID; }

    uint8_t getMessageType() override {return protocol::BrokerProtocolConstant::MESSAGE_TYPE_CONNECT_PTO;};

private:
    std::string m_clientID;
    std::string m_hostname;
    std::string m_userID;
    std::string m_userPassword;
    uint32_t    m_pid{0};
    uint64_t    m_requestID{0};

};



}} // namespace lightmq { namespace protocol {


#endif /* CONNECTPTO_H_ */
