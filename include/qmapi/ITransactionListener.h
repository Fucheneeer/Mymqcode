////////////////////////////////////////////////////////////////////////////////
//
// IConnection.h
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

#ifndef ITRANSACTION_LISTENER_H_
#define ITRANSACTION_LISTENER_H_


#include <memory>
#include "BrokerProtocolConstant.h"

namespace lightmq { namespace qmapi {

class IMessagingSession;

class ITransactionPreCommitListener {
public:
	virtual ~ITransactionPreCommitListener() = default;

	//  Broker应答Producer preCommit后回调
	//  Producer 需要在此函数中手动提交或者回滚事务,
	//  在此前：需要检验分布式事务所有的条件都已就绪
	//  比如：本地的事务操作，或者其他的分布式事务preCommit都已OK.

	/**
	 * Transaction PreCommit operate result listener.
	 * IMessagingSession::preCommitTransaction() is a asynchronous function. When broker handled the precommit request
	 * for transaction and receipt to client, ITransactionPreCommitListener::onPreCommitResult() will be called.
	 * If the resultCode is 0, it indicate the transaction is ready to commit.
	 *
	 * In common, you can do local business here, then call IMessagingSession::commitTransaction() with transID,
	 * or call IMessagingSession::rollbackTransaction() on error.
	 *
	 * These transact operate func (commitTransaction, rollbackTransaction) can call immediately here in onPreCommitResult()
	 * Or, call in a asynchronous callback function is also accept.
	 *
	 * @param transID
	 *      precommited transaction ID
	 * @param transState
	 * 		the precommited transaction's state, see define of TransactionState.
	 * @param resultCode
	 *		transaction precommit operate result.
	 *
	 *
	 * @return None.
	 *
	 */
	virtual void onPreCommitResult(IMessagingSession *session, const TransactionID &transID, TransactionState transStateAtBroker, uint8_t resultCode) = 0;



	/**
	 * Transaction PreCommit operate result listener when timeout.
	 * IMessagingSession::preCommitTransaction() is a asynchronous function.
	 * if the broker receipt the transaction precommit is timeout,
	 * ITransactionPreCommitListener::onPreCommitTimeout() will be called.
	 *
	 * You can call IMessagingSession::rollbackTransaction() with the transID to rollback the transaction.
	 *
	 * These transact operate func (commitTransaction, rollbackTransaction) can call immediately here.
	 * Call in a asynchronous callback function is also accept.
	 *
	 * @param transID
	 *      precommited transaction ID
	 *
	 *
	 * @return None.
	 *
	 */
	virtual void onPreCommitTimeout(IMessagingSession *session, const TransactionID &transID) = 0;
};


class ITransactionSessionListener {
public:
	virtual ~ITransactionSessionListener() = default;

	//  Broker对未处理事务定时询问Producer, endState为出参，返回事务的最终状态。
	virtual protocol::TransactionFinalState onQueryTransOperate(const TransactionID& transID, TransactionState transStateAtBroker) = 0;
};

}} // namespace lightmq { namespace qmapi {

#endif /* ICONNECTION_H_ */
