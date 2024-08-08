////////////////////////////////////////////////////////////////////////////////
//
// ISession.h
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


#ifndef IMESSAGINGSESSION_H_
#define IMESSAGINGSESSION_H_

#include <memory>

#include "IDestination.h"
#include "IMessageConsumer.h"
#include "IMessageListener.h"
#include "IMessageProducer.h"
#include "IAcknowledgeListener.h"
#include "IQueue.h"
#include "ITopic.h"
#include "IThrottler.h"
#include "LightmqMessage.h"
//#include "BrokerTypes.h"

namespace lightmq { namespace qmapi {

class ITransactionPreCommitListener;


struct TransactionInfo
{
	struct ProviderMessageItem {
		std::string destinationID;
		TransactionMessageSeq messageSeq;          //  Message sequence in Transaction
		char sendResult;   //  Message send result in client, send failed message will not commit.
	};

	struct ConsumerACKItem {
		std::string destinationID;
		MessageID messageID;
		std::string subscriptionName;
		uint16_t subscriptionId;
	};

	std::vector<ProviderMessageItem> providerMessages;
	std::vector<ConsumerACKItem> consumerACKs;

	void initTransactionInfo() {
		providerMessages.clear();
		consumerACKs.clear();
	}
};

struct BrokerTransactionStatus
{
	TransactionState transactionState;
};

class IMessagingSession {
public:
	enum MessageSendingMode : uint32_t {
		SYNCHRONOUS = 1,
		ASYNCHRONOUS
	};

	enum MessageReceivingAckMode : uint32_t {
        /**
         * Automatic acknowledge by API after the message has been processed by the application
         */
        API_AUTO_ACKNOWLEDGE = 1,

        /**
         * Acknowledgement per message by the application
         */
        APPL_INDIVIDUAL_ACKNOWLEDGE,

        /**
         * Acknowledgement per batch by the application
         */
        APPL_BATCH_ACKNOWLEDGE,
		//APPL_TRANS_ACKNOWLEDGE,
    };

	enum UserThreadingMode : uint32_t {
		SINGLE_USER_THREAD = 1,
		THREAD_PER_QUEUE_AND_TOPIC,
		MULTIPLE_THREADS
	};


public:

    virtual ~IMessagingSession() = default;

    /**
     * Creates a consumer for the specified destination and subscribe the destination
     * the created consumer is owned by the QMAPI session
     *
     * @param destination
     *      the Destination that this consumer receiving messages for.
     * @param listener
     * 		the message listener getting messages from this consumer
     * @param durableTopic
     * 		used when subscribing to a durable topic
     * @param rxAckMode
     *      the way to acknowledge received message
     *
     * @return pointer to a new consumer which is owned by this session
     *
     */
    virtual IMessageConsumer* createConsumer(const IDestination& destination, IMessageListener* listener,
            bool durableTopic = false, IMessagingSession::MessageReceivingAckMode rxAckMode = IMessagingSession::MessageReceivingAckMode::API_AUTO_ACKNOWLEDGE,
            const std::string& subscriptionName = "", const std::string& messageMatcher = "") = 0;

	/**
	 * create a consumer use key load balance (KLB).
	 * it is only adaped for Queue.
	 *
	 * @param destination
	 *      the Destination that this consumer receiving messages for.
	 * @param listener
	 * 		the message listener getting messages from this consumer
	 * @param rxAckMode
	 *      the way to acknowledge received message
	 *
	 * @return pointer to a new consumer which is owned by this session
	 */
	virtual IMessageConsumer* createKeyBalanceConsumer(const IDestination& destination, IMessageListener* listener,
		const std::string& subNodeID = "",
		IMessagingSession::MessageReceivingAckMode rxAckMode = IMessagingSession::MessageReceivingAckMode::API_AUTO_ACKNOWLEDGE) = 0;

	virtual IMessageConsumer* createGroupedTopicConsumer(const IDestination& destination, 
        IMessageListener* listener, 
        const std::string& groupName, 
        IMessagingSession::MessageReceivingAckMode rxAckMode = IMessagingSession::MessageReceivingAckMode::API_AUTO_ACKNOWLEDGE,
		const std::string& messageMatcher = "") = 0;

    /**
     * Creates a durable consumer of the specified topic and subscribe the topic
     * the created consumer is owned by the QMAPI session
     * This function is a specified function of createConsumer()
     *
     * @param destination
     *      the topic that this consumer receiving messages for.
     * @param listener
     *      message listener to receive messages
     * @param rxAckMode
     *      the way to acknowledge received message
     *
     * @return pointer to a new durable MessageConsumer that is owned by this session
     *
     */
    virtual IMessageConsumer* createDurableConsumer(const ITopic& destination, IMessageListener* listener, MessageReceivingAckMode rxAckMode = API_AUTO_ACKNOWLEDGE, const std::string& subscriptionName = "") = 0;

