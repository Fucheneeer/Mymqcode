////////////////////////////////////////////////////////////////////////////////
//
// QMAPIMessageProducer.h
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

#ifndef QMAPIMESSAGEPRODUCER_H_
#define QMAPIMESSAGEPRODUCER_H_

#include <iostream>
#include <memory>
#include <vector>
#include <list>
#include <algorithm>
#include <unordered_map>

#include "QMAPIConstant.h"
#include "IAcknowledgeListener.h"
#include "IDestination.h"
#include "IMessageProducer.h"
#include "IMessagingSession.h"
#include "QMAPISelectableClientSocket.h"
#include "QMAPIConfig.h"
#include "IIdGenerator.h"
#include "IThrottler.h"
#include "QMAPIThrottler.h"
#include "QMAPIProcessorChain.h"
#include "FinalChainProcessor.h"
#include "PluginModuleRepository.h"
#include "ImplFactoryAttribute.h"
#include "ConfigLoader.h"
#include "InternalLightmqMessage.h"
#include "QMAPITransactionCaller.h"

namespace lightmq { namespace qmapi {

class QMAPIMessageProducer : public IMessageProducer, public QMAPITransactionCaller {
	friend class QMAPISession;

public:
	static const ResultCode NA_RESULTCODE = 12345;

    class CvResult {
    public:
        std::condition_variable* pCV;
        ResultCode result;
        std::vector<InternalMessagePtr> messages;
        uint32_t nonAckedCount; // #message not ack'ed by broker, initial value = messages.size(), final = 0
        MessageID messageId;
		// std::unordered_map<std::string, std::string> messageProperties;

		//  TRANSACTION ptr USE SHARED_PTR
		//  FOR CvResult might reference old transaction info.
		std::shared_ptr<QMAPITransaction> concernedTransaction;

        CvResult() {
            pCV = nullptr;
            result = NA_RESULTCODE;
            nonAckedCount = 0;
            messages.clear();
            messageId = MessageID{0, 0};
        }

        CvResult(const CvResult& o) {
            pCV = o.pCV;
            result = o.result;
            nonAckedCount = o.nonAckedCount;
            messages.clear();
            for (const auto& message : o.messages) {
                messages.push_back(message->makeCopy());
            }
			messageId = o.messageId;
        }

        // CvResult& operator=(const CvResult& o) {
        //     pCV = o.pCV;
        //     result = o.result;
        //     nonAckedCount = o.nonAckedCount;
        //     messages.clear();
        //     for (const dep::ByteBuffer& message : o.messages) {
        //         messages.push_back(dep::ByteBuffer());
        //         messages.back().put(message.getArray()+message.getPosition(), message.remaining());
        //         messages.back().flip();
        //     }
		// 	messageId = o.messageId;
        //     return *this;
        // }
    };

    class ProducerStatus {
    public:
        std::string brokerIp;
        uint16_t brokerPort;
        std::string destinationName;
        IDestination::Type destinationType;
        uint64_t sentMessageCount;
        uint64_t notAcknowledgedSentMessageCount;
        uint64_t throttledMessageCount;
        uint64_t failedMessageCount;
        uint64_t suspendedMessageCount;
        uint64_t rejectedInFailoverMessageCount;
        uint64_t ackdRetxMsgsCount;
        IMessagingSession::MessageSendingMode txMode{IMessagingSession::MessageSendingMode::ASYNCHRONOUS};
        bool isSuspended;
    };

public:
	virtual ~QMAPIMessageProducer() override;
	QMAPIMessageProducer() = delete;


    /**
     *
     * Factory function to create a producer
     * pass (nullptr = default) to run a Producer in synchronous mode
     * pass (pointer to an client supplied listener) in aysnchronous mode
     *
     * @param config
     * 		QMAPI configuration
     * @param pSocket
     * 		TCP socket connected to a broker
     * @param clientId
     * 		Client ID
     * @param destination
     * 		Destination of a queue or topic
     * @param listener
     * 		Acknowledge listener of sent messages
     * @param idGenerator
     * 		ID Generator to producer Message ID to send with messages
     * @param pThrottler
     * 		Throttler to implement flow control
     * @param manager
     * 		Failover manager
     */
    static std::shared_ptr<QMAPIMessageProducer> getInstance(const QMAPIConfig& config, IMessagingSession::MessageSendingMode txMode, std::weak_ptr<QMAPISelectableClientSocket> pSocket, const std::string& clientId, const IDestination& destination, IAcknowledgeListener* listener, IIdGenerator& idGenerator, IThrottler* pThrottler, IConnectionManager& manager);

