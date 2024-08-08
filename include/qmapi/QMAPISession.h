////////////////////////////////////////////////////////////////////////////////
//
// QMAPISession.h
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


#ifndef QMAPISESSION_H_
#define QMAPISESSION_H_


#include <iostream>
#include <memory>
#include <vector>
#include <map>
#include "ISelector.h"
#include "IMessagingSession.h"
#include "IAckReceivedMsgSender.h"
#include "QMAPISelectableClientSocket.h"
#include "QMAPITypes.h"
#include "QMAPIConfig.h"
#include "IIdGenerator.h"
#include "MsgDispatchThread.h"
#include "QMAPIQueue.h"
#include "QMAPITopic.h"
#include "QMAPIConstant.h"
#include "QMAPIMessageProducer.h"
#include "QMAPIMessageConsumer.h"

namespace lightmq { namespace qmapi {

class TransactionSingleManager;
class QMAPIConnection;

class QMAPISession : public IMessagingSession/*, public IAckReceivedMsgSender*/ {
	friend class QMAPIConnection;
public:
    class CvResult {
    public:
        std::condition_variable* pCV;
        ResultCode result;
		std::mutex mtxSend;
    };

	static constexpr ResultCode NA_RESULTCODE = 12345;
	static constexpr auto WILDCARD = "*";
    static constexpr uint16_t MAX_SUBNODE_LENGTH = 128;

    class SessionStatus {
    public:
        uint32_t numberOfProducers;
        uint32_t numberOfConsumers;
        bool isSuspended;
        IMessagingSession::UserThreadingMode userThreadMode;
        std::vector<QMAPIMessageProducer::ProducerStatus> producerStatusVector;
        std::vector<QMAPIMessageConsumer::ConsumerStatus> consumerStatusVector;
    };

    class ServiceAttr {
    public:
        IDestination::Type type;
        bool isExclusive;
        IDestination::Persistency persistencyType;
        uint64_t ackdMsgRetransmissionCapacity;
		bool localFirst;
		bool isSyncMode;
    };


public:
	using RECEIPTID = uint16_t;


public:
    virtual ~QMAPISession() override;
	QMAPISession() = delete;

    /**
     * Creates a session which is owned by the caller.
     *
     * @param config
     * 		QMAPI configuration
     * @param pSocket
     *  	TCP socket that is connected to a broker
     * @param clientId
     * 		Client ID
     * @param sessionSeqId
     * 		Internal Session Sequence ID
     * @param sendingMode
     * 		synchronous or asynchonous message sending mode
     * @param ackMode
     * 		API auto ack, individual ack by application, or batch ack by application
     * @param threadMode
     * 		single thread, thread per queue/topic or multithreaded mode
     * @param idGenerator
     * 		ID Generator to supply Receipt ID, Subscription ID and Message ID
     * @param manager
     *		Failover manager
     *
     * @return A pointer to a connection object
     *
     */
    //static std::unique_ptr<QMAPISession> getInstance(QMAPIConnection &connection, const QMAPIConfig& config, QMAPISelectableClientSocket* pSocket, const std::string& clientId, IMessagingSession::UserThreadingMode threadMode, IIdGenerator& idGenerator, IConnectionManager& manager, bool isTransactSession = false);
    static std::unique_ptr<QMAPISession> getInstance(QMAPIConnection &connection,
	                                                 const QMAPIConfig& config,
	                                                 IMessagingSession::UserThreadingMode,
	                                                 bool isTransactSession = false,
	                                                 const std::string &transSessionID = "",
	                                                 const std::shared_ptr<ITransactionSessionListener> &transListener = nullptr);

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
     *
     * @return pointer to a new consumer which is owned by this session
     *
     */
    virtual IMessageConsumer* createConsumer(const IDestination& destination, IMessageListener* listener, bool durableTopic = false, 
            IMessagingSession::MessageReceivingAckMode rxAckMode = IMessagingSession::MessageReceivingAckMode::API_AUTO_ACKNOWLEDGE,
            const std::string& subscriptionName = "", const std::string& messageMatcher = "") override;

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
	virtual IMessageConsumer* createKeyBalanceConsumer(const IDestination& destination, IMessageListener* listener, const std::string& subNodeID = "",
		IMessagingSession::MessageReceivingAckMode rxAckMode = IMessagingSession::MessageReceivingAckMode::API_AUTO_ACKNOWLEDGE) override;

