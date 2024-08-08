////////////////////////////////////////////////////////////////////////////////
//
// QMAPIConstant.h
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

#ifndef QMAPICONSTANT_H_
#define QMAPICONSTANT_H_

namespace lightmq { namespace qmapi {

class QMAPIConstant {
public:
	QMAPIConstant() = delete;
	~QMAPIConstant() = delete;

	static constexpr auto CLIENT_ID = "cfmqapi.client.id";
	static constexpr auto LOG_PATH = "dep.framework.log.sink.s_file.path";
	static constexpr auto CONNECT_TIMEOUT = "cfmqapi.connect.timeoutMS";
	static constexpr auto BROKER_TIMEOUT = "cfmqapi.timeoutMS";
	static constexpr auto TX_MODE = "cfmqapi.tx.mode";
	static constexpr auto RX_MSGDISPATCHWINDOWSIZE = "cfmqapi.rx.msgDispatchWindowSize";
	static constexpr auto RX_TRANSACKCACHESIZE = "lightmqapi.rx.transMsgCacheWindowSize";
	static constexpr auto RX_MODE = "lightmqapi.rx.mode";
	static constexpr auto SELECTOR_MODE = "cfmqapi.selector.mode";
	static constexpr auto USERTHREAD_MODE = "cfmqapi.userthread.mode";
	static constexpr auto USERTHREAD_COUNT = "cfmqapi.userthread.count";
	static constexpr auto USERQUEUE_BUFFER = "cfmqapi.userqueue.buffer";
	static constexpr auto LIMIT_CONNECTION = "cfmqapi.limit.connection";
	static constexpr auto LIMIT_SESSION = "cfmqapi.limit.session";
	static constexpr auto LIMIT_PRODUCER = "cfmqapi.limit.producer";
	static constexpr auto LIMIT_CONSUMER = "cfmqapi.limit.consumer";
	static constexpr auto TCP_INTERFACE = "lightmqapi.tcp.interface";
	static constexpr auto TCP_SENDBUFFERSIZE = "lightmqapi.tcp.sendBufferSize";
	static constexpr auto TCP_RECVBUFFERSIZE = "lightmqapi.tcp.recvBufferSize";
	static constexpr auto TCPKEEPALIVE_ENABLE = "cfmqapi.tcpKeepAlive.enable";
	static constexpr auto TCPKEEPALIVE_IDLETIME = "cfmqapi.tcpKeepAlive.idleTime";
	static constexpr auto TCPKEEPALIVE_INTERVAL = "cfmqapi.tcpKeepAlive.interval";
	static constexpr auto TCPKEEPALIVE_PROBESCNT = "cfmqapi.tcpKeepAlive.probesCnt";
	static constexpr auto SESSIONSTORE_DIRECTORY = "lightmqapi.sessionStore.directory";
	static constexpr auto THROTTLE_CAPACITY = "cfmqapi.throttle.capacity";
	static constexpr auto CLIENTID_WITH_HOSTNAME = "cfmqapi.clientIdWithHostname";

	static constexpr auto COMPRESSION_LIBRARY = "lightmqapi.compression.library";               /**< Configuration key to load compression library */
    static constexpr auto ENCRYPTION_LIBRARY = "lightmqapi.encryption.library";                /**< Configuration key to load encryption library */

	static constexpr auto RESUBSCRIBE_RETRY_TIMES = "cfmqapi.resubscribeRetryTimes";
	static constexpr auto RESUBSCRIBE_RETRY_INTERVALS_MS = "cfmqapi.resubscribeRetryIntervalMs";
	static constexpr auto CFG_MAX_TRANS_MESSAGES = "cfmqapi.transaction.maxMessages";
	static constexpr auto CFG_MAX_TRANS_CONSUMER_ACKS = "cfmqapi.transaction.maxConsumerACKs";
	static constexpr auto CFG_TRANSACTION_SESSION_DEFAULT = "cfmqapi.transaction.defaultSession";

    static constexpr auto ACCOUNT_PATH = "cfmqapi.accountPath";
    static constexpr auto ACCOUNT_ENCRYPTION_LIBRARY = "cfmqapi.account.encryption.library";
    static constexpr auto ACCOUNT_ENCRYPTION_KEY = "cfmqapi.account.encryption.key";
 
	static constexpr auto SSL_LIB = "cfmqapi.ssl.lib";
	
	static constexpr auto SSL_CA_FILE = "cfmqapi.ssl.caFile";
	static constexpr auto SSL_CA_PATH = "cfmqapi.ssl.caPath";
	static constexpr auto SSL_CRT_FILE = "cfmqapi.ssl.crtFile";
	static constexpr auto SSL_KEY_FILE = "cfmqapi.ssl.keyFile";
	static constexpr auto SSL_KEY_PASSWD = "cfmqapi.ssl.keyPasswd";
	static constexpr auto SSL_ENC_CRT_FILE = "cfmqapi.ssl.encCrtFile";
	static constexpr auto SSL_ENC_KEY_FILE = "cfmqapi.ssl.encKeyFile";
	static constexpr auto SSL_VERIFY = "lightmqapi.broker.ssl.verify";
	static constexpr auto SSL_ENABLE = "cfmqapi.ssl.enable";
	static constexpr auto DEFAULT_SSL_VERIFY = 0;  


