////////////////////////////////////////////////////////////////////////////////
//
// QMAPIConnection.h
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

#ifndef QMAPICONNECTION_H_
#define QMAPICONNECTION_H_

#include <memory>
#include <vector>
#include <unordered_map>
#include "IConnection.h"
#include "ISelector.h"
#include "IMessagingSession.h"
#include "QMAPITypes.h"
#include "QMAPISession.h"
#include "QMAPISelectableClientSocket.h"
#include "QMAPIConfig.h"
#include "IBrokerMsgListener.h"
#include "IIdGenerator.h"
// #include "TimerService.h"
#include "IConnectionManager.h"
#include "SessionScheduleUtility.h"
#include "ModuleManager.h"
#include "ICryptoFactory.h"
#include "ISymmetricEncryptionEngine.h"

namespace lightmq { namespace qmapi {

class TransactionConnection;

class QMAPIConnection : public IConnection, public IBrokerMsgListener, public IIdGenerator, public IConnectionManager, public lightmq::util::ISessionChangeListener {
public:
    class ConnectionStatus {
    public:
        std::vector<BrokerIp> listOfBrokers;
        std::string conectedBrokerIp;
        uint16_t connectedBrokerPort;
        std::string clientId;
        bool isConnected;
        bool isSuspended;
        uint32_t numberOfSessions;
        std::vector<QMAPISession::SessionStatus> sessionStatusVector;
    };

    struct AccountFileInfo {
        std::string user;
        std::string plainPassword;
        //std::string encryptedPassword;
        std::string errorConditions;
        //bool isEncrypted;
    };
    class LastMessage {
    public:
        MessageID messageId;
        dep::ByteBuffer message;

        LastMessage() {
        }

        LastMessage(const LastMessage& o) {
            messageId = o.messageId;
            message.clear();
            message.put(o.message.getArray()+o.message.getPosition(), o.message.remaining());
            message.flip();
        }

        LastMessage& operator=(const LastMessage& o) {
            messageId = o.messageId;
            message.clear();
            message.put(o.message.getArray()+o.message.getPosition(), o.message.remaining());
            message.flip();
            return *this;
        }
    };

public:
	enum SelectorMode {
		POLL = 1,
		EPOLL
	};

public:
    virtual ~QMAPIConnection() override;
    QMAPIConnection() = delete;

    /**
     * Creates a connection which is owned by the caller. The connection is created in stopped mode.
     * No messages will be delivered until the Connection.start method is
     * explicitly called.
     *
     * @param config
     * 	QMAPI configuration
     * @param brokerIPs
     *  A vector of broker IPs.  More than one is needed for failover purpose.
     *
     * @return A pointer to a connection object
     *
     */
    static std::unique_ptr<QMAPIConnection> getInstance(const QMAPIConfig& config, const std::string& clientId, std::vector<BrokerIp>& brokerIPs);

    /**
     * Connect to the first available broker
     */
    bool start(IConnectionListener* pListener = nullptr) override;

    /**
     * Disconnect from the connected broker
     */

    bool stop() override;

    /**
     * An alias of stop except that it also reset the connected status
     */
    void close() override;

    /**
     * Return the connection status of previously called start()
     * Due to failover, a started connection does not mean it is connceted to a broker
     * Use isConnected() to check if a connection is currently connected to a broker
     *
     * @return connection status
     */
    bool isStarted() override {
        return m_started;
    }
    /**
     * Return the connection status of previously called close()
     *
     * @return connection status
     */
    bool isClosed() override {
        return !m_started && !m_connected;
    }


    /**
     * Return the connection status of previously called start()
     *
     * @return connection status
     */
    bool isConnected() override {
    	return m_connected;
    }

    /**
     * Suspend a connection without disconnecting from the broker
     *
     * @return QMAPI_RETCODE from suspending a connection and its sessions
     */
    QMAPIConstant::QMAPI_RETCODE suspend() override;

    /**
     * Resume a connection
     *
     * @return QMAPI_RETCODE from resuming a connection and its sessions
     */
    QMAPIConstant::QMAPI_RETCODE resume() override;


    /**
     * Creates a new Session with configuration from QMAPI properties
     *
     * @return a pointer to a session which is owned by QMAPI
     */
    IMessagingSession* createSession(IMessagingSession::MessageReceivingAckMode rxAckMode = IMessagingSession::MessageReceivingAckMode::API_AUTO_ACKNOWLEDGE) override;