	IMessageConsumer* createConsumerIntenal(const IDestination& destination, 
        IMessageListener* listener, 
        bool durableTopic,
		IMessagingSession::MessageReceivingAckMode rxAckMode,
		const std::string& subscriptionName, 
        const std::string& messageMatcher,
        const std::string& subNodeID,
        bool isGroupedSubscription);

    /**
     * Creates a durable consumer of the specified topic and subscribe the topic
     * the created consumer is owned by the QMAPI session
     * This function is a specified function of createConsumer()
     *
     * @param destination
     *      the topic that this consumer receiving messages for.
     * @param listener
     *      message listener to receive messages
     * @return pointer to a new durable MessageConsumer that is owned by this session
     *
     */
    IMessageConsumer* createDurableConsumer(const ITopic& destination, IMessageListener* listener, IMessagingSession::MessageReceivingAckMode = IMessagingSession::MessageReceivingAckMode::API_AUTO_ACKNOWLEDGE, const std::string& subscriptionName = "") override;

    virtual IMessageConsumer* createGroupedTopicConsumer(const IDestination& destination, IMessageListener* listener, const std::string& subscriptionGroupName, IMessagingSession::MessageReceivingAckMode rxAckMode = IMessagingSession::MessageReceivingAckMode::API_AUTO_ACKNOWLEDGE, const std::string& messageMatcher = "") override;

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
    IMessageProducer* createProducer(const IDestination& destination, IAcknowledgeListener* pListener = nullptr, IThrottler* pThrottler = nullptr) override;

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
    QMAPIConstant::QMAPI_RETCODE createQueue(const std::string& queueName, IDestination::Persistency persistency, bool isLocalFirst = false) override;

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
    virtual uint8_t createQueue(const std::string& queueName, bool exclusive, IDestination::Persistency persistency, bool isLocalFirst = false) override;

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
    virtual uint8_t createQueue(const std::string& queueName, bool exclusive, uint64_t ackdMsgRetransmissionCapacity, IDestination::Persistency persistency, bool isLocalFirst = false) override;

    /**
     * Creates a topic
     *
     * @param topicName
     *      the name of the topic
     *
     * @return status
     *
     */
    virtual QMAPIConstant::QMAPI_RETCODE createTopic(const std::string& topicName, IDestination::Persistency persistency) override;

	//virtual QMAPIConstant::QMAPI_RETCODE resendConsumedMessage(const std::string& destination, const std::string& subscriptionName, const std::vector<std::string>& messageids, const std::vector<std::string>& subTopicClients) override;

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
    virtual uint8_t createTopic(const std::string& topicName, IDestination::Persistency persistency, uint64_t ackdMsgRetransmissionCapacity = 0) override;

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
    QMAPIConstant::QMAPI_RETCODE queryLastMessage(const std::string& destination, MessageID& messageId, LightmqMessage& message) override;

    /**
     * Suspend all producers and consumers of the session
     *
     * @return status
     */
    QMAPIConstant::QMAPI_RETCODE suspend() override;

    /**
     * Resume all producers and consumers of the session
     *
     * @return status
     */
    QMAPIConstant::QMAPI_RETCODE resume() override;


	/* transaction operates */

	QMAPIConstant::QMAPI_RETCODE commitLocalTransaction(TransactionState *brokerTransState = nullptr) override;

	QMAPIConstant::QMAPI_RETCODE rollbackLocalTransaction(TransactionState *brokerTransState = nullptr) override;

	QMAPIConstant::QMAPI_RETCODE preCommitTransaction(const std::shared_ptr<ITransactionPreCommitListener> &transListener, const TransactionID &transID) override;

	QMAPIConstant::QMAPI_RETCODE commitTransaction(const TransactionID &transID) override;

	QMAPIConstant::QMAPI_RETCODE rollbackTransaction(const TransactionID &transID) override;

	std::tuple</*transSession*/bool, /*inTrans*/bool, TransactionID, /*localTransState*/TransactionState> getCurrentTransaction(TransactionInfo *info = nullptr) override;

