////////////////////////////////////////////////////////////////////////////////
//
// QMAPIQueue.h
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

#ifndef QMAPIQUEUE_H_
#define QMAPIQUEUE_H_

#include "IQueue.h"

namespace lightmq { namespace qmapi {

class QMAPIQueue : public IQueue {
public:
	virtual ~QMAPIQueue() override;
    //QMAPIQueue() = delete;
    QMAPIQueue(const std::string &name): m_name(name) {}
	QMAPIQueue(std::string &&name): m_name(std::move(name)) {}

    /**
     * Gets the name of this queue.
     *
     * @return The queue name.
     *
     */
    std::string getName() const override;
    IDestination::Type getType() const override;

private:
    std::string m_name;
};


}} // namespace lightmq { namespace qmapi {



#endif /* QMAPIQUEUE_H_ */
