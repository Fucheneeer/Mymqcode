////////////////////////////////////////////////////////////////////////////////
//
// QMAPISelectableClientSocket.h
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

#ifndef QMAPISELECTABLECLIENTSOCKET_H_
#define QMAPISELECTABLECLIENTSOCKET_H_

#include <mutex>
#include <condition_variable>
#include <queue>
#include <vector>
#include "TCPSocket.h"
#include "SSLTCPSocket.h"
#include "Thread.h"
#include "ByteBuffer.h"
#include "BrokerTypes.h"
#include "IMessage.h"
#include "ISelectable.h"
#include "ISelector.h"
#include "QMAPITypes.h"
#include "BaseSelectService.h"
#include "BrokerMessage.h"
#include "IBrokerMsgListener.h"
#include "QMAPIConfig.h"
#include "IThrottler.h"
#include "IConnectionManager.h"
#include "LightmqSelectableQueue.h"
#include "IDestination.h"
#include "InternalLightmqMessage.h"
#include "ReceiptManager.h"

namespace lightmq { namespace qmapi {

	class QMAPITransaction;


class QMAPISelectableClientSocket : public dep::ISelectable {
	friend class QMAPIConnection;
public:
    enum class SendStatus {
       SENT_OK = 0,
       SOCKET_UNAVAILABLE = 1,
       TXBUFFER_FULL = 2,
	   TIMEOUT = 3  // for requestAndReceipt()
    };

public:
	// dep::ISelectable overrides
	const int32_t getFileDescriptor() const override {
		return m_pSocket->getSocketDescriptor();
	}
	void onEvent(dep::SelectEvent& event) override;
	void onRead();
	void onWrite();


public:
	virtual ~QMAPISelectableClientSocket();
	QMAPISelectableClientSocket() = delete;

	static std::unique_ptr<QMAPISelectableClientSocket> getInstance(const QMAPIConfig& config, dep::ISelector* pSelector, std::unique_ptr<std::vector<BrokerIp>>& pBrokerips, IBrokerMsgListener& msgListener, IConnectionManager& manager);
	bool start();
	void stop();

    SendStatus sendConnect(const std::string& clientID, std::string& retCompositeClientId, const std::string& userID = "", const std::string& userPassword = "", const std::string& hostname = "", uint32_t pid = 0);
    SendStatus sendUnsubscribe(uint16_t subscriptionID, uint16_t receipt);
    SendStatus sendSubscribe(uint32_t ackWinSize, uint32_t transAckWinSize, const std::string& destination, uint16_t receipt, 
            uint16_t subscriptionId, uint8_t ackMode,const std::string& messageMatcher, const std::string &subNodeID, 
            std::string subscriptionName="", bool isGroupedSubscription=false);
    SendStatus sendAck(uint64_t sequence, SessionID sessionID, uint64_t endSequence, uint16_t subscriptionId, const std::string& destination);
	/**
     *
     * send expired ack use AckPto message
     */
	SendStatus sendExpiredAck(uint64_t sequence, SessionID sessionID, uint64_t endSequence, uint16_t subscriptionId, const std::string& destination);
    SendStatus sendSend(QMAPITransaction *trans, const std::string& sendClientName, const std::string& destination,
            const std::vector<InternalMessagePtr>& messages, uint64_t sequence, SessionID sessionID = 0);
    SendStatus sendSend(QMAPITransaction* trans, const std::string& sendClientName, const std::string& destination,
            const InternalLightmqMessage& message, uint64_t sequence, SessionID sessionID);
    SendStatus sendCreateService(const std::string& destination, uint8_t type, uint16_t receipt, IDestination::Persistency persistency, bool isExclusiveQueue = false, uint32_t ackdMsgRetxCap = 0, bool isSynMode = false, bool isLocalFirst = false);
    SendStatus sendQueryLastMessage(const std::string& destination, uint16_t receipt);
    SendStatus sendMessagePull(const std::string& destination, int64_t timeout);

	/**
	* send init producer message to broker to register producer
	* @param destination 
	*		Destination of a queue/topic
	* @param receipt 
	*		Receipt ID
	* @return status
	**/
	SendStatus sendRegisterProducerMessage(const std::string& destination, uint16_t receipt);
	//SendStatus resendConsumedMessage(const std::string& destination, const std::string& subscriptionName, const std::vector<std::string>& messageids, const std::vector < std::string>& subTopicClients, uint16_t receipt);

