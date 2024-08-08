////////////////////////////////////////////////////////////////////////////////
//
// .h
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
#pragma once
#include <vector>
#include <map> 
#include <memory>
#include <tuple>

// #include "MessageBlocksMapImpl.h"
#include "MessageID.h"
#include "GroupMessageID.h"
#include "MessageBlocksNew.h"

// 启动时加载，根据session清除

// using lightmq::util::MessageBlocks ;
// using lightmq::util::MessageBlocksMapImpl ;

namespace lightmq {
namespace util {


// 组消息备注：
// Ack: 
//     每条组消息的ack都会恢复ackWindowSize，但是并不把ack写入文件。
//     组End消息的ack，触发ack文件写入SessionStore。
// PendingAckCount的处理：
//     类似于Ack的处理。
//     普通组消息不触发PendingAckCount变动。
//     组End消息的ack,一次性把整组的消息数目加到pendingAckCount中。等待备机反馈。
// 事务中中的consumr接收组消息时,rollback机制：
//     处理原则：只有groupEnd消息才能被rollback。未完整接收的消息不能被rollback.
// 事务中中的producer发送组消息时不受rollback限制
// 发送异常：
// 组消息必须要保证完整性，如果发送中组消息异常要按照以下两种方式处理
// 1/组消息尚未发送：此时，直接取消组消息的发送。
// 2/组消息已经部分发送：此时给相应的客户端发送一个组消息异常结束的message消息(未实现)。应用收到这条消息的时候，将之前的消息丢弃
    

class ISessionStoreManager;

class VariantMap;
class GroupMessage;
typedef std::shared_ptr<GroupMessage> GroupMessagePtr;
class GroupMessageSending;
typedef std::shared_ptr<GroupMessageSending> GroupMessageSendingPtr;
class UnCompleteGroupMessageManager;
typedef std::shared_ptr<UnCompleteGroupMessageManager> UnCompleteGroupMessageManagerPtr;


class GroupMessage : public std::enable_shared_from_this<GroupMessage>
{
public:
    GroupMessage(const GroupMessageID& groupID);
    ~GroupMessage();

    typedef std::tuple<bool, bool> TestResult;
    static bool isGroupMessage(const TestResult &tr){return std::get<0>(tr);};
    static bool isEndGroupMessage(const TestResult &tr){return std::get<1>(tr);};
    static bool hasEndProperty(const VariantMap *prop);

    MessageID endMessageID() const ;
    GroupMessageID groupMessageID() const;
    // 
    bool isCreatedComplete() const ;
    void setCreatedComplete();
    void copyMessages(BriefBlocksBase &blocks, uint8_t level, bool retransmit);
    void foreachMessage(std::function<void(const MessageID &msg)> &&f);

    void addMessage(const MessageID &msgID, bool isEnd);

    std::string toString();

private:
    // std::vector<MessageID> m_groupMessages;
    BriefBlockMap m_groupMessages;
    GroupMessageID m_groupMessageID;

    // 组消息是否已经加载完所有的消息.
    // 在加载完所有的消息之前这个对象是不玩修正的
    bool m_createdComplete{false};
    MessageID m_endMessageID;
};

// 正在创建中的组对象的管理器,每个destination一个
// 组中的消息可能会跨session.在组创建的过程中,
// 需要在不同的sessionStore中传递这些组实例
// 第一条组消息创建GroupMessage.后续组消息把自己添加到该实例中
// end组消息的时候，该对象创建完毕.
class UnCompleteGroupMessageManager
{
public:    
    struct UnCompleteGroupMessage
    {
        UnCompleteGroupMessage(GroupMessagePtr &gp, uint16_t gs)
            : groupMessage(gp), lastGroupSequence(gs) {}
        GroupMessagePtr groupMessage; 
        uint32_t lastGroupSequence;
    };
    typedef std::shared_ptr<UnCompleteGroupMessage> UnCompleteGroupMessagePtr;
    UnCompleteGroupMessageManager();
    ~UnCompleteGroupMessageManager();

    // 注意：保持操作的原子性
    GroupMessagePtr addGroupMessage(const GroupMessageID &groupID, const MessageID &msgID, bool isEnd);
    GroupMessagePtr appendGroupMessage(const GroupMessageID &groupID, uint32_t groupSeq, const MessageID &msgID, bool isEnd);
    bool testMessage(const GroupMessageID &groupID, uint32_t groupSeq);

    

private:
    // lock/可以不用锁
    std::map<GroupMessageID, UnCompleteGroupMessagePtr> m_unCompleteMap;
};



class GroupMessageSending
{
public:
    GroupMessageSending(const std::string &destName, MessageID &groupEndMessageID, uint8_t level, bool isRetransmit);
    // GroupMessageSending(GroupMessagePtr &groupMessagePtr, bool isRetransmit);
    ~GroupMessageSending();
    bool initGroupSending(GroupMessagePtr &groupMessage);

    uint64_t split(uint64_t count, BriefBlocksBase& messageBlocks);
    void pushBack(BriefBlock &block);

    bool isSendStarted() const;
    void setSendStarted();
    bool isRetransmit() const;
    bool isError() const;
    void setError();
    bool isSendComplete() const;
    MessageID endMessageID() const ;
    int remain() const ;
    GroupMessagePtr getGroupMessage() const;
    // 生成的时候，要设置他的级别
    uint8_t groupLevel() const {return m_level;};


private:
    std::string m_destinationName;
    MessageID m_groupEndMessageID;
    GroupMessagePtr m_groupMessage;
    BriefBlockMap m_waitSend;
    // std::vector<MessageID> m_ackReceived; // 
    bool m_sendStarted; // 已经处于发送中
    uint8_t m_level{0};
    bool m_isRetransmit;
    
    bool m_error;

};

GroupMessageSendingPtr makeNewGroupSendinfo(const std::string &destName, MessageID &groupEndMessageID, uint8_t level, bool isRetransmit);
// #define makeNewGroupSendinfo(gg) nullptr

}
}
