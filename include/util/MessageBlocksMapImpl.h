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
#ifndef MessageBlocksMap_H_
#define MessageBlocksMap_H_

#include <map>
#include <memory>
#include "MessageBlocks.h"

// namespace lightmq { namespace util {
// 
// class MessageBlocksMapImpl : public MessageBlocks
// {
//     struct MessageBlocksInfo {
//         uint64_t endSeq{ 0 };
//         bool isRetransmit{ false };
//         bool complexMessage {false};
//     };
//     class Iterator : public MessageBlocks::IIterator {
//     public:
//         Iterator(std::map< MessageID, MessageBlocksInfo>::reverse_iterator rend, std::map< MessageID, MessageBlocksInfo>::reverse_iterator rbegin);
//         virtual void get(MessageID& message, uint64_t& endSeq, bool& isRetransmit, bool &isComplex) override;
//         virtual void get(MessageID& message, uint64_t& endSeq, bool& isRetransmit) override;
//         virtual bool next() override;
//     private:
//         std::map< MessageID, MessageBlocksInfo>::reverse_iterator m_rend;
//         std::map< MessageID, MessageBlocksInfo>::reverse_iterator m_rbegin;
//         std::map< MessageID, MessageBlocksInfo>::reverse_iterator m_current;
//     };
// public:
//     MessageBlocksMapImpl(bool isLock = false);
// 
//     void insert(MessageID messageID, uint64_t endSeq, bool isRetransmit, bool complexMessage);
//     // void insert(MessageID messageID, uint64_t endSeq, bool isRetransmit);
// 
//     virtual void pushBack(MessageID messageID, uint64_t endSeq = 0, bool isRetransmit = false, bool complexMessage = false) override;
//     // virtual void pushBack(MessageID messageID, uint64_t endSeq, bool isRetransmit, bool complexMessage) override;
// 
//     virtual bool getFirst(MessageID& messageID, uint64_t& endSeq, bool& isRetransmit) override;
// 
//     virtual bool getFirst(uint64_t size, MessageID& messageID, uint64_t& endSeq, bool& isRetransmit, bool &isGroupMessage) override;
// 
// 	uint64_t getNextMatched(std::function<bool(const uint64_t sessionID)> isSessionStoreExistFun, 
// 		std::function<bool(const uint64_t sessionID, const  uint64_t sequence)> isMatchFunc, MessageID& cursorMesssageID,
// 		uint64_t ackWindowsSize, uint64_t limitCount, MessageBlocks* pMatchedMessages, bool& hasMatchedToEnd);
// 
//     //bool remove(MessageBlocks& blockMessage);
// 
//     virtual bool isEmpty() const override;
//     //bool displayFirst(std::map<MessageID, MessageBlocksInfo>::iterator& messageIter, MessageID& message, uint64_t& endSeq, bool& isRetransmit);
// 
//     virtual uint64_t split(uint64_t count, MessageBlocks& messageBlocks, GroupMessageInfo *groupMessage = nullptr) override;
// 
//     virtual void removeExpired(ISessionScheduleUtility* pSessionScheduleUtility) override;
// 
//     virtual uint64_t getMessageCount() const override {
//         return m_totalCount;
//     }
// 
//     virtual void clear() override;
// 
//     virtual void dump(const std::string& message) const override;
// 
//     virtual std::unique_ptr<MessageBlocks::IIterator> getIterator() override;
// 
// private:
//     std::map<MessageID, MessageBlocksInfo> m_messageMap;
//     uint64_t m_totalCount{ 0 };
// };
// 
// }} // namespace lightmq { namespace util {

#endif//MessageBlocksMap_H_