	static constexpr auto DEFAULT_CONNECT_TIMEOUT = 1000;
	static constexpr auto DEFAULT_BROKER_TIMEOUT = 100;
	static constexpr auto DEFAULT_TX_MODE = 1;  // MessageSendingMode::SYNCHRONOUS
	static constexpr auto DEFAULT_TX_MSGDISPATCHWINDOWSIZE = 1;
	static constexpr auto DEFAULT_RX_MODE = 1;  // MessageReceivingAckMode::API_AUTO_ACKNOWLEDGE
	static constexpr auto DEFAULT_SELECTOR_MODE = 1;
	static constexpr auto DEFAULT_USERTHREAD_MODE = 2;
	static constexpr auto DEFAULT_USERTHREAD_COUNT = 5;
	static constexpr auto DEFAULT_USERQUEUE_BUFFER = 10000;
	static constexpr auto DEFAULT_LIMIT_CONNECTION = 1000;
	static constexpr auto DEFAULT_LIMIT_SESSION = 100;
	static constexpr auto DEFAULT_LIMIT_PRODUCER = 1000;
	static constexpr auto DEFAULT_LIMIT_CONSUMER = 1000;
	static constexpr auto DEFAULT_TCP_INTERFACE = "127.0.0.1";
	static constexpr auto DEFAULT_TCP_SENDBUFFERSIZE = 1048576;
	static constexpr auto DEFAULT_TCP_RECVBUFFERSIZE = 1048576;
	static constexpr auto DEFAULT_TCPKEEPALIVE_ENABLE = 1;
	static constexpr auto DEFAULT_TCPKEEPALIVE_IDLETIME = 3600;
	static constexpr auto DEFAULT_TCPKEEPALIVE_INTERVAL = 60;
	static constexpr auto DEFAULT_TCPKEEPALIVE_PROBESCNT = 9;
	static constexpr auto DEFAULT_SESSIONSTORE_DIRECTORY = ".";
	static constexpr auto DEFAULT_THROTTLE_CAPACITY = 10000;
	static constexpr auto DEFAULT_CLIENT_ID = "";
	static constexpr auto DEFAULT_CLIENTID_WITH_HOSTNAME = 0;
	static constexpr auto DEFAULT_MAX_TRANS_MESSAGES = 5000;
	static constexpr auto DEFAULT_MAX_TRANS_CONSUMER_ACKS = 5000;
	static constexpr auto MAX_TRANS_SESSION_ID_LEN = 95;

    static constexpr auto DEFAULT_RESUBSCRIBE_RETRY_TIMES = 5;
    static constexpr auto DEFAULT_RESUBSCRIBE_RETRY_INTERVALS_MS = 5000;
	
	static constexpr auto DEFAULT_TRANS_ACK_CACHE_SIZE = 0;
    static constexpr auto MAX_PROPERTIES_NUM = 256;
    static constexpr auto MAX_PROPERTY_STRING_LENGTH = 1024;
    static constexpr auto MAX_PROPERTY_KEY_LENGTH = 64;

	static constexpr auto QUEUE = "queue^";
	static constexpr auto TOPIC = "topic^";
	static constexpr auto QS = "^";
	static constexpr auto EXCLUSIVE = "^exclusive";
	static constexpr auto ACKDMSGRETXCAP = "^ackdMsgRetxCap";
    static constexpr auto ISSYNCMODE = "^isSyncMode";
	//static constexpr auto PROCESSORCHAIN_TYPE = "^receiveProcessChain";
	static constexpr auto PROCESSORCHAIN_NODE_DECRYPT = "decrypt";
	static constexpr auto DEFAULT = "default";
	static constexpr auto TRUE = "true";
	static constexpr auto FALSE = "false";
	static constexpr auto ON = 1;
	static constexpr auto OFF = 0;
	static constexpr auto PROCESSCHAIN = "^processChain";


    //static constexpr auto ENCRYPTION_ENABLE = "^encryption.enable";
    static constexpr auto COMPRESSION_PORCESSOR = "compression";
    static constexpr auto ENCRYPTION_PORCESSOR = "encryption";
    static constexpr auto ENCRYPTION_KEY = "^encryption.key";
    static constexpr auto ENCRYPTION_ALG = "^encryption.alg";
    static constexpr auto ENCRYPTION_ENGINE_PATH = "^encryption.path";
    static constexpr auto COMPRESSION_ENGINE_PATH = "^compression.path";
    static constexpr auto COMPRESSION_ALG = "^compression.alg";
    static constexpr auto COMPRESSION_MINLEN = "^compression.minlen";
    static constexpr auto COMPRESSION_LEVEL = "^compression.level";
    
