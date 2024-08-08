////////////////////////////////////////////////////////////////////////////////
//
// Error.h
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

#ifndef ERROR_H_
#define ERROR_H_

#include "BrokerProtocolConstant.h"
#include "IMessage.h"
#include "BrokerTypes.h"

namespace lightmq { namespace protocol {

// class dep::ByteBuffer;

class Error : public protocol::IMessage {
public:

    Error();
    ~Error() = default;

    void parse(dep::ByteBuffer& buffer) override;

    void writeTo(dep::ByteBuffer& buffer) override;

    void setErrorCode(ResultCode code) { m_errorCode = code; }

    ResultCode getErrorCode() const { return m_errorCode; }

    uint8_t getMessageType() override {return protocol::BrokerProtocolConstant::MESSAGE_TYPE_ERROR;};

private:
    ResultCode m_errorCode;

};

}} // namespace lightmq { namespace protocol {

#endif /* ERROR_H_ */