    /**
     * Sends a batch of messages
     *
     * @return status
     */
    virtual QMAPIConstant::QMAPI_RETCODE send(const std::vector<LightmqMessage*>& messages) override;

    /**
     * Sends a message
     *
     * @return status
     */
    virtual QMAPIConstant::QMAPI_RETCODE send(const LightmqMessage& message) override;


    virtual QMAPIConstant::QMAPI_RETCODE send(const LightmqMessage& message, const std::unordered_map<std::string, std::string> messageProperites) override;
	

    virtual QMAPIConstant::QMAPI_RETCODE sendKeyBalanceMessage(LightmqMessage& message, const char* dispatchKey) override;
	

    virtual QMAPIConstant::QMAPI_RETCODE sendKeyBalanceMessage(std::vector<LightmqMessage*>& messages, const char* dispatchKey) override;


	 /**
	* Sends a message
	*
	* @return struct
	*/
    virtual SendResultInfo send(const LightmqMessage& message, std::function<void(MessageID& messageID)> messageIdCallback) override;

    /**
     * Get status of the producer
     *
     * @param status
     * 		ProducerStatus object to be filled
     *
     */
    void getStatus(ProducerStatus& statistics) const;

    /**
     * Set acknowledge listener of the producer
     *
     * @param listner
     *      IAcknowledgeListener
     *
     */
    void setAcknowledgementListener(IAcknowledgeListener* m_pListener) override;

    /***
     * Set asynchonrous or synchronous sending mode on-the-fly
     *
     * @param listener
     *      Acknowledge listener of sent messages
     *
     */
    void setAsync(IAcknowledgeListener* pListener = nullptr);


	CvResult& addCvResult(const InternalMessagePtr *msg, int msgCount, MessageID messageId, std::condition_variable *pCv = nullptr);
	CvResult& addCvResult(const std::shared_ptr<QMAPITransaction> &trans,
		const InternalMessagePtr *msg, int msgCount, MessageID messageId,
        std::condition_variable *pCv = nullptr);
	CvResult& addCvResult(QMAPITransaction *concernedTrans,
		const InternalLightmqMessage &msg, MessageID messageId, 
        std::condition_variable *pCv = nullptr);

	QMAPIConstant::QMAPI_RETCODE startGroupMessage(const std::string &groupName, uint8_t messagLevel = util::CommonConstant::DEFAULT_MESSAGE_LEVEL) override;
	QMAPIConstant::QMAPI_RETCODE commitGroupMessage(TransactionState *brokerTransState) override;
	QMAPIConstant::QMAPI_RETCODE rollbackGroupMessage(TransactionState *brokerTransState) override;
    

protected:  // send interfaces with dep::ByteBuffer
    /**
     * Sends a batch of messages
     *
     * @return status
     */
    QMAPIConstant::QMAPI_RETCODE sendByteBuffer(const std::vector<InternalMessagePtr>& messages) ;

    /**
     * Sends a message
     *
     * @return status
     */
    QMAPIConstant::QMAPI_RETCODE sendByteBuffer(const InternalLightmqMessage& message) ;

	SendResultInfo sendByteBuffer(const InternalLightmqMessage& message, std::function<void(MessageID& messageID)> messageIdCallback);

protected:
    /**
     * Notify message sender in synchronous sending mode
     *
     * @param mag
     * 		broker message
     *
     * @return if msg corresponds to any of the outstanding sent messages from the producer
     */
  	bool notifyCorrelationId(const BrokerMessage& msg);

public:
    /**
     * Suspend the producer from sending messages
     *
     */
  	void suspend() {
    	m_suspended = true;
    }

    /**
     * Resume the producer from suspension
     *
     */
    void resume() {
    	m_suspended = false;
    }

    void setDefaultMsgTTL(uint32_t ttl){
        m_destinationDefaultTTL = ttl;
    }

    void setBrokerTimeDiff(uint64_t timeDiff) {
		m_brokerTimeDiff = timeDiff;
	}

protected:
    /**
     * Retransmit non ack'ed and ack'ed messages after a failover
     *
     */
    void retransmit();

    /**
     * Get Destination
     *
     */
    std::string getDestination();

	const std::string& getProviderID();

	std::mutex & getMessageIDLock();

