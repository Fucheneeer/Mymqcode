////////////////////////////////////////////////////////////////////////////////
//
// QMAPIConnectionFactory.h
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

#ifndef QMAPICONNECTIONFACTORY_H_
#define QMAPICONNECTIONFACTORY_H_
#include <mutex>
#include "IConnectionFactory.h"
#include "ISelectService.h"
//#include "QMAPIConnection.h"
#include "Properties.h"
#include "QMAPIConfig.h"
#include "ISeverityLog.h"
#include "Uri.h"

namespace lightmq { namespace qmapi {

struct ConnectionFactoryStatus;
class QMAPIConnectionFactory : public IConnectionFactory {
public:
	//typedef struct CONNECTIONFACTORYSTATUS {
	//	uint32_t numberOfConnections;
	//	std::vector<QMAPIConnection::ConnectionStatus> connectionStatusVector;
	//} ConnectionFactoryStatus;

public:
    virtual ~QMAPIConnectionFactory() override;

    /**
     * Creates a connection. The connection is created in stopped mode.
     * No messages will be delivered until the Connection.start method is
     * explicitly called.
     *
     * @param clientId
     * 	Client ID
     * @param brokerIPs
     *  A vector of broker IPs.  More than one is needed for failover purpose.
     *
     * @return A pointer to a connection object
     *
     * @throws Exception if an internal error occurs while creating the Connection.
     */
    IConnection* createConnection(const std::string& clientID = "") override;

    /**
     * Get status of the connection factory
     *
     * @param status
     * 	ConnectionFactoryStatus object to be filled
     *
     */
    void getStatus(ConnectionFactoryStatus& status);

public:
    /**
     * Static method that is used to create a provider specific connection
     * factory.  The provider implements this method in their library and
     * returns an instance of a ConnectionFactory derived object.
     *
     * @param propertiesFileName
     *  QMAPI properties file
     * @param queuePropertiesFileName
     *  Queue properties file
     *
     * @return A pointer to a connection factory which will be owned by the caller
     *
     */
    static std::unique_ptr<IConnectionFactory> getInstance(URI brokerURL);

    static bool setConfiguration(const std::string& propertiesFileName, std::string queuePropertiesFileName = "");

    static void setLogger(dep::ISeverityLog* pLogger);

    static QMAPIConfig getConfiguration() {
        return m_config;
    }

	static void initializeLog(const dep::Properties &prop);
	static dep::ISeverityLog *getModuleSeverityLog();

private:
    QMAPIConnectionFactory(URI brokerURL);

    static void dumpConfig(QMAPIConfig& config);
private:
    std::vector<std::unique_ptr<IConnection>> m_connections;
    std::vector<BrokerIp> m_brokers;
    static dep::Properties m_properties;
    static QMAPIConfig m_config;
    static std::once_flag onceFlag;
    static dep::ISeverityLog* m_pLogger;
};

}} // namespace lightmq { namespace qmapi {

#endif /* QMAPICONNECTIONFACTORY_H_ */