	/**
	* send transaction ack message to broker to record & resize ack window size
	* @param destination 
	*		Destination of a queue/topic
	* @return status
	**/
	SendStatus sendTransactionAckMessage(const std::string& destination);

	SendStatus sendLightmqMessage(protocol::ReceiptID rid, protocol::IMessage *msg, const std::shared_ptr<util::IReceiptListener> &lis);
	SendStatus sendPto(protocol::IMessage &msg);
	SendStatus send(dep::ByteBuffer& buffer);
	
//	SendStatus sendLightmqMessage(protocol::IMessage *msg, util::FutureReceiptMessage &futReceiptMsg) {
//		return sendLightmqMessage(futReceiptMsg.getListenReceiptID(), msg, futReceiptMsg.pretendAsSharedListener());
//	}

	SendStatus requestAndReceipt(protocol::IMessage *msg, std::unique_ptr<protocol::IMessage> &receiptMsg, int waitMilliSeconds);
	//SendStatus waitReceipt(protocol::ReceiptID rid, std::unique_ptr<protocol::IMessage> &receiptMsg, int waitMilliSeconds);

	std::string getLocalIP() const;
	std::string getIp() const;
	uint16_t getPort() const;

	util::ReceiptManager &getReceiptManager() {
		return m_receiptManager;
	}

	uint32_t getDefaultTTLByReceiptId(uint16_t receiptId){
		if (m_destinationDefaultTTLs.find(receiptId) == m_destinationDefaultTTLs.end()){
			return 0;
		}
		return m_destinationDefaultTTLs[receiptId];
	}
    QMAPISelectableClientSocket::SendStatus sendSuspendInfos(const std::vector<std::string>& destinations);
    QMAPISelectableClientSocket::SendStatus sendResumeInfos(const std::vector<std::string>& destinations);
	std::string getHostName() const{
		return m_hostname;
	}

protected:
	bool failover(std::atomic<bool>& connectionStarted);

    void onDisconnectionDetected();

private:
	QMAPISelectableClientSocket(const QMAPIConfig& config, dep::ISelector* pSelector, std::unique_ptr<std::vector<BrokerIp>>& pBrokerIps, IBrokerMsgListener& msgListener, IConnectionManager& manager);
	void run();
	uint8_t decodeMessage(dep::ByteBuffer& buffer);
	void getMiscInfo();
	void startFailover();

private:
	static constexpr auto USEC_PER_MSEC = 1000;
	static constexpr auto CHARBUFFSIZE = 256;

private:
	const QMAPIConfig& m_config;
	std::string m_ip;
	uint16_t m_port;
	std::unique_ptr<dep::TCPSocket> m_pSocket;
    IBrokerMsgListener& m_msgListener;
    dep::ByteBuffer m_rxBuffer;
    dep::ByteBuffer m_txBuffer;
    std::unique_ptr<dep::Thread> m_pThread;

    dep::ISelector* m_pSelector;

	std::mutex m_mtxTxBuffer;

	IConnectionManager& m_connectionManager;

	std::unique_ptr<std::vector<BrokerIp>>& m_pBrokerIps;
	uint32_t m_ipPos = 0;

	std::string m_localInterfaceIp;
	std::string m_cwdMD5;
	std::string m_hostname;

	std::unique_ptr<dep::Thread> m_pFailoverThread = nullptr;
    std::atomic<bool> m_connected{ false };

private:
    enum class JOB_TYPE{
        REGISTER_ON_WRITE,
        REGISTER_ON_READ,
        UNREGISTER_ALL,
    };
    struct InternalJob {
        JOB_TYPE jobType{JOB_TYPE::REGISTER_ON_WRITE};
    };

    void onInternalJob(InternalJob& job);
    void registerOnWrite();
    void registerOnRead();
    void unregisterAll();

    lightmq::util::LightmqSelectableQueue<InternalJob> m_selectableQueue;

	util::ReceiptManager m_receiptManager;

	std::unordered_map<uint16_t, uint32_t> m_destinationDefaultTTLs;
};

}} // namespace lightmq { namespace qmapi {

#endif /* QMAPISELECTABLECLIENTSOCKET_H_ */
