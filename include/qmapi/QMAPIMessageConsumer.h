////////////////////////////////////////////////////////////////////////////////
//
// QMAPIMessageConsumer.h
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

#ifndef QMAPIMESSAGECONSUMER_H_
#define QMAPIMESSAGECONSUMER_H_

// #include <condition_variable>
#include <memory>
#include "IMessageConsumer.h"
#include "IDestination.h"
#include "IMessagingSession.h"
#include "IAckReceivedMsgSender.h"
#include "QMAPISelectableClientSocket.h"
#include "QMAPIConfig.h"
#include "MsgDispatchThread.h"
#include "ApplicationLog.h"
#include "QMAPIProcessorChain.h"
#include "FinalChainProcessor.h"
#include "PluginModuleRepository.h"
#include "ImplFactoryAttribute.h"
#include "ConfigLoader.h"
#include "QMAPITransactionCaller.h"
#include "LogWrapper.h"
#include "UnconsumedQueue.h"

namespace lightmq { namespace qmapi {


class QMAPISession;
class QMAPIProcessorMap;

class QMAPIMessageConsumer : public IMessageConsumer, public QMAPITransactionCaller, public std::enable_shared_from_this<QMAPIMessageConsumer>{
	friend class QMAPISession;
public:
	class ConsumerStatus {
    public:
        std::string brokerIp;
        uint16_t brokerPort;
        std::string destinationName;
        IDestination::Type destinationType;
        uint64_t recvMessageCount;
        uint16_t subscriptionId;
        bool isDurableTopicSubscription;
        bool isSuspended;
        IMessagingSession::MessageReceivingAckMode rxAckMode;
    };

    enum ErrorCode{
        SUCCESS = 0,
        MESSAGE_EXPIRED = 10  // message expired
    };

    virtual void unsubscribe() override;

    virtual void resubscribe() override;

public:
	virtual ~QMAPIMessageConsumer();
	QMAPIMessageConsumer() = delete;

	/**
     *
     * Factory function to create a QMAPIMessageProducer
     *
     * @param config
     * 		QMAPI configurations
     * @param pSocket
     * 		TCP socket to consume messages from broker
     * @param destination
     * 		Destination of a queue/topic
     * @param listener
     * 		Message listener supplied by caller to consume messages
     * @param ackMode
     * 		API auto ack, individual ack by application, or batch ack by application
     * @param threadMOde
     * 		Single thread, thread per queue/topic, multithreaded
     * @param subscriptionName
     * 		SubscriptionName required for a durable topic subscription
     *
     * @return consumer to be owned by the caller
     *
     */
    static std::shared_ptr<QMAPIMessageConsumer> getInstance(QMAPISession* pSession, const QMAPIConfig& config,
        std::weak_ptr<QMAPISelectableClientSocket> pSocket, 
        const IDestination& destination, 
        IMessageListener* listener, 
        IMessagingSession::MessageReceivingAckMode = IMessagingSession::MessageReceivingAckMode::API_AUTO_ACKNOWLEDGE, 
        const std::string& subscriptionName = "", 
        const std::string& messageMatcher = "", 
        const std::string& subNodeID = "", 
        bool isGroupedSubscription = false);

	/**
     *
     * Acknowledge received message individually manual
     *
     * @param destination
     *  	Destination of this acknowledge corresponds to
     * @param messageId
     * 		Message ID to acknowledge
     *
     */
    bool sendAcknowledge(const std::string& destination, const MessageID& messageId) override;

    /**
     *
     * Acknowledge received message by batch manual
     *
     * @param destination
     *  	Destination of this acknowledge corresponds to
     * @param startMessageId
     * 		starting Message ID to acknowledge
     * @param endMessageId
     * 		ending Message ID to acknowledge
     */
    bool sendAcknowledge(const std::string& destination, const MessageID& startMessageId, const MessageID& endMessageId) override;

    /**
     * Get status of the consumer
     *
     * @param status
     * 		ConsumerStatus object to be filled
     *
     */
    void getStatus(ConsumerStatus& statistics) const;

    /**
   * Remove message listener of the consumer
   *
   *
   */
    void removeMessageListener() override;

    /**
     * Set message listener of the consumer
     *
     * @param listner
     *      IMessageListener
     *
     */
    void setMessageListener(IMessageListener* listener) override;

	/**
     *
     * Acknowledge received message auto.
     *
     * @param msg
     * 		Message to acknowledge
     *
     */
    bool sendAcknowledgeInternal(const std::string& destination, const MessageID& messageId, bool forced = false);
    bool sendMsgExpiredAcknowledgeInternal(const std::string& destination, const MessageID& messageId, bool forced = false);
	/**
	* Set the suspend status of the consumer
	*
	* @param status
	* 		suspend status
	*/
	void setSuspended(bool status);

    uint32_t parseMessage(BrokerMessage& msg, dep::ByteBuffer &brokerMsg, LightmqMessage &lmsg); 
    
#ifndef NDEBUG
    void send(dep::ByteBuffer &buf);
#endif
protected:
    /**
     *
     *  Set Subscription ID of the subscribed destination
     *
     *  @param subscriptionId
     *  	Subscription ID
     *
     */
    void setSubscriptionId(uint16_t subscriptionId) {
    	m_subscriptionId = subscriptionId;
    }

