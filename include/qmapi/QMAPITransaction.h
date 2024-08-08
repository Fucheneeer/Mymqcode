////////////////////////////////////////////////////////////////////////////////
//
// QMAPITransaction.h
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
#ifndef QMAPILOCALTRANSACTION_H_
#define QMAPILOCALTRANSACTION_H_
#include <time.h>
#include <vector>
#include <atomic>
#include "QMAPIConstant.h"
#include "BrokerProtocolConstant.h"
#include "MsgTransactionOper.h"
#include "IConnection.h"
#include "IAcknowledgeListener.h"
#include "CommonConstant.h"
#include "GroupMessageID.h"


namespace dep {
	class ByteBuffer;
}

namespace lightmq { namespace qmapi {

class QMAPIMessageProducer;
class QMAPISession;
class QMAPIConnection;
class IIdGenerator;
class IMessageProducer;

class QMAPIMessageProducer;
class TransactionSingleManager;

//  when consumer ACK it's message and then rollback the trasaction.
//  those ACKed messages will be sended back to consumer again.
//  But QMAPISession's sequence check will filter the message for it's sequence is old.
//  So, we use MessageConsumeStatusManager to known the message is rollbacked ACK message,
//  they should not check the sequence.
//    1) when rollback transaction, call registerRollbackedAcks().
//    2) when handle broker message, call checkMessageIsRollbacked().
enum class MessageConsumeStatus {
	NEW_MESSAGE,
	SENDED_TO_APP,      //  If received again: Will not send again, not force ACK.
	APP_ACKED_IN_TRANS, //  App call acked, but in trans, If received again: will not send again, not force ACK.
	ROLLBACKED,         //  If received again: send again, not force ACK.
	ACKED               //  non-trans acked, or trans-ack commited, If received again: will not send again, force ack.
};


struct GroupMessageInfo
{
    std::string groupName {""};
    util::GroupMessageID groupID;
    uint8_t level {util::CommonConstant::DEFAULT_MESSAGE_LEVEL};
    std::atomic<uint16_t> msgSequence{util::CommonConstant::BASE_GROUP_SEQUENCE};
    bool isEnd {false};
};

class QMAPITransaction: public std::enable_shared_from_this<QMAPITransaction> {
public:
	QMAPITransaction(TransactionSingleManager *transMana, QMAPISession* session, QMAPIMessageProducer *producer, uint64_t transID, IIdGenerator &idGen);
	~QMAPITransaction();

	const std::string &getTransSessionID();
	uint64_t getTransID() const { return m_transID; }
	uint32_t getTransMsgSeq(uint32_t count = 1);


	void endTrans() {
		m_transEnd = true;
	}

	bool isTransEnd() const {
		return m_transEnd;
	}

	bool isCacheAbleToRemove() const {
		return m_transLocalState == TransactionState::PRE_COMMITED
		    || !isTransactionStateActive(m_transLocalState);
	}

	bool isInCommiting() const {
		return m_inCommiting;
	}

	bool isCommitOK() const {
		return m_commitOK;
	}

	bool isRollbackOK() const {
		return m_rollbackOK;
	}

	bool isPreCommitOK() const {
		return m_preCommitOK;
	}

	bool isInPreCommiting() const {
		return m_inPreCommiting;
	}

	TransactionState getTransLocalState() const {
		return m_transLocalState;
	}

    bool isClosed() const{
	    auto localState = m_transLocalState;
		return (localState == TransactionState::COMMITED || 
		    localState == TransactionState::ROLLBACKED ||
		    localState == TransactionState::DISCARD ||
		    localState == TransactionState::TIMEOUTED);
    }