    /**
     * Creates a new Session with a list of explicit user mode parameters
     * which overrides the QMAPI properties
     *
     * @param threadMode
     *   single thread, thread per queue/topic or multithreaded
     *
     * @return a pointer to a session which is owned by QMAPI
     */
    IMessagingSession* createSession(IMessagingSession::UserThreadingMode threadMode) override;

	/**
	 * Creates a new transaction Session to work for this Connection
	 *
	 * @param transSession
	 *     trans session is a container to hold transactions.
	 *     After transactions precommited, if commit or rollback is not called in configured time,
	 *     broker will query transaction's session, to ask the next operate, commit or rollback?
	 *     The query is call by the param transSessionListener.
	 *     So, these params, transSession and transSessionListener must be both set or none set.
	 *
	 *     The real transaction session will be <generate-prefix>-<transSession>
	 *     <generate-prefix> is generate by hostname and lightmq binary path.
	 *
	 * @param transSessionListener
	 *     The listener to query timeout precommit transaction, is it commit or rollback.
	 *
	 * @return a pointer to session
	 *
	 * @throws CMSException
	 */
	IMessagingSession* createTransactionSession(const std::shared_ptr<ITransactionSessionListener> &transSessionListener, const std::string &transSessionID) override;

    /**
     * Get status of the connection
     *
     * @param status
     * 	ConnectionStatus object to be filled
     *
     */
	void getStatus(ConnectionStatus& status);

	void onDisconnect() override;

	void setConnectionListener(IConnectionListener* pListener) override;

	std::shared_ptr<ITransactionSessionListener> getTransSessionListener(const std::string &transSessionID);
	
public:
	std::shared_ptr<TransactionConnection> getTransactionConnection();

	const std::string & getClientID() {
		return m_clientId;
	}

	std::weak_ptr<QMAPISelectableClientSocket> getClientSocket() {
		return m_pSocket;
	}

	const QMAPIConfig& getConfig() const {
		return m_config;
	}

	TransactionPool& getTransactionPool() {
		return m_transPool;
	}

    uint64_t getBrokerTimeDiff() const{
		return m_brokerTimeDiff;
	}

    void setBrokerTimeDiff(uint64_t timeDiff) {
		m_brokerTimeDiff = timeDiff;
	}

protected:
	//
	// IIDGenerator interfaces
	//

    /**
     * Get next sequential connection-wide Subscription ID
     *
     * @return Subscription ID
     */
    uint16_t getSubscriptionId() override;

    /**
     * Get next sequential connection-wide Receipt ID
     *
     * @return Receipt ID
     */
    uint16_t getReceiptId() override;

    /**
     * Get MessageID by destination, and advance MessageID from the number of messages
     *
     * @param destination
     *  Destination of the msssages
     * @param msgs
     *  Messages to count for the next message ID
     *
     * @return a MessageID to be used to send with the messages
     */
    MessageID getMessageID(const std::string& destination, int32_t messageCount = 1) override;
    void rollbackMessageID(const std::string& destination, int32_t messageCount = 1) override;



    /**
     * Get MessageID by destination for recovery purpose
     *
     * @param destination
     *  Destination
     *
     * @return  MessageID
     */
    MessageID getMessageIDNoAdvance(const std::string& destination) override;

    /**
     * Get last sent message by Receipt ID
     *
     * @param receiptId
     *  Receipt ID previously used to query broker for the last sent message
     * @param messageId
     *  Message ID to return
     * @param message
     *  Message to return
     *
     * @return if a message can be returned with the supplied Receipt ID
     */
    bool getLastMessage(uint16_t receiptId, MessageID& messageId, dep::ByteBuffer& message) override;
    // get MessageID by receiptID and initialize the (destination=>messageID) map

    /**
     * Initiate Message ID map per destination
     *
     * @param destination
     *  Destination
     * @param receiptId
     *  Receipt ID previously used to query broker for the last sent message
     *
     * @return last sent Message ID of the destination
     */
    MessageID initWithBrokerMessageID(const std::string& destination, uint16_t receiptId) override;
    void initFailoverMessageID(const std::string& destination, MessageID& msgId) override;

    /**
     * Process a broker message
     *
     * @param msg
     *  A broker message
     */
    void onBrokerMessage(BrokerMessage& msg) override;
	void onBrokerMessage2(std::unique_ptr<protocol::IMessage> &&msg) override;
	void onBrokerMessage_TRANSACT_QUERY(std::unique_ptr<protocol::IMessage> &&msg);

