////////////////////////////////////////////////////////////////////////////////
//
// IConnectionListener.h
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

#ifndef ICONNECTIONLISTENER_H_
#define ICONNECTIONLISTENER_H_

namespace lightmq { namespace qmapi {

class IConnectionListener {
public:
    virtual ~IConnectionListener() = default;

	virtual void onConnect() = 0;
	virtual void onDisconnect() = 0;
	virtual void onStatusChange(uint32_t connectStatus) = 0;
};

}} // namespace lightmq { namespace qmapi {

#endif /* ICONNECTIONLISTENER_H_ */
