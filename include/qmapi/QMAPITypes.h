////////////////////////////////////////////////////////////////////////////////
//
// QMAPITypes.h
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


#ifndef QMAPITYPES_H_
#define QMAPITYPES_H_

#include <StringUtil.h>
#include <memory>
#include <vector>
//#include "QMAPIClientSocket.h"
#include "QMAPIQueue.h"
#include "QMAPITopic.h"
#include "QMAPIConstant.h"

namespace lightmq { namespace qmapi {

typedef struct BROKERIP {
	std::string ip;
	uint16_t port;

	static void get(std::string brokerIP, std::string& ip, uint16_t& port) {
		std::vector<std::string> tokens;
		StringUtil::split(brokerIP, ":", tokens);
		ip = tokens[0];
		port = static_cast<uint16_t>(StringUtil::stou(tokens[1]));
	}
	bool operator==(const BROKERIP& broker) {
		if (broker.ip == this->ip && broker.port == this->port)
			return true;
		else
			return false;
	}
} BrokerIp;

//struct SOCKET2IP {
//	std::unique_ptr<QMAPIClientSocket> pSocket;
//	BROKERIP brokerIp;
//};

}} // namespace lightmq { namespace qmapi {

#endif /* QMAPITYPES_H_ */
