////////////////////////////////////////////////////////////////////////////////
//
// IConnectionFactory.h
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


#ifndef ICONNECTIONFACTORY_H_
#define ICONNECTIONFACTORY_H_

#include <memory>

#include "QMAPITypes.h"
#include "IConnection.h"
#include <string>

namespace lightmq { namespace qmapi {

class IConnectionFactory {
public:
    virtual ~IConnectionFactory() = default;


    /**
     * Creates a connection. The connection is created in stopped mode.
     * No messages will be delivered until the Connection.start method is
     * explicitly called.
     *
     * @return A pointer to a connection object
     *
     * @throws Exception if an internal error occurs while creating the Connection.
     */
    virtual IConnection* createConnection(const std::string& clientID = "") = 0;
};

}} // namespace lightmq { namespace qmapi {

#endif /* ICONNECTIONFACTORY_H_ */
