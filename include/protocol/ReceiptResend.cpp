////////////////////////////////////////////////////////////////////////////////
//
// QueryLastMessage.cpp
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
#include "../inc/ReceiptResend.h"
#include "BrokerProtocolUtil.h"
#include "ByteBuffer.h"
#include "UnpackageUnorderedParameters.h"
#include "PackageParameters.h"

// namespace lightmq {
// 	namespace protocol {
// 		void ReceiptResend::parse(dep::ByteBuffer& buffer) {
// 			UNPACKAGE_PARAMETERS_WITHID(buffer, RECEIPTRESENDMESSAGE_PARAM_LIST);
// 			return;
// 		}
// 
// 		void ReceiptResend::writeTo(dep::ByteBuffer& buffer) {
// 			util::PackageParametersWithId(buffer, RECEIPTRESENDMESSAGE_PARAM_LIST);
// 			return;
// 		}
// 
// 
// 	}
// } // namespace lightmq { namespace protocol {


