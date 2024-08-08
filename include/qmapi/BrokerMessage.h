////////////////////////////////////////////////////////////////////////////////
//
// BrokerMessage.h
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
/*
 * BrokerMessage.h
 *
 *  Created on: Feb 15, 2019
 *      Author: cheung_w
 */

#ifndef BROKERMESSAGE_H_
#define BROKERMESSAGE_H_

#include <vector>
#include <memory>
#include "BrokerTypes.h"
#include "IMessageListener.h"


namespace lightmq { namespace qmapi {

class QMAPIMessageConsumer;

struct ProviderInfo {
	std::string providerClientId;
	MessageID providerMessageId;
};


typedef struct BROKERMESSAGE {
	uint8_t msgType;
	// CONNECT with a clientId, which is sent back to client in CONNECTED
	std::string clientId;					// CONNECTED

	// (create)QUEUE, (create)TOPIC and SUBSCRIBE are sent with a receptId, which is sent back to client in RECEIPT
	uint16_t receiptId;						// RECEIPT / RECEIPTLASTMESSAGE
    ResultCode resultCode;					// RECEIPT / ACKSEND / RECEIPTLASTMESSAGE

    // Send ACK with sessionId and sequence after getting a MESSAGE
    std::string destination;				// MESSAGE / ACKSEND
    SessionID sessionId;					// MESSAGE / ACKSEND / RECEIPTLASTMESSAGE
    uint64_t sequence;						// MESSAGE / ACKSEND / RECEIPTLASTMESSAGE
    uint64_t endSequence;					// MESSAGE / ACKSEND
	std::vector<dep::ByteBuffer> messages;		// MESSAGE / RECEIPTLASTMESSAGE
	std::vector<ProviderInfo> providerInfo;	// MESSAGE
	uint16_t subscriptionId;				// MESSAGE
	bool isRetransmitted;					// MESSAGE
	IMessageListener* pListener;			// Listener to process a BrokerMessage
	std::vector<bool> filteredMessages;     // true is filtered (not going to send to upper layer), should be same length as messages
	bool needSendAck;                       // need to send ack to broker(for consumer use)
	std::weak_ptr<QMAPIMessageConsumer> pMessageConsumer;     // use it to send ack to broker
    std::string userID;

	BROKERMESSAGE() {
//		printf("BROKERMESSAGE constructor address[%p]\n\n", this);
		msgType = 0;
		clientId = "";
		receiptId = 0;
		resultCode = 0;
		destination = "";
		sessionId = 0;
		sequence = 0;
		endSequence = 0;
		messages.clear();
		providerInfo.clear();
		subscriptionId = 0;
		isRetransmitted = false;
		pListener = nullptr;
		needSendAck = false;
		//pMessageConsumer =nullptr;
        userID = "";
	}

	BROKERMESSAGE& operator=(BROKERMESSAGE&& o) {
        msgType = o.msgType;
        clientId = std::move(o.clientId);
        receiptId = o.receiptId;
        resultCode = o.resultCode;
        destination = std::move(o.destination);
        sessionId = o.sessionId;
        sequence = o.sequence;
        endSequence = o.endSequence;
        messages = std::move(o.messages);
        providerInfo = std::move(o.providerInfo);
        subscriptionId = o.subscriptionId;
        isRetransmitted = o.isRetransmitted;
        pListener = o.pListener;
        filteredMessages = std::move(o.filteredMessages);
        needSendAck = o.needSendAck;
        pMessageConsumer = o.pMessageConsumer;
        userID = o.userID;
        return *this;
	}


    BROKERMESSAGE& operator=(const BROKERMESSAGE& o) {
    	msgType = o.msgType;
    	clientId = o.clientId;
    	receiptId = o.receiptId;
    	resultCode = o.resultCode;
    	destination = o.destination;
    	sessionId = o.sessionId;
    	sequence = o.sequence;
    	endSequence = o.endSequence;
    	messages.clear();
		for (const dep::ByteBuffer& message : o.messages) {
//		    printf("BROKERMESSAGE operator= address[%p] cp[%p] [%s] seq[%lu] endSeq[%lu]\n\n\n",
//		    		this,
//					&o,
//		    		std::string((const char*)message.getArray()+message.getPosition(), message.remaining()).data(),
//					sequence, endSequence);
		    messages.push_back(dep::ByteBuffer());
		    messages.back().put(message.getArray()+message.getPosition(), message.remaining());
		    messages.back().flip();
		}
		providerInfo = o.providerInfo;
		subscriptionId = o.subscriptionId;
		isRetransmitted = o.isRetransmitted;
		pListener = o.pListener;
		filteredMessages = o.filteredMessages;
		needSendAck = o.needSendAck;
		pMessageConsumer = o.pMessageConsumer;
        userID = o.userID;
		return *this;
    }


    BROKERMESSAGE(BROKERMESSAGE &&o) :
        msgType(o.msgType),
        clientId (std::move(o.clientId)),
        receiptId (o.receiptId),
        resultCode (o.resultCode),
        destination (std::move(o.destination)),
        sessionId (o.sessionId),
        sequence (o.sequence),
        endSequence (o.endSequence),
        messages (std::move(o.messages)),
        providerInfo (std::move(o.providerInfo)),
        subscriptionId (o.subscriptionId),
        isRetransmitted (o.isRetransmitted),
        pListener (o.pListener),
        filteredMessages (std::move(o.filteredMessages)),
        needSendAck (o.needSendAck),
        pMessageConsumer(o.pMessageConsumer),
        userID(std::move(o.userID))
    {

    }

	BROKERMESSAGE(const BROKERMESSAGE& o) {
		msgType = o.msgType;
		clientId = o.clientId;
		receiptId = o.receiptId;
		resultCode = o.resultCode;
		destination = o.destination;
		sessionId = o.sessionId;
		sequence = o.sequence;
		endSequence = o.endSequence;
		messages.clear();
		for (const dep::ByteBuffer& message : o.messages) {
//		    printf("BROKERMESSAGE copy constructor address[%p] [%s] sequence[%lu] endSequence[%lu]\n\n\n",
//		    	this,
//		    	std::string((const char*)message.getArray()+message.getPosition(), message.remaining()).data(),
//				sequence, endSequence);
		    messages.push_back(dep::ByteBuffer());
		    messages.back().put(message.getArray()+message.getPosition(), message.remaining());
		    messages.back().flip();
		}
		providerInfo = o.providerInfo;
		subscriptionId = o.subscriptionId;
		isRetransmitted = o.isRetransmitted;
		pListener = o.pListener;
        filteredMessages = o.filteredMessages;
        needSendAck = o.needSendAck;
        pMessageConsumer = o.pMessageConsumer;
        userID = o.userID;
   }
} BrokerMessage;

}} // namespace lightmq { namespace qmapi {

#endif /* BROKERMESSAGE_H_ */
