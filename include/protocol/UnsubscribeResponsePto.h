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

#ifndef UNSUBSCRIBEREPONSEPTO_H_
#define UNSUBSCRIBEREPONSEPTO_H_

#include <cstdint>
#include <string>
#include <memory>

#include "BrokerProtocolConstant.h"
#include "IMessage.h"
#include "BrokerTypes.h"

namespace lightmq {
namespace protocol {

class UnsubscribeReponsePto : public protocol::IMessage {
public:
    UnsubscribeReponsePto() = default;
    virtual ~UnsubscribeReponsePto() = default;

    void parse(dep::ByteBuffer& buffer) override;

    void writeTo(dep::ByteBuffer& buffer) override;


    void setReceiptID(uint16_t id) { m_receiptID = id; }
    uint16_t getReceiptID() const { return m_receiptID; }

    void setResultCode(ResultCode code) { m_resultCode = code; }
    ResultCode getResultCode() const { return m_resultCode; };

    void setDestination(const std::string& destination) { m_destination = destination; };
    std::string getDestination() const { return m_destination; };

    void setRequestID(uint64_t id) { m_requestID = id; };
    uint64_t getRequestID() const { return m_requestID; };

    uint8_t getMessageType() override { return protocol::BrokerProtocolConstant::MESSAGE_TYPE_UNSUBSCRIBE_RESPONSE; };

private:
    uint16_t        m_receiptID{ 0 };
    ResultCode      m_resultCode{ 0 };
    std::string     m_destination;
    uint64_t        m_requestID{ 0 };
};


}} // namespace lightmq::protocol


#endif /* UNSUBSCRIBEREPONSEPTO_H_ */
