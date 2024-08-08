////////////////////////////////////////////////////////////////////////////////
//
// QMAPIUtility.h
//
// Copyright (c) 2016 China Foreign Exchange Trade System and National Interbank Funding Center.
// Building 30, 1387 ZhangDong Road, Shanghai, China.
// All rights reserved.
//
// Data Exchange Platform (DEP) is the copyrighted,
// proprietary property of China Foreign Exchange Trade System and National
// Interbank Funding Center which retain all right, title and interest therein.
//
// Description:
//
////////////////////////////////////////////////////////////////////////////////
#include "IConnection.h"
//#include "QMAPITransaction.h"

namespace lightmq { namespace qmapi {

struct ITransactionPreCommitWaiter {
	virtual bool waitPrecommit(const std::chrono::milliseconds &ms) = 0;
	virtual std::shared_ptr<ITransactionPreCommitListener> getPreCommitListener() = 0;

	virtual /*inline*/ bool isPrecommitReceipted() const = 0;
	virtual /*inline*/ bool isPrecommitTimeout() const = 0;

	virtual IMessagingSession *getMessagingSession() const = 0;
	virtual const TransactionID &getTransID() const = 0;
	virtual uint8_t getResultCode() const = 0;
	virtual TransactionState getBrokerTransState() const = 0;
};

std::shared_ptr<ITransactionPreCommitWaiter> makeTransactionPreCommitWaiter();


}} // namespace lightmq { namespace qmapi {