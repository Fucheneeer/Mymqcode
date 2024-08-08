////////////////////////////////////////////////////////////////////////////////
//
// Create.h
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

#ifndef CREATE_H_
#define CREATE_H_


#include <string>
#include <cstdint>

#include "BrokerProtocolConstant.h"
#include "IMessage.h"

namespace lightmq { namespace protocol {

// class dep::ByteBuffer;

class CreateService : public protocol::IMessage {
public:

    CreateService() =default;
    virtual ~CreateService() = default;

    virtual void parse(dep::ByteBuffer& buffer) override;

    virtual void writeTo(dep::ByteBuffer& buffer) override;

    void setReceiptID(uint16_t receiptID) {m_receiptID=receiptID;};
    uint16_t getReceiptID() const {return m_receiptID;};

    void setDestination(const std::string& destination) {m_destination=destination;};
    const std::string& getDestination() const {return m_destination;};

    uint8_t getType() const {return m_type;};
    void setType(uint8_t type) {m_type=type;};

    uint8_t getPersistency() const {return m_persistentcy;};
    void setPersistency(uint8_t persistentcy) {m_persistentcy=persistentcy;};

    bool getIsExclusive() const {return m_isExclusive;};
    void setIsExclusive(bool isExclusive) {m_isExclusive=isExclusive;};

    uint32_t getAckMsgRetxCap() const {return m_ackdMsgRetxCap;};
    void setAckdMsgRetxCap(uint32_t ackdMsgRetxCap) {m_ackdMsgRetxCap = ackdMsgRetxCap;};

    bool getIsSyncMode() const {return m_isSyncMode;};
    void setIsSyncMode(bool isSyncMode) {m_isSyncMode=isSyncMode;};

	bool getIsLocalFirst() const { return m_isLocalFirst; };
	void setIsLocalFirst(bool isLocalFirst) { m_isLocalFirst = isLocalFirst; };

    uint8_t getMessageType() override {return protocol::BrokerProtocolConstant::MESSAGE_TYPE_CREATE;};

private:
    uint16_t m_receiptID{0};
    std::string m_destination;
    uint8_t m_type{0};
    uint8_t m_persistentcy{protocol::BrokerProtocolConstant::CREATE_PERSISTENCY_PERSIST};
    bool m_isExclusive{false};
    uint32_t m_ackdMsgRetxCap{0};
    bool m_isSyncMode{false};
	bool m_isLocalFirst{ false };
};



}} // namespace lightmq { namespace protocol {


#endif /* CREATE_H_ */
