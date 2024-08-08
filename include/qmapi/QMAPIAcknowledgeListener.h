////////////////////////////////////////////////////////////////////////////////
//
// QMAPIAcknowledgeListener.h
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

#ifndef QMAPIACKNOWLEDGELISTENER_H_
#define QMAPIACKNOWLEDGELISTENER_H_

#include "IAcknowledgeListener.h"

namespace lightmq { namespace qmapi {

class QMAPIAcknowledgeListener : public IAcknowledgeListener {
public:
	virtual ~QMAPIAcknowledgeListener() override;

    /**
     * Called asynchronously when an new acknowledge is received
     *
     * @param messageId
     *      Acknowledge of a message (const} reference recipient does not own.
     */
    void onAcknowledge(const MessageID& messageId) override;
};

}} // namespace lightmq { namespace qmapi {

#endif /* QMAPIACKNOWLEDGELISTENER_H_ */
