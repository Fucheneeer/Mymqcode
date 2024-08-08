////////////////////////////////////////////////////////////////////////////////
//
// IDestination.h
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

#ifndef IDESTINATION_H_
#define IDESTINATION_H_

#include <memory>

namespace lightmq { namespace qmapi {

class IDestination {
public:
	enum Type {
        TOPIC,
        QUEUE
    };
	enum Persistency {
	    UnKnowType,
	    PERSISTENT,
		NON_PERSISTENT
    };

public:
    virtual ~IDestination() = default;

    /**
     * Retrieve the Destination Type for this Destination
     *
     * @return The Destination Type
     */
    virtual Type getType() const = 0;

    virtual std::string getName() const = 0;
};

inline std::string to_string(IDestination::Type t) {
	return t == IDestination::Type::QUEUE ? "QUEUE"
	     : t == IDestination::Type::TOPIC ? "TOPIC"
	     : "UnknowedDestType";
}

inline std::string to_string(IDestination::Persistency p) {
	return p == IDestination::Persistency::PERSISTENT ? "PERSISTENT"
	     : p == IDestination::Persistency::NON_PERSISTENT ? "NON_PERSISTENT"
		 : "UnknowedPersistency";
}


}} // namespace lightmq { namespace qmapi {

#endif /* IDESTINATION_H_ */