	/**
     * send a message to broker, within the current Transaction
     *
     * @param destination
     *      name of a queue/topic
     * @param providerID
     * 		the sender's providerID
     * @param msgBuf
     *		message array to send
     * @param msgCount
     * 		message count in message array to send
     * @param messageProperties
     * 		message properties for all send messages.
     *
     * @return status, QMAPIConstant::QMAPI_OK if success.
     *
     */
	QMAPIConstant::QMAPI_RETCODE sendTransactionMessage(const std::string &destination, const std::string &providerID,
	                                                    InternalLightmqMessage **msgBuf, int msgCount );

	bool regTransactionMessage(const std::string destination, TransactionMessageSeq transMsgSeq, size_t size, protocol::TransactionMessageSendResult sendResult);
	bool regConsumerAck(const DestinationMessageID &destinationMessageID, const std::string &subscriptionName, uint16_t subscriptionId);

	void getTransactionInfo(TransactionInfo *info = nullptr);

//	QMAPIConstant::QMAPI_RETCODE commitLocalTransactionOld();
//	QMAPIConstant::QMAPI_RETCODE rollbackLocalTransactionOld();

	//  Local transaction operates
	QMAPIConstant::QMAPI_RETCODE commitLocalTransaction(TransactionState *brokerTransState);
	QMAPIConstant::QMAPI_RETCODE rollbackLocalTransaction(TransactionState *brokerTransState);

	//  Distribute transaction operates
	QMAPIConstant::QMAPI_RETCODE preCommitTransaction(const std::shared_ptr<ITransactionPreCommitListener> &transListener);
	QMAPIConstant::QMAPI_RETCODE commitTransaction();
	QMAPIConstant::QMAPI_RETCODE rollbackTransaction();
	static QMAPIConstant::QMAPI_RETCODE commitTransaction(QMAPIConnection *conn, const TransactionID &transID);
	static QMAPIConstant::QMAPI_RETCODE rollbackTransaction(QMAPIConnection *conn, const TransactionID &transID);
	static QMAPIConstant::QMAPI_RETCODE queryTransactionStatus(QMAPIConnection *conn, const TransactionID &transID, BrokerTransactionStatus *transactionStatus);

	QMAPIConstant::QMAPI_RETCODE startTransaction();
	QMAPIConstant::QMAPI_RETCODE discardTransaction();

    // group Message.
    void initGroupInfo(uint8_t level, const std::string &groupName);
    std::string groupName() const;
    uint8_t groupLevel() const;
    bool isGroup() const;
    void assignGroupInfoNolock(InternalLightmqMessage &msg);

	inline bool isLocalTransaction() const
	{
		return m_isLocalTrans;
	}

	inline bool isEmptyTransaction() const
	{
		return m_transMessageInfos.empty() && m_consumerMsgIDs.empty();
	}

	void setPreCommitListener(const std::shared_ptr<ITransactionPreCommitListener> &transListener)
	{
		m_preCommitListener = transListener;
	}


private:
	void registerToPool();
	void unregisterFromPool();
	QMAPIConstant::QMAPI_RETCODE sendTransOperMessage(protocol::ReceiptID rid, protocol::TransactionOper oper);
	static QMAPIConstant::QMAPI_RETCODE sendTransOperMessage(QMAPIConnection *conn, protocol::ReceiptID rid, const TransactionID &transID, protocol::TransactionOper oper);
	uint16_t getTransOperSeq();

	mutable std::mutex m_mtxTransOperate;

	TransactionSingleManager *m_transMana;
	QMAPISession* m_session;
	QMAPIMessageProducer *m_transOperQueueProducer;
	uint64_t m_transID;
	std::atomic<uint32_t> m_msgSeq;
	std::atomic<uint16_t> m_operSeq;

	//  事务的状态管理：
	//  1. m_inCommiting变为true之前，不允许开始新事务
	//  2. 一旦开始一次提交或者回滚, m_inCommiting为true, 此时发送消息或者手动消费消息将触发新事务
	//     但是将禁止被动消费者收取消息。
	//  3. 一旦提交成功(m_commitOK)或者回滚成功(m_rollbackOK)，则任何操作包括被动消息都将触发新事务
	//  4. 开始另外一个事务后，m_transEnd为true.
	bool m_transEnd{false};
	bool m_inCommiting{false};
	bool m_inPreCommiting{false};
	bool m_preCommitOK{false};
	bool m_commitOK{false};
	bool m_rollbackOK{false};

