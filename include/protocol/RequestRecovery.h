////////////////////////////////////////////////////////////////////////////////
//
// RequestRecovery.h
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

#ifndef REQUESTRECOVERY_H_
#define REQUESTRECOVERY_H_

#include <cstdint>
#include <string>
#include <memory>
#include <unordered_map>

#include "BrokerProtocolConstant.h"
#include "IMessage.h"
#include "SessionScheduleUtility.h"

namespace lightmq { namespace protocol {

using namespace lightmq::util;

// class dep::ByteBuffer;
class RequestRecovery : public protocol::IMessage {
public:

    RequestRecovery()=default;
    virtual ~RequestRecovery()=default;

    void parse(dep::ByteBuffer& buffer) override;
    void writeTo(dep::ByteBuffer& buffer) override;

    std::unordered_map<std::string, MessageID>& getAddressLastInfo() {return m_addressLastInfo;}

    uint16_t getTotalDestination() const {return m_addressLastInfo.size();}

    void addAddressInfo(const std::string& address, MessageID messageID) {m_addressLastInfo[address]=messageID;}

	ISessionScheduleUtility::MODE getSessionMode() { return m_sessionMode; }
	void setSessionMode(ISessionScheduleUtility::MODE sessionMode) { m_sessionMode = sessionMode; }

    uint8_t getMessageType() override {return protocol::BrokerProtocolConstant::MESSAGE_TYPE_REQUEST_RECOVERY;};

private:
    std::unordered_map<std::string, MessageID> m_addressLastInfo;
	ISessionScheduleUtility::MODE m_sessionMode;
};


}} // namespace lightmq { namespace protocol {


#endif /* REQUESTRECOVERY_H_ */
