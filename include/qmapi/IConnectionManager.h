////////////////////////////////////////////////////////////////////////////////
//
// IConnectionManager.h
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

#ifndef ICONNECTIONMANAGER_H_
#define ICONNECTIONMANAGER_H_


namespace lightmq { namespace qmapi {

class IConnectionManager {
public:
	virtual ~IConnectionManager() = default;

	virtual bool failover() = 0;

	virtual bool isInFailover() = 0;
	virtual bool setFailoverFlag(bool failover) = 0;

	virtual void onDisconnect() = 0;
};


}} // namespace lightmq { namespace qmapi {


#endif /* ICONNECTIONMANAGER_H_ */
