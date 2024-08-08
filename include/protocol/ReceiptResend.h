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

#ifndef RECEIPTRESEND_H_
#define RECEIPTRESEND_H_
#include <string>
#include <cstdint>
#include <unistd.h>
#include <iostream>
#include "BrokerProtocolConstant.h"
#include "IMessage.h"
#include <vector>
#include "BrokerTypes.h"

using namespace std;

#define RECEIPTRESENDMESSAGE_PARAM_LIST\
             1, m_result,\
             2, m_receiptID,\
			 3, m_resultCode

namespace lightmq {	namespace protocol {
		class ReceiptResend : public protocol::IMessage {
		public:
			ReceiptResend();
			virtual ~ReceiptResend();

			virtual void parse(dep::ByteBuffer& buffer) override;

			virtual void writeTo(dep::ByteBuffer& buffer) override;

			void setReceiptID(uint16_t receiptID) { m_receiptID = receiptID; };
			uint16_t getReceiptID() const { return m_receiptID; };

			void setResultCode(ResultCode code) { m_resultCode = code; }
			ResultCode getResultCode() const { return m_resultCode; };

			void setResult(const std::string& result) { m_result = result; }
			const std::string& getResult() const { return m_result; }
			
			uint8_t getMessageType() override { return protocol::BrokerProtocolConstant::MESSAGE_TYPE_RECEIPT_RESEND; };

		private:
			std::string m_result;
			uint16_t m_receiptID{ 0 };
			ResultCode m_resultCode;

		};
}}
#endif /* RESENDCONSUMEDMESSAGE_H_ */
