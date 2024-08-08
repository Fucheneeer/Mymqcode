////////////////////////////////////////////////////////////////////////////////
//
// QMAPIClientSocket.h
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

/*
#ifndef QMAPICLIENTSOCKET_H_
#define QMAPICLIENTSOCKET_H_

#include <mutex>
#include <condition_variable>
#include <queue>
#include "TCPSocket.h"
#include "Thread.h"
#include "ByteBuffer.h"
#include "BrokerTypes.h"
#include "IMessage.h"


namespace lightmq { namespace qmapi {


class QMAPIClientSocket {
public:
	enum class ClientMessageType : uint8_t {
	    onClientMessage = 5,
	    onClientDisconnect = 6
	};

	struct TestClientMessage {
	    ClientMessageType type;
	    uint32_t testClientID;
	    std::unique_ptr<protocol::IMessage> pMessage; // only when it is not onClientDisconnect
	    std::unique_ptr<std::vector<std::string>> pMessageVector;
	};

public:
	virtual ~QMAPIClientSocket() = default;
	QMAPIClientSocket() = delete;

	// static std::unique_ptr<QMAPIClientSocket> getInstance(const std::string& ip, uint16_t port);
	void stop();

	void sendConnect(const std::string& clientId);
	void sendUnsubscribe(uint16_t subscriptionID, uint16_t receipt);
	void sendSubscribe(const std::string& destination, uint16_t receipt, std::string subscriptionName="");
	void sendAck(uint64_t sequence, SessionID sessionID);
	void sendSend(const std::string& destination, const std::string& body, uint64_t correlationID);
	void sendCreateService(const std::string& destination, uint8_t type, uint16_t receipt);

	// DEBUG
	bool getClientMessageSync(uint8_t messageType);
	bool getClientMessageSync2(uint8_t messageType, QMAPIClientSocket::TestClientMessage& clientMsg);

private:
	QMAPIClientSocket(const std::string& ip, uint16_t port);
	void run();

private:	// handle received messages
	void onReceiveClientMessage(TestClientMessage&& message);
	bool waitReceiveClientMessage(uint64_t waitTime = 1000);	// wait for 1000mS at most
	bool getClientMessage(TestClientMessage& msg);

	bool decode(dep::ByteBuffer& buffer, uint32_t testClientID);
	uint8_t decodeSync(dep::ByteBuffer& buffer, TestClientMessage& clientMessage, uint32_t testClientID);

private:
	uint32_t m_id{0};
    std::unique_ptr<dep::TCPSocket> m_pSocket;
    dep::ByteBuffer m_rxBuffer{2000};
    std::unique_ptr<dep::Thread> m_pThread;

    bool m_run;

    std::queue<TestClientMessage> m_ClientMessageQueue;
    std::mutex m_ClientMutex;
    std::condition_variable m_ClientCV;
};

}} // namespace lightmq { namespace qmapi {

#endif // QMAPICLIENTSOCKET_H_
*/
