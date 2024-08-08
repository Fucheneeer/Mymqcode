////////////////////////////////////////////////////////////////////////////////
//
// AccountAuthenticationAck.h
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

#ifndef ACCOUNTAUTHENTICATIONACK_H_
#define ACCOUNTAUTHENTICATIONACK_H_
#include "IMessage.h"
#include "BrokerProtocolConstant.h"

namespace lightmq { namespace protocol {
class AccountAuthenticationAck : public protocol::IMessage{
public:
	AccountAuthenticationAck() = default;
	virtual ~AccountAuthenticationAck() = default;
    void parse(dep::ByteBuffer& buffer) override;
    void writeTo(dep::ByteBuffer& buffer) override;

    void setRetCode(uint32_t retCode){m_retCode = retCode;}
    uint32_t getRetCode(){return m_retCode;}

    void setRequestID(uint64_t requestID){m_requestID = requestID;}
    uint64_t getRequestID(){return m_requestID;}

    uint8_t getMessageType(){return protocol::BrokerProtocolConstant::MESSAGE_TYPE_STANDBY_ACCOUNT_AUTHENTICATION_UPDATE_ACK;}
private:
    uint32_t m_retCode;
    uint64_t m_requestID;
};
}}

#endif /* ACCOUNTAUTHENTICATIONACK_H_ */
