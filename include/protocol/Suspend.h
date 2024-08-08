////////////////////////////////////////////////////////////////////////////////
//
// Suspend.h
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

#ifndef SUSPEND_H_
#define SUSPEND_H_

#include <cstdint>
#include <string>
#include <memory>
#include <vector>

#include "BrokerProtocolConstant.h"
#include "IMessage.h"
#include "BrokerTypes.h"

namespace lightmq { namespace protocol {

// class dep::ByteBuffer;

class Suspend : public protocol::IMessage{
public:
    enum StatType{
        NORMAL = 0,
        SUSPEND
    };
    Suspend();
    virtual ~Suspend();

    void parse(dep::ByteBuffer& buffer) override;

    void writeTo(dep::ByteBuffer& buffer) override;

    void setDestinations(const std::vector<std::string>& destinations) { m_destinations = destinations; }
    const std::vector<std::string>& getDestinations() const { return m_destinations; }

    void setStat(uint8_t stat) { m_stat = stat; }
    uint8_t getStat() const { return m_stat; }

    uint8_t getMessageType() override {return protocol::BrokerProtocolConstant::MESSAGE_TYPE_SUSPEND_PRODUCER;};

private:
    std::vector<std::string> m_destinations;
    uint8_t m_stat;
};


}} // namespace lightmq { namespace protocol {


#endif /* SUSPEND_H_ */
