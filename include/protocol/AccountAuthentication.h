////////////////////////////////////////////////////////////////////////////////
//
// AccountAuthentication.h
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

#ifndef ACCOUNTAUTHENTICATION_H_
#define ACCOUNTAUTHENTICATION_H_
#include <string>

#include "IMessage.h"
#include "BrokerProtocolConstant.h"

namespace lightmq { namespace protocol {
class AccountAuthentication : public protocol::IMessage{
public:
	AccountAuthentication() = default;
	virtual ~AccountAuthentication() = default;
    void parse(dep::ByteBuffer& buffer) override;
    void writeTo(dep::ByteBuffer& buffer) override;

    void setContent(const std::string& content){m_content = content;}
    const std::string& getContent(){return m_content;}

    void setRequestID(uint64_t requestID){m_requestID = requestID;}
    uint64_t getRequestID(){return m_requestID;}

    uint8_t getMessageType(){return protocol::BrokerProtocolConstant::MESSAGE_TYPE_STANDBY_ACCOUNT_AUTHENTICATION_UPDATE;}
private:
    std::string m_content;
    uint64_t m_requestID;
};
}}

#endif /* ACCOUNTAUTHENTICATION_H_ */