    //
    // IConnectionManager interfaces
    //

    /**
     * Failover a connection
     *
     * @return if a failover has been started
     */
	bool failover() override;

	/**
     * Check if the connection is in a failover
     *
     * @return if the connection is in a failover
     */
	bool isInFailover() override;
    /**
     * Set the failover flag
     *
     * @param failover
     *  Failover flag
     *
     * @return previous failover flag
     */
	bool setFailoverFlag(bool failover) override;

    std::recursive_mutex& getApiMutex() override {
    	return m_mtxApi;
    }

    void initAccountInfo(const std::string& filePath);

private:
    QMAPIConnection(const QMAPIConfig& config, const std::string& clientId, std::vector<BROKERIP>& brokerIPs);
	void run();
    void onSessionChanged(SessionID oldestValidSessionID, SessionID newSessionID) override;

private:
    const std::string AUTHENTICATION_INFO_DELIMITER{ "|" };
    const uint32_t AUTHENTICATION_INFO_ENTRY_NUMBER{ 3 };
    const std::string ENCRYPTION_FLAG_YES{ "Y" };
    const std::string ENCRYPTION_FLAG_NO{ "N" };
    const std::string IMPL_ENCRYPTION_LIB{ "sm4" };
    const std::string KEY_ENCRYPTION{ "EncryptionKey" };

private:
    std::atomic<bool> m_connected{ false };      // go CONNECT_ACK from broker ?
    std::atomic<bool> m_brokenSocket;   // onDisconnect() called from socket ?
    std::atomic<bool> m_started{ false };
    std::atomic<bool> m_closed{ true };

    std::string m_clientId = std::string();
    std::string m_givenClientId = std::string();

    std::unique_ptr<std::vector<BrokerIp>> m_pBroker;
    std::vector<std::unique_ptr<QMAPISession>> m_sessions;

    std::shared_ptr<QMAPISelectableClientSocket> m_pSocket;
    std::unique_ptr<dep::ISelectService> m_pSelectService;

	std::mutex m_mtxSessions;
	std::mutex m_mtxSeqId;
	std::mutex m_mtxConnect;

	// An entry only exists when there is a Producer of that destination
	// A new Producer of a Destination should always query for the last sent MessageID (from this connection)
	// Used in processing MESSAGE_TYPE_RECEIPT_LAST_MESSAGE
	std::map<uint16_t, LastMessage> m_lastMsgIdPerReceiptId;
	std::map<std::string, LastMessage> m_msgIdPerDestination;	// MessageID per destination

	uint16_t m_subscriptionId = 0; 	// subscription Id per connection
	uint16_t m_receiptId = 0;	// receipt Id used in queue/topic creation and subscription

	const QMAPIConfig m_config;
	std::condition_variable m_connectCV;
	bool m_suspended = false;

	std::mutex m_mtxFailover;
	std::unique_ptr<dep::Thread> m_pFailoverThread = nullptr;
    std::unique_ptr<dep::Thread> m_pDisconnectNotifyThread = nullptr;
	bool m_failover;

	SessionID m_sessionId;
	std::unique_ptr<lightmq::util::SessionScheduleUtility> m_pSessionScheduleUtil = nullptr;
	std::mutex m_mtxSessionId;
	std::recursive_mutex m_mtxApi;

	IConnectionListener* m_pConnectionListener;

    dep::ModuleManager<ICryptoFactory> m_moduleManager;
    std::unique_ptr<ISymmetricEncryptionEngine> m_pEncryptionEngine{ nullptr };
    AccountFileInfo m_accountFileInfo;

	//  Transaction conncerned:
	TransactionPool m_transPool;

	std::mutex m_mtxTransConn;
	std::shared_ptr<TransactionConnection> m_transConn;

	std::mutex m_mtxTransSessionListener;
	std::unordered_map<std::string, std::shared_ptr<ITransactionSessionListener>> m_transSessionListeners;
	bool registerTransSessionListener(const std::string &transSessionID, const std::shared_ptr<ITransactionSessionListener> &transSessionListener);
	bool reportTransSessions(const std::set<std::string> &sessions);

    int64_t m_brokerTimeDiff = 0;   // current time - broker time

};


struct ConnectionFactoryStatus {
	uint32_t numberOfConnections;
	std::vector<QMAPIConnection::ConnectionStatus> connectionStatusVector;
};

}} // namespace lightmq { namespace qmapi {





#endif /* QMAPICONNECTION_H_ */
