////////////////////////////////////////////////////////////////////////////////
//
// BrokerTypes.h
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

#ifndef BROKERTYPES_H_
#define BROKERTYPES_H_

#include <unordered_map>
#include "DEPException.h"
#include "hash_combine.h"
#include "MessageID.h"

namespace lightmq { 


typedef uint16_t ResultCode;

enum class PersistencyType {
    Persistent,
    InMemory,
	UnKnowType
};

enum class AddressType {
    QUEUE,
    TOPIC
};

enum class AddressType2 {
    TOPIC,
    QUEUE,
    EXCLUSIVE_QUEUE
};


#define MATCH_ADDRESSTYPE(at2, at, isExclusive)\
    ((at2 == AddressType2::TOPIC && at == AddressType::TOPIC)\
    || (at2 == AddressType2::QUEUE && at == AddressType::QUEUE && !isExclusive)\
    || (at2 == AddressType2::EXCLUSIVE_QUEUE && at == AddressType::QUEUE && isExclusive))

#define TO_ADDRESSTYPE2(at, isExclusive)\
(at==addressType::TOPIC ? AddressType2::TOPIC : (isExclusive ? AddressType2::EXCLUSIVE_QUEUE : AddressType2::QUEUE))

enum class AckMode {
    CLIENT_AUTO_ACK_MODE        = 0,
    CLIENT_CUMMULATIVE_ACK_MODE = 1,
    CLIENT_INDIVIDUAL_ACK_MODE  = 2,
	//CLIENT_TRANSACTION_ACK_MODE  = 3
};

enum class TransactionState: uint8_t {
	NOT_A_TRANS = 0,
	BROKER_NOT_RECEIPTED = 1,  //  only used in client for query broker's transaction state.
	START = 5,
	IN_PRE_COMMITING = 10,
	IN_COMMITING = 11,
	IN_ROLLBACK = 12,
	//SUSPENDED = 15,
	PRE_COMMITED = 17,
	PRE_COMMIT_FAILED = 18,
	COMMIT_FAILED = 21,
	ROLLBACK_FAILED = 22,

	//  the following transaction state is end.
	COMMITED = 30,
	ROLLBACKED = 31,
	DISCARD = 32,
	TIMEOUTED = 33,
};

inline bool isTransactionStateActive(TransactionState s) {
	return s < TransactionState::COMMITED;
}

inline std::string to_string(TransactionState transState) {
	switch (transState) {
		case TransactionState::NOT_A_TRANS: return "NOT_A_TRANS";
		case TransactionState::BROKER_NOT_RECEIPTED: return "BROKER_NOT_RECEIPTED";
		case TransactionState::START: return "START";
		case TransactionState::IN_PRE_COMMITING: return "IN_PRE_COMMITING";
		case TransactionState::IN_COMMITING: return "IN_COMMITING";
		case TransactionState::IN_ROLLBACK: return "IN_ROLLBACK";
		case TransactionState::PRE_COMMITED: return "PRE_COMMITED";
		//case TransactionState::SUSPENDED: return "SUSPENDED";
		case TransactionState::COMMITED: return "COMMITED";
		case TransactionState::ROLLBACKED: return "ROLLBACKED";
		case TransactionState::COMMIT_FAILED: return "COMMIT_FAILED";
		case TransactionState::ROLLBACK_FAILED: return "ROLLBACK_FAILED";
		case TransactionState::PRE_COMMIT_FAILED: return "PRE_COMMIT_FAILED";
		case TransactionState::DISCARD: return "DISCARD";
		case TransactionState::TIMEOUTED: return "TIMEOUTED";
		default: return std::string("UKNOWED-TRANS-STATE-") + std::to_string((uint8_t)transState);
	}
}

class AddressTypeUtil {
public:
    AddressTypeUtil()=delete;
    static const char* getAddressTypeString(AddressType type) {
        if (type==AddressType::QUEUE)
            return "QUEUE";
        else if (type==AddressType::TOPIC)
            return "TOPIC";
        else
            throw dep::DEPException("unknown AddressType:" + std::to_string((uint32_t)type) );
    }

    static std::string getPersistencyTypeString(PersistencyType type) {
        if (type==PersistencyType::Persistent)
            return "PERSISTENT";
        else if (type==PersistencyType::InMemory)
            return "INMEMORY";
        else if (type==PersistencyType::UnKnowType)
            return "UNKNOWTYPE";
        else
            throw dep::DEPException("unknown PersistencyType:" + std::to_string((uint32_t)type) );
    }
};




struct ProviderIDInfo {
    ProviderIDInfo() {

    }
    ProviderIDInfo(const std::string& ID, const MessageID& messageID) :
        providerID(ID),
        providerMessageID(messageID) {

    }

