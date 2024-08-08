////////////////////////////////////////////////////////////////////////////////
//
// QMAPITransactionCaller.h
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
#ifndef LOCALTRANSACTIONCALLER_H_
#define LOCALTRANSACTIONCALLER_H_

#include "QMAPITransaction.h"

namespace lightmq { namespace qmapi {

class QMAPITransaction;
class TransactionSingleManager;

class QMAPITransactionCaller {
public:
	QMAPITransactionCaller()
		: m_transactionMana(nullptr)
	{}

	void setTransactionSingleMana(TransactionSingleManager* localTransaction) {
		m_transactionMana = localTransaction;
	}

	bool inTransaction() const {
		return m_transactionMana && m_transactionMana->isEnableTrans();
	}

	const std::shared_ptr<QMAPITransaction> getCurrentTransaction() {
		return m_transactionMana ? m_transactionMana->getCurrentTrans() : std::shared_ptr<QMAPITransaction>();
	}

	// uint64_t getCurrentTransID() {
	// 	if (m_transactionMana && m_transactionMana->getCurrentTrans()) {
	// 		return m_transactionMana->getCurrentTrans()->getTransID();
	// 	}
	// 	return 0ull;
	// }

	// std::shared_ptr<QMAPITransaction> touchTransaction(bool allowDiscardPrevTrans) {
	// 	if (m_transactionMana) {
	// 		return m_transactionMana->touchTransaction(allowDiscardPrevTrans);
	// 	} else {
	// 		return std::shared_ptr<QMAPITransaction>();
	// 	}
	// }

	QMAPIConstant::QMAPI_RETCODE sendTransactionMessage(const std::string &destination, const std::string &providerID,
														InternalLightmqMessage **msgBuf, int msgCount)
	{
		if (m_transactionMana) {
			return m_transactionMana->sendTransactionMessage(destination, providerID, msgBuf, msgCount);
		}

		return QMAPIConstant::QMAPI_SESSION_IS_NOT_TRANSACT;
	}

//	bool regTransactionMessage(const ProviderMessageID &msgID, size_t size) {
//		if (m_transactionMana) {
//			return m_transactionMana->regTransactionMessage(msgID, size);
//		}
//		return false;
//	}

	bool regConsumerAck(const DestinationMessageID &destinationMessageID, const std::string &subscriptionName, uint16_t subscriptionId, bool allowDiscardPrevTrans) {
		if (m_transactionMana) {
			return m_transactionMana->regConsumerAck(destinationMessageID, subscriptionName, subscriptionId, allowDiscardPrevTrans);
		}
		return false;
	}

	//template <typename... T>
	//bool emplaceJobSend(T... args) {
	//	if (m_currentTransaction == nullptr)
	//		return false;
	//	return m_currentTransaction->emplaceJobSend(args...);
	//}


	//bool checkMessageIsRollbacked(const std::string &subscriptionName, const std::string &destination, MessageID brokerMsgID, bool unRegister = true) {
	//	if (m_transactionMana) {
	//		return m_transactionMana->getRollbackedAckMana().checkMessageIsRollbacked(subscriptionName, destination, brokerMsgID, unRegister);
	//	}
	//	return false;
	//}

protected:
	TransactionSingleManager* m_transactionMana;
};

}} // namespace lightmq { namespace qmapi {

#endif  //  !QMAPILOCALTRANSACTION_H_