	TransactionState m_transLocalState {TransactionState::START};
	bool m_isLocalTrans { true };

	std::vector<protocol::TransactionMessageInfo> m_transMessageInfos;
	//std::vector<ProviderMessageID> m_providerMsgIDs;
	std::vector<protocol::ConsumerAckInfo> m_consumerMsgIDs;
	void updateConsumeAckStatus(MessageConsumeStatus consumeStatus);

	IIdGenerator& m_idGenerator;
	std::shared_ptr<ITransactionPreCommitListener> m_preCommitListener;
    std::unique_ptr<GroupMessageInfo> m_groupInfo; 
};

class TransactionPool {
	TransactionPool(const TransactionPool &pool) = delete;
	void operator=(const TransactionPool &pool) = delete;
public:
	TransactionPool() = default;
	//~TransactionPool() = default;

	void registerTransactionToPool(const TransactionID &transID, const std::shared_ptr<QMAPITransaction> &trans);
	void unregisterTransactionFromPool(const TransactionID &transID);

	std::shared_ptr<QMAPITransaction> findTransactionInPool(const TransactionID &transID);
private:
	std::mutex m_mtxTransGlobalMana;

	std::unordered_map<TransactionID, std::shared_ptr<QMAPITransaction>> m_transactions;
};

class TransactionIDGenerator {
public:
	TransactionIDGenerator();
	~TransactionIDGenerator();

	//const std::string& genTransactionIDStr();
	uint64_t genTransactionID();

private:
	// std::string m_transSession;
	// char m_idBuf[1024];
	// char *m_seqStr;
	// time_t m_startTime;
	// std::atomic<uint32_t> m_transIDSeq;
	static std::atomic<uint64_t> m_transIDTotalSeq;
};

class MessageConsumeStatusManager {
	MessageConsumeStatusManager(const MessageConsumeStatusManager &rv) = delete;
	void operator=(const MessageConsumeStatusManager &rv) = delete;

public:
	MessageConsumeStatusManager();
	//void registerRollbackedAcks(const protocol::ConsumerAckInfo &ackInfo);

	//bool checkMessageIsRollbacked(const std::string &subscriptionName, const std::string &destination, MessageID brokerMsgID, bool unRegister = true);

	MessageConsumeStatus updateMessageConsumeStatus(const protocol::ConsumerAckInfo &ackInfo, MessageConsumeStatus consumeStatus);
	MessageConsumeStatus getMessageConsumeStatus(const protocol::ConsumerAckInfo &ackInfo);

private:
	std::mutex m_mtxRollbackedAcks;
	std::unordered_map<protocol::ConsumerAckInfo, MessageConsumeStatus,
	                   std::hash<protocol::ConsumerAckInfo>,
	                   protocol::CmpConsumerAckInfoWithoutSubscriptionId> m_consumedMessages;  //  waiting consumer ack from HA.
};

class TransactionConnection;
class TransactionSingleManager {
public:
	TransactionSingleManager(QMAPISession *session, bool enableTrans,
	                         const std::string &transSessionID = "",
	                         const std::shared_ptr<ITransactionSessionListener> &transListener = nullptr);
	~TransactionSingleManager();
	void initLocalTransactionSingleMana();

	bool isTransactionInBusy();
	std::shared_ptr<QMAPITransaction> touchTransaction(bool allowDiscardPrevTrans);
	std::shared_ptr<QMAPITransaction> startGroupMessage(const std::string &groupName, uint8_t level);
	QMAPIConstant::QMAPI_RETCODE commitGroupMessage(TransactionState *brokerTransState);
	QMAPIConstant::QMAPI_RETCODE rollbackGroupMessage(TransactionState *brokerTransState);

