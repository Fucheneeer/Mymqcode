////////////////////////////////////////////////////////////////////////////////
//
// IAckReceivedMsgSender.h
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

#ifndef IACKRECEIVEDMSGSENDER_H_
#define IACKRECEIVEDMSGSENDER_H_

#include "BrokerMessage.h"

namespace lightmq { namespace qmapi {

class IAckReceivedMsgSender {
public:
	virtual ~IAckReceivedMsgSender() = default;

    /**
     * Acknowledge received message
     *
     * @param msg
     *      message to acknowledge
     *
     */
    virtual bool ackRecvMessage(const BrokerMessage& msg) = 0;

};

}} // namespace lightmq { namespace qmapi {

#endif /* IACKRECEIVEDMSGSENDER_H_ */