	static constexpr auto DEFAULT_ACKDMSGRETXCAP = "10000";
	using QMAPI_RETCODE = uint8_t;
	static constexpr QMAPI_RETCODE QMAPI_OK = 0;
	static constexpr QMAPI_RETCODE QMAPI_BROKER_TIMEOUT = 1;
	static constexpr QMAPI_RETCODE QMAPI_EXCEED_BANDWIDTH = 2;
	static constexpr QMAPI_RETCODE QMAPI_SERVICE_SUSPENDED = 3;
	static constexpr QMAPI_RETCODE QMAPI_UNSUBSCRIPTION_FAILURE = 4;
	static constexpr QMAPI_RETCODE QMAPI_RESUBSCRIPTION_FAILURE = 5;
	static constexpr QMAPI_RETCODE QMAPI_BROKER_RETURNED_ERROR = 6;
	static constexpr QMAPI_RETCODE QMAPI_SERVICE_FAILOVER = 7;
	static constexpr QMAPI_RETCODE QMAPI_SEND_ERROR = 8;
	static constexpr QMAPI_RETCODE QMAPI_TXBUFFER_OVERFLOW = 9;
	static constexpr QMAPI_RETCODE QMAPI_ILLEGAL_DESTINATION_NAME = 10;
	static constexpr QMAPI_RETCODE QMAPI_CREATE_CHANNEL_ERROR = 11;
	static constexpr QMAPI_RETCODE QMAPI_MESSAGE_TOO_BIG = 12;
	static constexpr QMAPI_RETCODE QMAPI_PROCESS_FAILED = 13;
	static constexpr QMAPI_RETCODE QMAPI_SESSION_IS_NOT_TRANSACT = 14;
	static constexpr QMAPI_RETCODE QMAPI_TRANSACT_IS_EMPTY = 15;
	static constexpr QMAPI_RETCODE QMAPI_TRANSACT_TOO_MUCH_MESSAGS = 16;
	static constexpr QMAPI_RETCODE QMAPI_TRANSACT_ILLEGAL_OPERATE = 17;
	static constexpr QMAPI_RETCODE QMAPI_TRANSACT_NOT_EXIST = 18;
	static constexpr QMAPI_RETCODE QMAPI_DISTRIBUTE_TRANSACT_MUST_HAS_A_TRANS_LISTENER = 19;
	static constexpr QMAPI_RETCODE QMAPI_TRANSACT_IS_BUSY = 20;	
    static constexpr QMAPI_RETCODE QMAPI_DISPATCHKEY_TOO_LONG = 21;
	static constexpr QMAPI_RETCODE QMAPI_TIMEOUT = 22;
	static constexpr QMAPI_RETCODE QMAPI_DISCONNECT = 23;
	static constexpr QMAPI_RETCODE QMAPI_QUIT = 24;
	static constexpr QMAPI_RETCODE QMAPI_PARAMERROR = 25;
	static constexpr QMAPI_RETCODE QMAPI_NOMESSAGE = 26;
    static constexpr QMAPI_RETCODE QMAPI_ADD_PROPERTY_OUT_LIMIT = 27;
    static constexpr QMAPI_RETCODE QMAPI_ADD_PROPERTY_KEY_TOO_LONG = 28;
    static constexpr QMAPI_RETCODE QMAPI_ADD_PROPERTY_STRING_TOO_LONG = 29;
    static constexpr QMAPI_RETCODE QMAPI_ADD_PROPERTY_ERROR = 30;
    static constexpr QMAPI_RETCODE QMAPI_JMS_STRING_PROPERTY_TOO_LONG = 31;
    static constexpr QMAPI_RETCODE QMAPI_WRONG_DELIVERY_MODE_VALUE = 32;
    static constexpr QMAPI_RETCODE QMAPI_WRONG_PRIORITY_MODE_VALUE = 33;
    static constexpr QMAPI_RETCODE QMAPI_PROPERTY_NULL_KEY = 34;
    static constexpr QMAPI_RETCODE QMAPI_PROPERTY_NULL_VALUE = 35;
	static constexpr QMAPI_RETCODE QMAPI_SOCKET_CLOSED = 36;
    static constexpr QMAPI_RETCODE QMAPI_GROUP_ALREADY_EXIST = 98;
    static constexpr QMAPI_RETCODE QMAPI_GROUP_NOT_EXIST = 99;
	static constexpr QMAPI_RETCODE QMAPI_JMS_EXCEPTION = 100;


	static constexpr uint16_t FEATURE_FLAG_NONE{0}; /**< Value indicate no feature */
    static constexpr uint16_t FEATURE_FLAG_LATENCY_SAMPLING{1}; /**< Enable latency sampling if set */
    static constexpr uint16_t FEATURE_FLAG_COMPRESSION{2};      /**< Enable compression if set */
    static constexpr uint16_t FEATURE_FLAG_ENCRYPTION{4};      /**< Enable encryption if set */

	static constexpr QMAPI_RETCODE QMAPI_CONNECTION_OK = 0;
	static constexpr QMAPI_RETCODE QMAPI_NOT_CONNECTION = 1;
	static constexpr QMAPI_RETCODE QMAPI_DISCONNECTION = 2;
	static constexpr QMAPI_RETCODE QMAPI_CONNECTION_TIMEOUT = 3;
};

}} // namespace lightmq { namespace qmapi {

#endif /* QMAPICONSTANT_H_ */
