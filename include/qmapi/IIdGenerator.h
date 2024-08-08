////////////////////////////////////////////////////////////////////////////////
//
// IIdGenerator.h
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


#ifndef IIDGENERATOR_H_
#define IIDGENERATOR_H_

#include "BrokerMessage.h"

namespace lightmq { namespace qmapi {

class IIdGenerator {
public:
	virtual ~IIdGenerator() = default;

    virtual uint16_t getSubscriptionId() = 0;
    virtual uint16_t getReceiptId() = 0;

    // get MessageID by destination, and advance MessageID from the number of messages
    virtual MessageID getMessageID(const std::string& destination, int32_t messageCount = 1) = 0;
    // rollback messageID by the number of messages
    virtual void rollbackMessageID(const std::string& destination, int32_t messageCount = 1) = 0;


    virtual MessageID getMessageIDNoAdvance(const std::string& destination) = 0;
    virtual bool getLastMessage(uint16_t receiptId, MessageID& messageId, dep::ByteBuffer& message) = 0;

    // get MessageID by receiptID and initialize the (destination=>messageID) map
    virtual MessageID initWithBrokerMessageID(const std::string& destination, uint16_t receiptId) = 0;
    virtual void initFailoverMessageID(const std::string& destination, MessageID& msgId) = 0;

    virtual std::recursive_mutex& getApiMutex() = 0;
};


}} // namespace lightmq { namespace qmapi {


#endif /* IIDGENERATOR_H_ */