	QMAPIConstant::QMAPI_RETCODE queryTransactionStatus(const TransactionID &transID, BrokerTransactionStatus *transactionStatus) override;

	void setPreCommitListener(const std::shared_ptr<ITransactionPreCommitListener> &transListener) override;


public:
    /**
     * Get status of the session
     *
     * @param status
     * 	SessionStatus object to be filled
     *
     */
    void getStatus(SessionStatus& status);

	/**
	 * Unsubscribe specified destinations
	 *
	  * @return status
	 */
	QMAPIConstant::QMAPI_RETCODE unsubscribeSingle(const std::string& destination);

    /**
     * resubscribe
     *
      * @return status
     */
    QMAPIConstant::QMAPI_RETCODE resubscribeSingle(const std::string& destination);


	QMAPIConnection &getConnection() {
		return m_connection;
	}

	std::shared_ptr<QMAPISelectableClientSocket> getConnectSocket() {
		return m_pSocket.lock();
	}

	IIdGenerator& getIdGenerator() const {
		return m_idGenerator;
	}

	const QMAPIConfig &getConfig() {
		return m_config;
	}

	const std::string& getTransactionSessionID();

	MessageConsumeStatusManager &getMessageConsumeStatusManager() {
		return m_msgConsumeStatusMana;
	}

    MsgDispatchThread::RXMSGQUEUE* getConsumerDispatchQueue(const std::string& destination);
protected:
    /**
     * Query last sent message of a destination from broker
     *
     * @param destination
     *      name of a queue/topic
     * @param messageId
     *      returned last Message ID
     * @param message
     *      returned last message
     *
     * @return status
     *
     */
    QMAPIConstant::QMAPI_RETCODE queryLastMessage(const std::string& queueName, MessageID& messageId, dep::ByteBuffer& message);

    /**
     * Subscribe a queue
     *
     * @param queueName
     *      the name of a Queue
     * @param subscriptionId
     * 		subscription ID for subscribe
     * @param ackMode
     * 		mode to ack received message
     *
     * @return status
     *
     */
    QMAPIConstant::QMAPI_RETCODE subscribeQueue(const std::string& queueName, uint16_t subscriptionId, uint8_t ackMode,
            const std::string& messageMatcher,
            const std::string& subNodeID);

    /**
     * Unsubscribe a queue
     *
     * @param queueName
     *      the name of a Queue
     *
     * @return status
     *
     */
    QMAPIConstant::QMAPI_RETCODE unsubscribeQueue(const std::string& queueName);

    /**
     * Subscribe a topic
     *
     * @param topicName
     *      the name of the topic
     * @param durable
     * 		durable subscription of the topic
     * @param subscriptionId
     * 		subscriptionId for subscribe
     * @param ackMode
     * 		mode to ack received message
     *
     * @return status
     *
     */
    QMAPIConstant::QMAPI_RETCODE subscribeTopic(const std::string& topicName, 
            bool durable, 
            uint16_t subscriptionId, 
            uint8_t ackMode, 
            const std::string& subscriptionName,
            const std::string& messageMatcher, 
            const std::string& subNodeID, 
            bool isGroupedSubscription);

    /**
     * Unsubscribe a topic
     *
     * @param topicName
     *      the name of a Queue
     *
     * @return status
     *
     */
    QMAPIConstant::QMAPI_RETCODE unsubscribeTopic(const std::string& topicName);

    /**
     * Unsubscribe all destinations
     *
     * @return status
     */
    QMAPIConstant::QMAPI_RETCODE unsubscribeAll();

    /**
     * Resubscribe all destinations
     *
     * @return status
     */
    QMAPIConstant::QMAPI_RETCODE resubscribeAll();

    static uint8_t getAckMode(IMessagingSession::MessageReceivingAckMode rxAckMode);

    /**
     * Acknowledge a received to broker
     *
     * @param msg
     *  	Received message
     *
     */
    //bool ackRecvMessage(const BrokerMessage& msg) override;

    /**
     * Notify an acknowledge from broker of a sent message
     *
     * @param msg
     *  	the sent message acknowledged by broker
     *
     */
    void ackSentMessage(const BrokerMessage& msg);

    /**
     * Process a broker data message
     *
     * @param msg
     *  A broker data message
     */
    void onMessage(BrokerMessage& msg);

