////////////////////////////////////////////////////////////////////////////////
//
// IMessageConsumer.h
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

#ifndef IMESSAGECONSUMER_H_
#define IMESSAGECONSUMER_H_

#include "IMessageListener.h"
#include "BrokerTypes.h"
#include "BrokerMessage.h"

namespace lightmq { namespace qmapi {

class LightmqMessage;
class IMessageConsumer {
public:
	virtual ~IMessageConsumer() = default;

	/**
     *
     * Acknowledge received message individually
     *
     * @param destination
     *  	Destination of this acknowledge corresponds to
     * @param messageId
     * 		Message ID to acknowledge
     *
     */
    virtual bool sendAcknowledge(const std::string& destination, const MessageID& messageId) = 0;

    /**
     *
     * Acknowledge received message by batch
     *
     * @param destination
     *  	Destination of this acknowledge corresponds to
     * @param startMessageId
     * 		starting Message ID to acknowledge
     * @param endMessageId
     * 		ending Message ID to acknowledge
     */
    virtual bool sendAcknowledge(const std::string& destination, const MessageID& startMessageId, const MessageID& endMessageId) = 0;

    /**
     * Set message listener of the consumer
     *
     * @param listner
     *      IMessageListener
     *
     */
    virtual void setMessageListener(IMessageListener* listener) = 0;

    /**
     * Remove message listener of the consumer
     *
     *
     */
    virtual void removeMessageListener() = 0;

    /**
     * unsubscribe the channel
     *
     *
     */
    virtual void unsubscribe() = 0;

    /**
        *unsubscribe the channel
        *
        *
    */
    virtual void resubscribe() = 0;

    virtual uint8_t receive(int milliTimeout, LightmqMessage &lmqMessage) =0;
};

}} // namespace lightmq { namespace qmapi {

#endif /* IMESSAGECONSUMER_H_ */