    std::string providerID;
    MessageID providerMessageID;
};

struct DestinationMessageID {
	DestinationMessageID() {}
	DestinationMessageID(const std::string &destID, const MessageID &msgID)
		: destinationID(destID)
		, messageID(msgID)
	{}

	std::string destinationID;
	MessageID messageID;

	inline bool operator==(const DestinationMessageID &rv) const {
		return messageID == rv.messageID
			&& destinationID == rv.destinationID;
	}
};

inline std::string to_string(const DestinationMessageID &destID) {
	return destID.messageID.toString() + "." + destID.destinationID;
}

typedef std::unordered_map<MessageID, ProviderIDInfo> MessageIDProviderInfoMap;

struct DestinationProviderID {
	std::string m_destination;
	std::string m_providerID;
	size_t m_hashValue;

	DestinationProviderID()
		: m_hashValue(0)
	{}

	DestinationProviderID(const std::string &destination, const std::string &providerID)
		: m_destination(destination)
		, m_providerID(providerID)
	{
		updateHashValue();
	}

	DestinationProviderID(std::string &&destination, std::string &&providerID)
		: m_destination(std::move(destination))
		, m_providerID(std::move(providerID))
	{
		updateHashValue();
	}

	bool operator==(const DestinationProviderID& rv) const
	{
		return m_hashValue == rv.m_hashValue  //  if they are not same, we will known very quickly
			&& m_destination == rv.m_destination
			&& m_providerID == rv.m_providerID;
	}

	inline void updateHashValue()
	{
		m_hashValue = std::hash<decltype(m_destination)>()(m_destination);
		hash_combine(m_hashValue, m_providerID);
	}
};

struct ProviderMessageID {
	DestinationProviderID m_destinationProviderID;
	MessageID m_msgID;
	size_t m_hashValue;

	ProviderMessageID()
		: m_hashValue(0)
	{}

	ProviderMessageID(const DestinationProviderID &destinationProviderID, const MessageID &msgID)
		: m_destinationProviderID(destinationProviderID)
		, m_msgID(msgID)
	{
		updateHashValue();
	}

	ProviderMessageID(DestinationProviderID &&destinationProviderID, MessageID &&msgID)
		: m_destinationProviderID(std::move(destinationProviderID))
		, m_msgID(std::move(msgID))
	{
		updateHashValue();
	}

	ProviderMessageID(const std::string &destination, const std::string &ProviderID, const MessageID &msgID)
		: m_destinationProviderID(destination, ProviderID)
		, m_msgID(msgID)
	{
		updateHashValue();
	}

	ProviderMessageID(std::string &&destination, std::string &&ProviderID, const MessageID &msgID)
		: m_destinationProviderID(destination, ProviderID)
		, m_msgID(msgID)
	{
		updateHashValue();
	}

	bool operator==(const ProviderMessageID& rv) const
	{
		return m_hashValue == rv.m_hashValue  //  if they are not same, we will known very quickly
			&& m_msgID == rv.m_msgID
			&& m_destinationProviderID == rv.m_destinationProviderID;
	}

	inline void updateHashValue();
};

typedef MessageID TransactionInnerID;

struct TransactionID {
	std::string m_transSessionID;
	uint64_t m_transID;
	size_t m_hashValue;

	TransactionID()
		: m_transID(0)
		, m_hashValue(0)
	{}

	void clearTransactionID() {
		m_transSessionID.clear();
		m_transID = 0;
		m_hashValue = 0;
	}

	inline operator bool() const {
		return m_transID || !m_transSessionID.empty();
	}

	TransactionID(const std::string &transSession, uint64_t transID)
		: m_transSessionID(transSession)
		, m_transID(transID)
	{
		updateHashValue();
	}

	TransactionID(std::string &&transSession, uint64_t transID)
		: m_transSessionID(transSession)
		, m_transID(transID)
	{
		updateHashValue();
	}

	void setTransactionID(const std::string &transSession, uint64_t transID)
	{
		m_transSessionID = transSession;
		m_transID = transID;
		updateHashValue();
	}

