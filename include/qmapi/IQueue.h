////////////////////////////////////////////////////////////////////////////////
//
// IQueue.h
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

#ifndef IQUEUE_H_
#define IQUEUE_H_

#include <string>
#include "IDestination.h"

namespace lightmq { namespace qmapi {

class IQueue : public IDestination {
public:
	virtual ~IQueue() = default;

    /**
     * Gets the name of this queue.
     *
     * @return The queue name.
     *
     */
    virtual std::string getName() const = 0;


};

}} // namespace lightmq { namespace qmapi {

#endif /* IQUEUE_H_ */
