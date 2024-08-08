////////////////////////////////////////////////////////////////////////////////
//
// ITopic.h
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

#ifndef ITOPIC_H_
#define ITOPIC_H_

#include <string>
#include "IDestination.h"

namespace lightmq { namespace qmapi {

class ITopic : public IDestination {
public:
	virtual ~ITopic() = default;

    /**
     * Gets the name of this topic.
     *
     * @return The topic name.
     *
     */
    virtual std::string getName() const = 0;
};

}} // namespace lightmq { namespace qmapi {

#endif /* ITOPIC_H_ */
