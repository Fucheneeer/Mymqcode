////////////////////////////////////////////////////////////////////////////////
//
// IMessageProducer.h
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

#ifndef IMESSAGEPRODUCER_H_
#define IMESSAGEPRODUCER_H_

#include <unordered_map>
#include <vector>
#include "ByteBuffer.h"
#include "BrokerTypes.h"
#include "IAcknowledgeListener.h"
#include "LightmqMessage.h"
#include "QMAPIConstant.h"
#include "CommonConstant.h"

namespace lightmq { namespace qmapi {

class IMessageProducer {
public:
    struct SendResultInfo
    {
        QMAPIConstant::QMAPI_RETCODE sendResult;
        MessageID messageId;
    };

    virtual ~IMessageProducer() = default;

    /**
     * Sends a message to the default producer destination, but does not take ownership
     * of the message, caller must still destroy it
     *
     * @return status
     */
    virtual uint8_t send(const LightmqMessage& message) = 0;

    /**
     * Sends a message to the default producer destination with string type message properties
     *
     * @return status
     */
    virtual uint8_t send(const LightmqMessage& message, const std::unordered_map<std::string, std::string> messageProperites) = 0;

    /**
     * Sends the message to the default producer destination, but does not take ownership
     * of the message, caller must still destroy it.
     *
     * @return status
     *
     */
    virtual uint8_t send(const std::vector<LightmqMessage*>& messages) = 0;

    virtual SendResultInfo send(const LightmqMessage& message, std::function<void(MessageID& messageID)> messageIdCallback) = 0;


	virtual uint8_t sendKeyBalanceMessage(LightmqMessage& message, const char* dispatchKey) = 0;

	virtual uint8_t sendKeyBalanceMessage(std::vector<LightmqMessage*>& messages, const char* dispatchKey) = 0;

    /**
     * Set acknowledge listener of the producer
     *
     * @param listener
     *      IAcknowledgeListener
     *
     */
    virtual void setAcknowledgementListener(IAcknowledgeListener* pListener) = 0;


    virtual void setAsync(IAcknowledgeListener* pListener = nullptr) = 0;
	virtual QMAPIConstant::QMAPI_RETCODE startGroupMessage(
            const std::string &groupName,
            uint8_t messagLevel = util::CommonConstant::DEFAULT_MESSAGE_LEVEL) =0;
	virtual QMAPIConstant::QMAPI_RETCODE commitGroupMessage(TransactionState *brokerTransState) =0;
	virtual QMAPIConstant::QMAPI_RETCODE rollbackGroupMessage(TransactionState *brokerTransState) =0;

protected:
	//virtual uint8_t sendBB(const std::vector<dep::ByteBuffer>& messages) = 0;
	//		
	//virtual uint8_t sendBB(const dep::ByteBuffer& messages) = 0;
};


}} // namespace lightmq { namespace qmapi {

#endif /* IMESSAGEPRODUCER_H_ */
