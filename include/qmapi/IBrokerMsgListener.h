////////////////////////////////////////////////////////////////////////////////
//
// IBrokerMsgListener.h
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

#ifndef IBROKERMSGLISTENER_H_
#define IBROKERMSGLISTENER_H_

#include "BrokerMessage.h"

namespace lightmq { namespace qmapi {

class IBrokerMsgListener {
public:
    virtual ~IBrokerMsgListener() = default;

	virtual void onBrokerMessage(BrokerMessage& msg) = 0;
	virtual void onBrokerMessage2(std::unique_ptr<protocol::IMessage> &&msg) = 0;

	virtual void setBrokerTimeDiff(uint64_t timeDiff) = 0;
};

}} // namespace lightmq { namespace qmapi {


#endif /* IBROKERMSGLISTENER_H_ */
