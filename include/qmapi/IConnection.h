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

#ifndef ICONNECTION_H_
#define ICONNECTION_H_

#include <memory>
#include "IMessagingSession.h"
#include "IConnectionListener.h"
#include "ITransactionListener.h"

namespace lightmq { namespace qmapi {

class IConnection {
public:
    virtual ~IConnection() = default;

    /**
     * Closes this connection as well as any Sessions created from it
     * (and those Sessions' consumers and producers).
     *
     */
    virtual void close() = 0;
    /**
     * Connect to the broker
     *
     */
    virtual bool start(IConnectionListener* pListener = nullptr) = 0;
    /**
     * Stop this connection (close the sessions)
     *
     */
    virtual bool stop() = 0;

    /**
     * Creates a new Session to work for this Connection using the
     * specified acknowledgment mode
     *
     * @param threadMode
     * 		  user thread mode
     *
     * @throws CMSException
     */
    virtual IMessagingSession* createSession(IMessagingSession::UserThreadingMode threadMode) = 0;

    /**
     * Creates a new Session to work for this Connection using the
     * specified acknowledgment mode
     *
     * @param rxAckMode
     *        the Acknowledgment Mode to use (default = AUTO_ACKNOWLEDGE)
     *
     * @throws CMSException
     */
    virtual IMessagingSession* createSession(IMessagingSession::MessageReceivingAckMode rxAckMode = IMessagingSession::MessageReceivingAckMode::API_AUTO_ACKNOWLEDGE) = 0;

    /*
     * Check if the connection is connected to a broker
     *
     * @return
     * 	true	connected to a broker
     * 	false	not connected
     *
     */
    virtual bool isConnected() = 0;

    virtual bool isStarted() = 0;
    virtual bool isClosed() = 0;


    virtual uint8_t suspend() = 0;
    virtual uint8_t resume() = 0;
	virtual void setConnectionListener(IConnectionListener* pListener) = 0;


	/**
	 * Creates a new transaction Session to work for this Connection
	 *
	 * When send messages or consume messages in a transaction session, every thing is like a common session.
	 * But all the operate take no effect in broker server until you call commit function,
	 * it is stored in the session's current transaction
	 * You can also call rollback to cancel all the message operate(send or consume).
	 *
	 * one session has one current transaction, no matter how many thread you used to operate the session.
	 * It start when you send/consume the first message, and end when you commit or rollback.
	 * If you want call transaction in parallel, you need multi transaction Session.
	 *
	 * transaction session has 2 work method:
	 *          Local transaction: Synchronous call,
	 *     Distribute transaction: Asynchronous call, before commit, you need precommit.
	 *                             If you don't commit or rollback after precommit, broker will query for your operate.
	 *
	 * local transaction functions:
	 *     commitLocalTransaction()
	 *     rollbackLocalTransaction()
	 *
	 * distribute transaction functions:
	 *     preCommitTransaction()
	 *     commitTransaction()
	 *     rollbackTransaction
	 *     listener: ITransactionPreCommitListener
	 *     listener: ITransactionSessionListener
	 *
	 * @param transSessionID
	 *     trans session is a container to hold transactions.
	 *     After transactions precommited, if commit or rollback is not called in configured time,
	 *     broker will query transaction's session, to ask the next operate, commit or rollback?
	 *     The query is call by the param transSessionListener.
	 *
	 *     if transSessionID is empty, the function will use a default value(generate by hostname, work path...)
	 *     if two session use same transSessionID(eg: both "kkk" or both empty), transSessionListener must be same too.
	 *
	 * @param transSessionListener
	 *     The listener to query timeout precommit transaction, is it commit or rollback.
	 *     If transSessionListener is empty, the session can not use distribute transaction
	 *      - this meant you can not call IMessagingSession::preCommitTransaction().
	 *
	 * @return a pointer to session.
	 *
	 * @throws CMSException
	 */
	virtual IMessagingSession* createTransactionSession(const std::shared_ptr<ITransactionSessionListener> &transSessionListener, const std::string &transSessionID = "") = 0;
};

}} // namespace lightmq { namespace qmapi {

#endif /* ICONNECTION_H_ */
