////////////////////////////////////////////////////////////////////////////////
//
// DestinationAtrributes.h
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

#ifndef DESTINATION_ATTR_H_
#define DESTINATION_ATTR_H_

#include <cstdint>
#include <string>
#include <memory>

#include "BrokerProtocolConstant.h"
#include "IMessage.h"
#include "BrokerTypes.h"
#include <unordered_map>

namespace lightmq { namespace protocol {

// class dep::ByteBuffer;
class DestinationAtrributes : public protocol::IMessage {
public:

	DestinationAtrributes() = default;
	virtual ~DestinationAtrributes() override = default;

	void parse(dep::ByteBuffer& buffer) override;

	void writeTo(dep::ByteBuffer& buffer) override;	

	uint8_t getMessageType() override { return protocol::BrokerProtocolConstant::MESSAGE_TYPE_DESTINATION_ATTRIBUTES; };

	void setPersistency(const std::unordered_map<std::string, PersistencyType>& destinationPersistencyType) {
		m_destinationPersistencyType = destinationPersistencyType;
	}
	const std::unordered_map<std::string, PersistencyType>& getPersistency() const {
		return m_destinationPersistencyType;
	}

protected:
	std::unordered_map<std::string, PersistencyType> m_destinationPersistencyType;
};

}} // namespace lightmq { namespace protocol {

#endif /* DESTINATION_ATTR_H_ */
