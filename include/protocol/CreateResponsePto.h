////////////////////////////////////////////////////////////////////////////////
//
// Receipt.h
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

#ifndef CREATERESPONSEPTO_H_
#define CREATERESPONSEPTO_H_

#include <cstdint>
#include <string>
#include <memory>

#include "BrokerProtocolConstant.h"
#include "IMessage.h"
#include "BrokerTypes.h"

namespace lightmq {
namespace protocol {

// class dep::ByteBuffer;

class CreateResponsePto : public protocol::IMessage {
public:

    CreateResponsePto() = default;
    virtual ~CreateResponsePto() = default;

    void parse(dep::ByteBuffer& buffer) override;

    void writeTo(dep::ByteBuffer& buffer) override;


    void setReceiptID(uint16_t id) { m_receiptID = id; }
    uint16_t getReceiptID() const { return m_receiptID; }

    void setResultCode(ResultCode code) { m_resultCode = code; }
    ResultCode getResultCode() const { return m_resultCode; };

    void setRequestID(uint64_t id) { m_requestID = id; };
    uint64_t getRequestID() const { return m_requestID; };

    uint8_t getMessageType() override { return protocol::BrokerProtocolConstant::MESSAGE_TYPE_CREATE_RESPONSE; };

private:
    uint16_t    m_receiptID{ 0 };
    ResultCode  m_resultCode{ 0 };
    uint64_t    m_requestID{ 0 };
};


}} // namespace lightmq { namespace protocol {


#endif /* CREATERESPONSEPTO_H_ */