	void setTransactionID(std::string &&transSession, uint64_t transID)
	{
		m_transSessionID = std::move(transSession);
		m_transID = transID;
		updateHashValue();
	}

	bool operator==(const TransactionID& rv) const
	{
		return m_hashValue == rv.m_hashValue  //  if they are not same, we will known very quickly
			&& m_transID == rv.m_transID
			&& m_transSessionID == rv.m_transSessionID;
	}

	bool operator!=(const TransactionID& rv) const
	{
		return m_hashValue != rv.m_hashValue  //  if they are not same, we will known very quickly
		       || m_transID != rv.m_transID
		       || m_transSessionID != rv.m_transSessionID;
	}

	bool operator<(const TransactionID& rv) const
	{
		return m_transID < rv.m_transID
			|| (m_transID == rv.m_transID && m_transSessionID < rv.m_transSessionID);
	}

	inline void updateHashValue()
	{
		m_hashValue = std::hash<decltype(m_transSessionID)>()(m_transSessionID);
		hash_combine(m_hashValue, m_transID);
	}
};

inline std::string to_string(const TransactionID &transID) {
	return std::to_string(transID.m_transID) + "." + transID.m_transSessionID;
}

typedef uint32_t TransactionMessageSeq;

struct TransactionMessageID {
	TransactionID m_transID;
	TransactionMessageSeq m_messageSeq;

	TransactionMessageID(TransactionID &&transID, TransactionMessageSeq msgSeq)
		: m_transID(std::move(transID))
		, m_messageSeq(msgSeq)
	{}

	TransactionMessageID(const TransactionID &transID, TransactionMessageSeq msgSeq)
			: m_transID(transID)
			, m_messageSeq(msgSeq)
	{}

	void setTransactionMessageID(TransactionID &&transID, TransactionMessageSeq msgSeq)
	{
		m_transID = std::move(transID);
		m_messageSeq = msgSeq;
	}

	void setTransactionMessageID(const TransactionID &transID, TransactionMessageSeq msgSeq)
	{
		m_transID = transID;
		m_messageSeq = msgSeq;
	}


	bool operator==(const TransactionMessageID& rv) const {
		return m_messageSeq == rv.m_messageSeq
			&& m_transID == rv.m_transID;
	}
};

inline std::string to_string(const TransactionMessageID &transMsgID) {
	return std::to_string(transMsgID.m_messageSeq) + "." + to_string(transMsgID.m_transID);
}

} // namespace lightmq {


namespace std
{
    template <>
    struct hash<lightmq::MessageID>
    {
        size_t operator()(const lightmq::MessageID& k) const
        {
			//size_t hashValue = k.sequence;
			//hash_combine(hashValue, sessionID);
			//return hashValue;

            // use sequence for hash key only
            return ((hash<uint64_t>()(k.sequence)));
        }
    };

	template<>
	struct hash<lightmq::DestinationProviderID>
	{
		inline size_t operator()(const lightmq::DestinationProviderID& rv) const
		{
			return rv.m_hashValue;
		}
	};

	template<>
	struct hash<lightmq::ProviderMessageID>
	{
		inline size_t operator()(const lightmq::ProviderMessageID& rv) const
		{
			return rv.m_hashValue;
		}
	};

	template<>
	struct hash<lightmq::TransactionID>
	{
		inline size_t operator()(const lightmq::TransactionID& rv) const
		{
			return rv.m_hashValue;
		}
	};

	template<>
	struct hash<lightmq::TransactionMessageID>
	{
		inline size_t operator()(const lightmq::TransactionMessageID& rv) const
		{
			size_t hashValue =  hash<lightmq::TransactionID>()(rv.m_transID);
			hash_combine(hashValue, rv.m_messageSeq);
			return hashValue;
		}
	};

	template<>
	struct hash<lightmq::DestinationMessageID>
	{
		inline size_t operator()(const lightmq::DestinationMessageID& rv) const
		{
			size_t hashValue = std::hash<lightmq::MessageID>()(rv.messageID);
			hash_combine(hashValue, rv.destinationID);
			return hashValue;
		}
	};
}


namespace lightmq { 

inline void ProviderMessageID::updateHashValue()
{
	if (m_destinationProviderID.m_hashValue == 0)
		m_destinationProviderID.updateHashValue();
	m_hashValue = std::hash<decltype(m_destinationProviderID)>()(m_destinationProviderID);
	hash_combine(m_hashValue, m_msgID);
}

} // namespace lightmq {

#endif /* BROKERTYPES_H_ */