    /**
     *
     *  Get Subscription ID of the subscribed destination
     *
     *  @return Subscription ID
     *
     */
    uint16_t getSubscriptionId() const {
    	return m_subscriptionId;
    }

    /**
     *
     *  Get Subscription Name of a durable topic subscription
     *
     *  @return Subscription Name
     *
     */
    std::string getSubscriptionName() const {
    	return m_subscriptionName;
    }

    /**
     *
     *  Get type of the subscribed destination
     *
     *  @return queue/topic
     *
     */
    IDestination::Type getSubscriptionType() const {
    	return m_pDestination->getType();
    }

    /**
     * Send received message to listener supplied by caller in the following user thread mode
     * 1) THread per queue/topic
     * 2) Multithreaded
     *
     * @param msg
     * 		message to send
     */
    void onMessage(BrokerMessage& msg, MsgDispatchThread::RXMSGQUEUE* pQueue);

    /**
     * Send received message to listener supplied by caller in single thread mode
     *
     * @param msg
     * 		message to send
     */
    //void sendMessage(BrokerMessage& msg);

    /**
     * Return the given received message acknowledge mode
     *
     *  @return Received message acknowledge mode
     */
    IMessagingSession::MessageReceivingAckMode getAckMode() const {
    	return m_rxAckMode;
    }

    std::mutex& getSubscriptionIdMutex() {
    	return m_mtxSubscriptionId;
    }
	
	std::string getMessageMatcher() const {
		return m_messageMatcher;
	}

	std::string getSubNodeID() const {
		return m_subNodeID;
	}

    bool getIsGroupedSubscription() const {
        return m_isGroupedSubscription;
    }

    bool canAcknowledgeManual() const;
    
    void setBrokerTimeDiff(uint64_t timeDiff) {
		m_brokerTimeDiff = timeDiff;
	}
    virtual QMAPIConstant::QMAPI_RETCODE receive(int milliTimeout, LightmqMessage &lmqMessage) override;

    void beforeFailover();
    void endFailover();
    QMAPIConstant::QMAPI_RETCODE failover();


private:
    using LASTMSGKEY = std::string;

    QMAPIMessageConsumer(QMAPISession* pSession,
            const QMAPIConfig& config,
            std::weak_ptr<QMAPISelectableClientSocket> pSocket,
			const IDestination& destination,
			IMessageListener* listener,
			IMessagingSession::MessageReceivingAckMode rxAckMode,
			const std::string& subscriptionName,
            const std::string& messageMatcher,
            const std::string& subNodeID, 
            bool isGroupedSubscription = false);

    MessageID& getLastRxMessageID(LASTMSGKEY &lastMsgKey, bool isGroup, uint8_t level);
	uint32_t filterMessage(BrokerMessage& msg);

    inline LASTMSGKEY getRxMsgKey(const std::string& providerClientId, const std::string& destination) {
    	return std::string(providerClientId + "^" + destination);
    }

    void incRecvMessageCount(uint64_t count = 1);

    inline IMessageListener* getListener();

    void loadProcessorMap();
    void createLightmqMessage(BrokerMessage &msg, uint32_t idx, LightmqMessage &lmqMsg);
    void sendMessagePull(int timeout);

    void sendTransactionAckMessage(const std::string& destination);

private:

	std::string getBrokerSubscriptionName();

	const QMAPIConfig& m_config;

    dep::ApplicationLog m_applicationLog;     //!< application logger

    //FinalMessageListener m_finalMsgListener;
    std::unique_ptr<PluginModuleRepository> m_pPluginRepository;
    //std::unique_ptr<QMAPIProcessorChain> m_pProcessorChain;
    std::unique_ptr<QMAPIProcessorMap> m_pProcessorMap;
    
	std::unique_ptr<IDestination> m_pDestination = nullptr;
    IMessageListener* m_pListener;
    std::weak_ptr<QMAPISelectableClientSocket> m_pSocket;
    std::vector<std::string> m_subscribedQueues;
    std::vector<std::string> m_subscribedTopics;
    uint16_t m_subscriptionId{0};
    std::string m_subscriptionName;	// non-empty for consumer of durable subscription

    uint64_t m_recvMessageCount = 0;
    bool m_suspended = false;

    // std::map<LASTMSGKEY, MessageID> m_lastRxMessageMap;
    // std::map<LASTMSGKEY, MessageID> m_lastRxGroupMessageMap;
    struct LastMessage
    {
        MessageID m_lastRxMessageMap[util::CommonConstant::MAX_MESSAGE_LEVEL_COUNT * 2];
    };
    std::map<LASTMSGKEY, LastMessage > m_lastRxMessageMap;

    IMessagingSession::MessageReceivingAckMode m_rxAckMode;
    //IAckReceivedMsgSender& m_ackSender;

	std::mutex m_mtxSubscriptionId;
	std::recursive_mutex m_mtxListener;
	std::string m_messageMatcher;
	std::string m_subNodeID;

    bool m_isGroupedSubscription{ false };
    QMAPISession* m_messageSession;
    ConfigLoader m_configLoader;

    int64_t m_brokerTimeDiff = 0;   // current time - broker time
	// using RXMSGQUEUE = QMAPICircularBuffer<BrokerMessage>;
    // RXMSGQUEUE *m_unConsumerdMessage; 
    UnconsumedQueue m_unConsumedQ;
};

}} // namespace lightmq { namespace qmapi {

#endif /* QMAPIMESSAGECONSUMER_H_ */