    /**
     * Process a broker receipt
     *
     * @param msg
     *  A broker receipt
     */
    void onReceipt(BrokerMessage& msg);

    /**
     * Failover producers and consumers of a session
     * Create queue and topics
     * Resubscribe queues and topics
     * Retransmit messages if necessary
     *
     * @return if a failover has been started
     */
    QMAPIConstant::QMAPI_RETCODE failover();

    void retransmit();

	friend class TransactionConnection;
    QMAPIConstant::QMAPI_RETCODE createQueueIntenal(const std::string& queueName, bool isExclusive, IDestination::Persistency persistency, bool isLocalFirst, uint64_t ackdMsgRetransmissionCapacity, bool isSyncMode);
    QMAPIConstant::QMAPI_RETCODE createTopicInternal(const std::string& topicName, IDestination::Persistency persistency, bool executedByFailOver = false, uint64_t ackdMsgRetransmissionCapacity = 0);

    /**
     * Query last sent message of a destination from broker
     *
     * @param destination
     *      name of a queue/topic
     * @param messageId
     *      returned last Message ID
     * @param message
     *      returned last message
     *
     * @return 
     *      QMAPIConstant::QMAPI_RETCODE: success  
     *      defaultTTL: default ttl
     *
     */
    QMAPIConstant::QMAPI_RETCODE registerProducer(const std::string& destination, uint32_t& defaultTTL);

    QMAPIConstant::QMAPI_RETCODE createQueueInternal(const std::string& queueName, bool exclusive, uint64_t ackdMsgRetransmissionCapacity, IDestination::Persistency persistency, bool isLocalFirst, bool executedByFailOver = false);

private:
    QMAPISession(QMAPIConnection &connection,
				 const QMAPIConfig& config,
				 IMessagingSession::UserThreadingMode,
				 bool isTransactSession = false,
				 const std::string &transSessionID = "",
				 const std::shared_ptr<ITransactionSessionListener> &transListener = nullptr);

    QMAPIConstant::QMAPI_RETCODE unsubscribe(const std::string& destinationName);

    bool validateDestination(const std::string& destinationName);
	std::string generateSubNodeID();
    QMAPIConstant::QMAPI_RETCODE sendSuspendInfos(const std::vector<std::string>& destinations);
    QMAPIConstant::QMAPI_RETCODE sendResumeInfos(const std::vector<std::string>& destinations);

private:
	QMAPIConnection &m_connection;
    std::string m_clientId;
    IMessagingSession::UserThreadingMode m_threadMode;

	std::map<std::string, std::shared_ptr<QMAPIMessageConsumer>> m_consumers;
	std::vector<std::shared_ptr<QMAPIMessageProducer>> m_producers;

	std::map<std::string, std::unique_ptr<MsgDispatchThread>> m_dest2Thread;  //  for UserThreadingMode::THREAD_PER_QUEUE_AND_TOPIC
	std::vector<std::unique_ptr<MsgDispatchThread>> m_dispatchThreads;  //  for UserThreadingMode::MULTIPLE_THREADS
	uint32_t m_dispatchThreadId = 0;
	std::unique_ptr<MsgDispatchThread::RXMSGQUEUE> m_pQueue = nullptr;

	const QMAPIConfig& m_config;
	std::weak_ptr<QMAPISelectableClientSocket> m_pSocket;

	std::map<RECEIPTID, CvResult> m_receiptIds;

	std::mutex m_mtxConsumers;
	std::mutex m_mtxProducers;
	std::mutex m_mtxReceiptIds;
	std::mutex m_mtxDestinations;
	std::mutex m_mtxSend;

	IIdGenerator& m_idGenerator;
	bool m_suspended = false;

	std::map<std::string, ServiceAttr> m_destinationAttributes;
	IConnectionManager& m_connectionManager;


	std::string m_transSessionID;
	std::shared_ptr<ITransactionSessionListener> m_transSessionListener;
	std::unique_ptr<TransactionSingleManager> m_localTransactionMana;

	MessageConsumeStatusManager m_msgConsumeStatusMana;
};

}} // namespace lightmq { namespace qmapi {

#endif /* QMAPISESSION_H_ */
