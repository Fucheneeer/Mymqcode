////////////////////////////////////////////////////////////////////////////////
//
// IMessageListener.h
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

#ifndef IMESSAGELISTENER_H_
#define IMESSAGELISTENER_H_

#include <memory>
#include <vector>
#include "IMessage.h"
#include "MessageID.h"
#include "BrokerTypes.h"
#include "LightmqMessage.h"

namespace lightmq { namespace qmapi {

class IMessageListener {
public:
	virtual ~IMessageListener() = default;

    /**
     * Called asynchronously when a new message is received, the message
     * reference can be to any of the Message types. a dynamic cast is used
     * to find out what type of message this is.  The lifetime of this
     * object is only guaranteed to be for life of the onMessage function
     * after this call-back returns the message may no longer exists.  Users should
     * copy the data or clone the message if they wish to retain information that
     * was contained in this Message.
     *
     * It is considered a programming error for this method to throw an
     * exception.  The method has been tagged with the 'throw()' qualifier,
     * this implies that you application will segfault if you throw an error
     * from an implementation of this method.
     *
     * @param message
     *      Message object {const} pointer recipient does not own.
     */
	virtual void onMessage(std::string& destination, MessageID msgId, LightmqMessage& message) = 0;
};

}} // namespace lightmq { namespace qmapi {

#endif /* IMESSAGELISTENER_H_ */
