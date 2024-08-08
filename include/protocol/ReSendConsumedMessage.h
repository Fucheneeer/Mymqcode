////////////////////////////////////////////////////////////////////////////////
//
// QueryLastMessage.h
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

#ifndef RESENDCONSUMEDMESSAGE_H_
#define RESENDCONSUMEDMESSAGE_H_
#include <string>
#include <cstdint>
#include <unistd.h>
#include <iostream>
#include "BrokerProtocolConstant.h"
#include "IMessage.h"
#include <vector>
using namespace std;

#define RESENDCONSUMEDMESSAGE_PARAM_LIST\
             1, m_destination,\
             2, m_messageIDs,\
			 3, m_subscriptionName,\
			 4, m_consumerClientIDs,\
			 5, m_receiptID

namespace lightmq {	namespace protocol {
		class ReSendConsumedMessage : public protocol::IMessage {
		public:
			ReSendConsumedMessage() = default;
			virtual ~ReSendConsumedMessage() = default;

			virtual void parse(dep::ByteBuffer& buffer) override;

			virtual void writeTo(dep::ByteBuffer& buffer) override;

			void setMessageIDs(const std::vector<std::string>& messageIDs) { m_messageIDs=messageIDs; };
			std::vector <std::string> getMessageIDs() const { return m_messageIDs; };

			void setConsumerClientIDs(const std::vector<std::string>& subTopicClients) { m_consumerClientIDs =subTopicClients; };
			std::vector < std::string> getConsumerClientIDs() const { return m_consumerClientIDs; };

			void setReceiptID(uint16_t receiptID) { m_receiptID = receiptID; };
			uint16_t getReceiptID() const { return m_receiptID; };

			void setDestination(const std::string& destination) { m_destination = destination; }
			const std::string& getDestination() const { return m_destination; }

			void setSubscriptionName(const std::string& subscriptionName) { m_subscriptionName = subscriptionName; }
			const std::string& getSubscriptionName() const { return m_subscriptionName; }
			
			uint8_t getMessageType() override { return protocol::BrokerProtocolConstant::MESSAGE_TYPE_RESEND_MESSAGE; };

		private:
			std::vector<std::string> m_messageIDs;
			std::vector<std::string> m_consumerClientIDs;
			std::string m_subscriptionName;
			std::string m_destination;
			uint16_t m_receiptID{ 0 };

		};
}}
#endif /* RESENDCONSUMEDMESSAGE_H_ */