    inline void sendAcknowledge(const MessageID& startMsgId, const MessageID& endMsgId);


private:
    QMAPIMessageProducer(const QMAPIConfig& config,
                         IMessagingSession::MessageSendingMode txMode,
						 std::weak_ptr<QMAPISelectableClientSocket> pSocket,
						 const std::string& clientId,
						 const IDestination& destination,
						 IAcknowledgeListener* listener,
						 IIdGenerator& idGenerator,
						 IThrottler* pThrottler,
						 IConnectionManager& manager);

    QMAPIConstant::QMAPI_RETCODE sendWithDestination(const IDestination& destination, const std::vector<InternalMessagePtr>& messages, MessageID messageId);
    QMAPIConstant::QMAPI_RETCODE sendWithDestination(QMAPITransaction *concernedTrans,
	                                  const IDestination& destination, const InternalLightmqMessage& message, MessageID messageId);

   	QMAPIConstant::QMAPI_RETCODE sendInternal(const std::vector<InternalMessagePtr>& messages);
	SendResultInfo sendInternal(QMAPITransaction *concernedTrans, const InternalLightmqMessage& message, std::function<void(MessageID&)> messageIdCallback);

	friend class QMAPITransaction;
	QMAPIConstant::QMAPI_RETCODE sendInnerTransactQueueMessage(QMAPITransaction *concernedTrans, const LightmqMessage& message);
	SendResultInfo retransmitSend(QMAPITransaction *conceredTrans, const InternalMessagePtr& message, MessageID messageId);

  	void recoverBufferedMessages(const std::string &recoverPrompt, const std::list<CvResult>& msgBuf, const MessageID& recoveredMsgId);

  	inline IAcknowledgeListener* getListener();

    void loadProcessChain();
    IChainProcessor::ProcessResult processSendBuffer(InternalLightmqMessage &imessage);

    bool isRealInTransaction() const
    {
        return !inGroupMessage()  && inTransaction();
    }

    bool inGroupMessage() const
    {
        if (nullptr == m_groupController) return false;
        auto p = m_groupController->getCurrentTrans();
        if (nullptr == p) return false;
        if (p->isClosed()) return false;
        return p->isGroup();
    }

    uint8_t getGroupLevel() const
    {
        if (nullptr == m_groupController) return 0;
        auto p = m_groupController->getCurrentTrans();
        if (nullptr == p || p->isClosed() || !p->isGroup()) return 0;
        return p->groupLevel();
    }

    void initGroupController(QMAPISession *session);

private:
  	uint32_t m_loopRetryUsec;

  	std::string m_clientId;

    uint16_t m_sessionSeqId;

    std::unique_ptr<IDestination> m_pDestination = nullptr;
    std::atomic<IMessagingSession::MessageSendingMode> m_txMode;
	IAcknowledgeListener* m_pListener;

    std::weak_ptr<QMAPISelectableClientSocket> m_pSocket;
	const QMAPIConfig& m_config;

	//FinalMessageListener m_finalMsgListener;
	std::unique_ptr<PluginModuleRepository> m_pPluginRepository;
	std::unique_ptr<QMAPIProcessorChain> m_pProcessorChain;

	std::list<CvResult> m_sentSeqIds;
	std::list<CvResult> m_ackdMsgs;
	std::mutex m_mtxSentSeqIds;

	IIdGenerator& m_idGenerator;

	std::unique_ptr<IThrottler> m_pOwnedThrottler = nullptr;
	IThrottler* m_pThrottler = nullptr;

	std::atomic<bool> m_suspended;
	std::atomic<uint64_t> m_sentMessageCount;
	std::atomic<uint64_t> m_throttledMessageCount;
	std::atomic<uint64_t> m_failedMessageCount;
    std::atomic<uint64_t> m_suspendedMessageCount;

    std::atomic<uint64_t> m_ackdMsgRetransmissionCapacity;
    IConnectionManager& m_connectionManager;
    std::atomic<uint64_t> m_failoverMessageCount;

    std::mutex m_mtxSend;
    std::atomic<uint32_t> m_sizeAckdMsgs;

    std::mutex m_mtxListener;
	bool m_enableEncryption;

    std::mutex m_messageIDLock;
    ConfigLoader m_configLoader;

    // 组消息，直接使用事务的机制来实现
    std::unique_ptr<TransactionSingleManager> m_groupController;
	
	static constexpr uint16_t MAX_DISPATCHKEY_LENGTH = 128;

    uint32_t m_destinationDefaultTTL;
    int64_t m_brokerTimeDiff = 0;   // current time - broker time
};


}} // namespace lightmq { namespace qmapi {


#endif /* QMAPIMESSAGEPRODUCER_H_ */