	bool isEnableTrans() const {
		return m_enableLocalTrans;
	}

	const std::shared_ptr<QMAPITransaction>& getCurrentTrans() const {
		std::unique_lock<std::recursive_mutex> lckTrans(m_mtxTransMana);
		return m_currentTransaction;
	}

	void setPreCommitListener(const std::shared_ptr<ITransactionPreCommitListener> &transListener);

	std::tuple</*transSession*/bool, /*inTrans*/bool, TransactionID, /*localTransState*/TransactionState> getCurrentTransaction(TransactionInfo *info = nullptr);

	//  Local Transaction Operates
	QMAPIConstant::QMAPI_RETCODE commitLocalTransaction(TransactionState *brokerTransState);
	QMAPIConstant::QMAPI_RETCODE rollbackLocalTransaction(TransactionState *brokerTransState);

	//  Distribute Transaction Operates
	QMAPIConstant::QMAPI_RETCODE preCommitTransaction(const std::shared_ptr<ITransactionPreCommitListener> &transListener, const TransactionID &transID);
	QMAPIConstant::QMAPI_RETCODE commitTransaction(const TransactionID &transID);   //  应传入TransactionID
	QMAPIConstant::QMAPI_RETCODE rollbackTransaction(const TransactionID &transID); //  应传入TransactionID
	QMAPIConstant::QMAPI_RETCODE queryTransactionStatus(const TransactionID &transID, BrokerTransactionStatus *transactionStatus);
	//QMAPIConstant::QMAPI_RETCODE commitTransaction(QMAPIConnection *conn, const TransactionID &transID);
	//QMAPIConstant::QMAPI_RETCODE rollbackTransaction(QMAPIConnection *conn, const TransactionID &transID);

	QMAPIConstant::QMAPI_RETCODE sendTransactionMessage(const std::string &destination, const std::string &providerID,
	                                                    InternalLightmqMessage **msgBuf, int msgCount);

	//bool regTransactionMessage(const ProviderMessageID &msgID, size_t size);

	bool regConsumerAck(const DestinationMessageID &destinationMessageID, const std::string &subscriptionName, uint16_t subscriptionId, bool allowDiscardPrevTrans);

	TransactionPool& getTransPool() { return m_transPool; }

	//MessageConsumeStatusManager& getRollbackedAckMana() { return m_rollbackedAckMana; }

private:
	QMAPISession *m_session;
	QMAPIMessageProducer *m_transOperQueueProducer;

	mutable std::recursive_mutex m_mtxTransMana;
	bool m_enableLocalTrans;
	std::shared_ptr<QMAPITransaction> m_currentTransaction;
	TransactionIDGenerator m_transIDGenerator;

	std::shared_ptr<TransactionConnection> m_transConn;

	TransactionPool m_transPool;  //  for async operate transaction.
	//std::shared_ptr<ITransactionPreCommitListener> m_preCommitListener;

	//MessageConsumeStatusManager m_rollbackedAckMana;
};

class TransactionConnection: private IAcknowledgeListener {
public:
	TransactionConnection(QMAPIConnection &conn);
	~TransactionConnection();
	void initTransactionConnection();


	QMAPIMessageProducer *getTransOperQueueProducer() {
		return m_transOperQueueProducer;
	}

private:  //  IAcknowledgeListener
	void onAcknowledge(const std::string &destination, const MessageID &messageId, const MessageID &endMessageId) override;

private:
	QMAPIConnection &m_connection;
	QMAPISession *m_transProducerSession;
	IMessageProducer *m_transOperQueueProducerInter;
	QMAPIMessageProducer *m_transOperQueueProducer;
};

}} // namespace lightmq { namespace qmapi {

#endif  //  !QMAPILOCALTRANSACTION_H_