    /**
      * Creates a producer of the specified destination
      * the created producer is owned by the QMAPI session
      *
      * @param destination
      *      the topic to subscribe to
      * @param pListener
      *      listener to receive acknowledges of sent messages
      * @param pThrottler
      * 	 throttler for message flow control, use an internal one if set to nullptr
      *
      * @return pointer to a new producer that is owned by this session
      *
      */
    virtual IMessageProducer* createProducer(const IDestination& destination, IAcknowledgeListener* pListener = nullptr, IThrottler* pThrottler = nullptr) = 0;

    /**
     * Creates a queue
     *
     * @param queueName
     *      the name of the queue
     * @param isLocalFirst
     *      is the queue local first
     *
     * @return status
     *
     */
    virtual uint8_t createQueue(const std::string& queueName, IDestination::Persistency persistency/* = IDestination::PERSISTENT*/, bool isLocalFirst = false) = 0;

    /**
    * Creates a queue
    *
    * @param queueName
    *      the name of the queue
    * @param exclusive
    *      is the queue exclusive
    * @param isLocalFirst
    *      is the queue local first
    *
    * @return status
    *
    */
    virtual uint8_t createQueue(const std::string& queueName, bool exclusive, IDestination::Persistency persistency/* = IDestination::PERSISTENT*/, bool isLocalFirst = false) = 0;

    /**
    * Creates a queue
    *
    * @param queueName
    *      the name of the queue
    * @param exclusive
    *      is the queue exclusive
    * @param ackdMsgRetransmissionCapacity
    *      the message cap that producer keep from broker acked
    * @param isLocalFirst
    *      is the queue local first
    * @return status
    *
    */
    virtual uint8_t createQueue(const std::string& queueName, bool exclusive, uint64_t ackdMsgRetransmissionCapacity, IDestination::Persistency persistency, bool isLocalFirst = false) = 0;

    /**
     * Creates a topic
     *
     * @param topicName
     *      the name of the topic
     *
     * @return status
     *
     */
    virtual uint8_t createTopic(const std::string& topicName, IDestination::Persistency persistency/* = IDestination::PERSISTENT*/) = 0;

    /**
    * Creates a topic
    *
    * @param topicName
    *      the name of the topic
    * @param ackdMsgRetransmissionCapacity
    *      the message cap that producer keep from broker acked
    * @return status
    *
    */
    virtual uint8_t createTopic(const std::string& topicName, IDestination::Persistency persistency, uint64_t ackdMsgRetransmissionCapacity) = 0;
	 
    /**
     * Query last sent message of a destination from broker
     *
     * @param destination
     *      name of a queue/topic
     * @param messageId
     * 		returned last Message ID
     * @param message
     *		returned last message
     *
     * @return status
     *
     */
    //virtual uint8_t queryLastMessage(const std::string& destination, MessageID& messageId, dep::ByteBuffer& message) = 0;
    virtual uint8_t queryLastMessage(const std::string& destination, MessageID& messageId, LightmqMessage& message) = 0;

	/**
	 * reSend Consumed Message for consumer
	 *
	 * @param destination
	 *      name of a queue/topic
	 * @param messageids
	 * 		reSend these messageids
	 * @param subTopicClients
	 *		for topic channel,these message with be send to subTopicClients
	 *
	 * @return status
	 *
	 */
	//virtual uint8_t resendConsumedMessage(const std::string& destination, const std::string& subscriptionName, const std::vector<std::string>& messageids, const std::vector<std::string>& subTopicClients) = 0;
    /**
     * Suspend a session without disconnecting from the broker
     *
     * @return status
     */
    virtual uint8_t suspend() = 0;

    /**
     * Resume a session
     *
     * @return status
     */
    virtual uint8_t resume() = 0;

	virtual void setPreCommitListener(const std::shared_ptr<ITransactionPreCommitListener> &transListener) = 0;

	virtual std::tuple</*transSession*/bool, /*inTrans*/bool, TransactionID, /*localTransState*/TransactionState> getCurrentTransaction(TransactionInfo *info = nullptr) = 0;

	virtual uint8_t queryTransactionStatus(const TransactionID &transID, BrokerTransactionStatus *transactionStatus) = 0;

	virtual uint8_t commitLocalTransaction(TransactionState *brokerTransState = nullptr) = 0;

	virtual uint8_t rollbackLocalTransaction(TransactionState *brokerTransState = nullptr) = 0;

	virtual uint8_t preCommitTransaction(const std::shared_ptr<ITransactionPreCommitListener> &transListener, const TransactionID &transID = TransactionID()) = 0;

	virtual uint8_t commitTransaction(const TransactionID &transID) = 0;

	virtual uint8_t rollbackTransaction(const TransactionID &transID) = 0;

};

}} // namespace lightmq { namespace qmapi {



#endif /* IMESSAGINGSESSION_H_ */
