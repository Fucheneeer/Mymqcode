////////////////////////////////////////////////////////////////////////////////
//
// MessageBlocksDequeImpl.h
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
#ifndef MessageBlocksDeque_H_
#define MessageBlocksDeque_H_

#include <deque>
#include <memory>
#include "MessageBlocks.h"

// namespace lightmq { namespace util {
// 
// 
// class MessageBlocksDequeImpl : public MessageBlocks
// {
//     struct MessageBlocksInfo {
//         MessageBlocksInfo(const MessageID messageId_, uint64_t endSeq_, bool isRetransmit_, bool complexMessage_)
//              :messageId(messageId_)
//              , endSeq(endSeq_)
//              , isRetransmit(isRetransmit_)
//              , complexMessage(complexMessage_)
//         {}
//         MessageID messageId;
//         uint64_t endSeq{ 0 };
//         bool isRetransmit{ false };
//         bool complexMessage {false};
//     };
// 
//     class Iterator : public MessageBlocks::IIterator {
//     public:
//         Iterator(std::deque<MessageBlocksInfo>::reverse_iterator rend, std::deque<MessageBlocksInfo>::reverse_iterator rbegin);
//         virtual void get(MessageID& message, uint64_t& endSeq, bool& isRetransmit, bool &isComplex) override;
//         virtual void get(MessageID& message, uint64_t& endSeq, bool& isRetransmit) override;
//         virtual bool next() override;
//     private:
//         std::deque<MessageBlocksInfo>::reverse_iterator m_rend;
//         std::deque<MessageBlocksInfo>::reverse_iterator m_rbegin;
//         std::deque<MessageBlocksInfo>::reverse_iterator m_current;
//     };
// 
// public:
// 
//     MessageBlocksDequeImpl(bool isLock = false);
// 
//     virtual void pushBack(MessageID messageID, uint64_t endSeq = 0, bool isRetransmit = false, bool complexMessage = false) override;
//     // virtual void pushBack(MessageID messageID, uint64_t endSeq , bool isRetransmit, bool complexMessage ) override;
// 
//     virtual bool getFirst(MessageID& messageID, uint64_t& endSeq, bool& isRetransmit) override;
// 
//     virtual bool getFirst(uint64_t size, MessageID& messageID, uint64_t& endSeq, bool& isRetransmit, bool &isGroupMessage) override;
// 
//     virtual bool isEmpty() const override;
// 
//     virtual uint64_t split(uint64_t count, MessageBlocks& blockMessage, GroupMessageInfo *groupMessage = nullptr) override;
// 
//     virtual void removeExpired(ISessionScheduleUtility* pSessionScheduleUtility) override;
// 
//     virtual uint64_t getMessageCount() const override{
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
//     std::deque<MessageBlocksInfo> m_messageDeque;
//     uint64_t m_totalCount{ 0 };
// };
// 
// }} // namespace lightmq { namespace util {

#endif//MessageBlocksDeque_H_
