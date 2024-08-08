////////////////////////////////////////////////////////////////////////////////
//
// MessageBlocks.h
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
#ifndef MessageBlocks_H_
#define MessageBlocks_H_

#include <map>
#include <memory>
#include "BrokerTypes.h"
#include "ISessionScheduleUtility.h"
//#include "SessionStoreManager.h"
#include "ILock.h"
#include <functional>

// namespace lightmq { namespace util {
// 
// class MessageBlocks
// {
// public:
//     struct GroupMessageInfo
//     {
//         MessageID groupEndMessageID; 
//         bool isRetransmit;
//     };
// 
// 	class IIterator {
// 	public:
// 		IIterator() = default;
// 
// 		virtual ~IIterator() = default;
// 
// 		virtual void get(MessageID& message, uint64_t& endSeq, bool& isRetransmit, bool &isComplex) = 0;
// 		virtual void get(MessageID& message, uint64_t& endSeq, bool& isRetransmit) = 0;
// 		virtual bool next() = 0;
// 	};
// public:
// 	MessageBlocks(bool isLock = false);
// 
//     // virtual void pushBack(MessageID messageID, uint64_t endSeq = 0, bool isRetransmit = false) = 0;
//     virtual void pushBack(MessageID messageID, uint64_t endSeq = 0, bool isRetransmit = 0, bool complexMessage = false) = 0;
// 
// 	virtual bool getFirst(MessageID& messageID, uint64_t& endSeq, bool& isRetransmit) = 0;
// 
// 	virtual bool getFirst(uint64_t size, MessageID& messageID, uint64_t& endSeq, bool& isRetransmit, bool &isGroupMessage) = 0;
// 
//     virtual bool isEmpty() const = 0;
// 
//     virtual uint64_t split(uint64_t count, MessageBlocks& blockMessage, GroupMessageInfo *groupMessage = nullptr) = 0;
// 
//     virtual void removeExpired(ISessionScheduleUtility* pSessionScheduleUtility) = 0;
// 
// 	virtual uint64_t getMessageCount() const = 0;
// 
//     virtual void clear() = 0;
// 
// 	void lock();
// 
// 	void unlock();
// 
// 	bool try_lock();
// 
//     virtual void dump(const std::string& message) const = 0;
// 
// 	virtual std::unique_ptr<IIterator> getIterator() = 0;
// 
// protected:
//     std::unique_ptr<ILock> m_pLock;
// 
// };
// 
// }} // namespace lightmq { namespace util {

#endif//MessageBlocks_H_
