////////////////////////////////////////////////////////////////////////////////
//
// IAcknowledgeListener.h
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

#ifndef IACKNOWLEGELISTENER_H_
#define IACKNOWLEGELISTENER_H_

#include "BrokerTypes.h"

namespace lightmq { namespace qmapi {

class IAcknowledgeListener {
public:
	virtual ~IAcknowledgeListener() = default;

    /**
     * Called asynchronously when an new acknowledge is received
     * @param destination address of destination
     * @param messageId
     *      Acknowledge of a message (const} reference recipient does not own.
     * @param endMessageId end message id of acked messages
     */
    virtual void onAcknowledge(const std::string& destination, const MessageID& messageId, const MessageID& endMessageId) = 0;
};

}} // namespace lightmq { namespace qmapi {

#endif /* IACKNOWLEGELISTENER_H_ */
