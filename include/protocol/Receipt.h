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

#ifndef RECEIPT_H_
#define RECEIPT_H_

#include <cstdint>
#include <string>
#include <memory>

#include "BrokerProtocolConstant.h"
#include "IMessage.h"
#include "BrokerTypes.h"

namespace lightmq { namespace protocol {

// class dep::ByteBuffer;

class Receipt : public protocol::IMessage{
public:

    Receipt();
    virtual ~Receipt();

    void parse(dep::ByteBuffer& buffer) override;

    void writeTo(dep::ByteBuffer& buffer) override;


    void setReceiptID(uint16_t id) { m_receiptID = id; }
    uint16_t getReceiptID() const { return m_receiptID; }

    void setResultCode(ResultCode code) {m_resultCode=code;}
    ResultCode getResultCode() const {return m_resultCode;};

    uint8_t getMessageType() override {return protocol::BrokerProtocolConstant::MESSAGE_TYPE_RECEIPT;};

private:
    uint16_t m_receiptID;
    ResultCode m_resultCode;

};


}} // namespace lightmq { namespace protocol {


#endif /* RECEIPT_H_ */
