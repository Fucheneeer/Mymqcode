////////////////////////////////////////////////////////////////////////////////
//
// QMAPIConfig.h
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

#ifndef QMAPICONFIG_H_
#define QMAPICONFIG_H_

#include <map>
#include "Properties.h"

namespace lightmq { namespace qmapi {

class QMAPIConfig {
public:
    std::string clientId;
    uint32_t connectTimeoutMS;
    uint32_t brokerTimeoutMS;
    uint32_t txMode;
    uint32_t txAckWinSize;
    uint32_t selectorMode;
    uint32_t userThreadMode;
    uint32_t userThreadCount;
    uint32_t userQueueBuffer;
    uint32_t limitConnection;
    uint32_t limitSession;
    uint32_t limitProducer;
    uint32_t limitConsumer;
    std::string tcpInterface;
    uint32_t tcpKeepAliveEnable;
    uint32_t tcpKeepAliveIdleTime;
    uint32_t tcpKeepAliveInterval;
    uint32_t tcpKeepAliveProbesCnt;
    std::string sessionStoreDirectory;
    uint32_t throttleCapacity;
    uint32_t clientIdWithHostname;
    uint32_t resubscribeRestryTimes;
    uint32_t resubscribeRestryIntervalMs;
    std::string accountPath;
    std::string accountEncryptionLibrary;
    std::string accountEencryptionkey;
	uint32_t maxTransactionMessages;
	uint32_t maxTransactionConsumerACKs;

	std::string defaultTransactionSession;
    dep::Properties queueProp;


    std::string libssl;     
    std::string caFile;
    std::string caPath;    
    std::string crtFile;
    std::string keyFile;
    std::string keyPasswd;
    std::string encCrtFile;
    std::string encKeyFile;
    bool enableSsl=false;
    int verify = 0;
    bool loadSSL = false; 

    uint32_t transAckCacheSize;

	bool loadLightmqConfig(const dep::Properties &prop);
	bool loadQueueConfig(const std::string &queuePropertiesFileName);
};


}} // namespace lightmq { namespace qmapi {

#endif /* QMAPICONFIG_H_ */
