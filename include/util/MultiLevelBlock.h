#pragma once

#include <atomic>
#include "MessageBlocksNew.h"


namespace lightmq { namespace util {

#define LEVEL_IDX(lev) (lev)

#define FOREACH_LEVEL(lev) for (size_t lev = 0; lev <= CommonConstant::MAX_MESSAGE_LEVEL; ++lev)
#define RFOREACH_LEVEL(lev) for (int lev = CommonConstant::MAX_MESSAGE_LEVEL; lev >= 0; --lev)
#define FOREACH_LEVELIDX(lev) for (size_t lev = 0; lev <= CommonConstant::MAX_MESSAGE_LEVEL; ++lev)

class MultiSequence
{
public:
    void reset(); // 清0
    void set(uint8_t lev, uint64_t seq);
    uint64_t get(uint8_t lev);
    void setAll(uint64_t seq); 

    // 初始化为0
    uint64_t sequence[CommonConstant::MAX_MESSAGE_LEVEL_COUNT] = {0};
};

class MultiCursor
{
public:
    MultiCursor();
    ~MultiCursor() = default;
    std::string toString() const;

    const MessageID& get(int level) const; 
    MessageID& get(int level); 
    void setCursor(int level, const MessageID &cursor); 
    const MessageID& oldestCursor() const;
    void setAll(const MessageID &cursor); 
    bool valid() const {return m_inited;}
    void makeValid() {m_inited = true;}
    
private:
    bool m_inited {false}; // 不通用
    MessageID m_cursors[CommonConstant::MAX_MESSAGE_LEVEL_COUNT];
};


class MultiLevelBlock
{
typedef std::function<bool(const uint64_t sessionID)> IsSessionStoreExistFunc;
typedef std::function<bool(const uint64_t sessionID, const  uint64_t sequence)> MatchFunc;
public: 
    MultiLevelBlock(bool lock = false);
    ~MultiLevelBlock();

    // 添加消息的接口
    void pushBack(const BriefBlock &block); 
    void insert(const BriefBlock &block); 
    void removeExpired(std::function<bool(BriefBlock &block)> func);
    bool getFirst(uint64_t size, BriefBlock &block);

    bool isEmpty() const;
    uint64_t getMessageCount();
    // 获从 cursor到最后的消息数量,不包括cursor
    uint64_t getMessageCount(const MultiCursor &cursor);
    
    
    // !!!!!!!!!!!!! 需要移动cursor.
    // 获取消息，不删除
    uint64_t getLevelBlocks(uint64_t level, MessageID &cursor, uint64_t windowSize, BriefBlocksBase &blocks);
    uint64_t getBlocks(MultiCursor &cursor, uint64_t windowSize, BriefBlocksBase & blocks);

    // !!!!!!!!!!!!! 需要移动cursor.
    // 匹配，获取，并且删除消息, 根据级别获取
    uint64_t getLevelBlocks(int lev, 
        IsSessionStoreExistFunc isSessionStoreExistFun, 
	    MatchFunc isMatchFunc, MessageID& cursor,
	    uint64_t ackWindowsSize, uint64_t limitCount, 
        BriefBlocksBase* pMatchedMessages, bool& hasMatchedToEnd);
     uint64_t getBlocks(IsSessionStoreExistFunc isSessionStoreExistFun, 
	    MatchFunc isMatchFunc, MultiCursor& cursor,
	    uint64_t ackWindowsSize, uint64_t limitCount, BriefBlocksBase* pMatchedMessages, bool& hasMatchedToEnd);   

    std::string toString(int maxPrint) const;
protected:
    // queue匹配，取消息
    static uint64_t getNextMatched(BriefBlockMap &blocks, 
        IsSessionStoreExistFunc isSessionStoreExistFun, 
	    MatchFunc isMatchFunc, MessageID& cursor,
	    uint64_t ackWindowsSize, uint64_t limitCount, BriefBlocksBase* pMatchedMessages, bool& hasMatchedToEnd);

    BriefBlockMap& getBlockMap(uint8_t level);
    const BriefBlockMap& getBlockMap(uint8_t level) const;
    void updateCnt(uint64_t dec) {m_count = (m_count > dec ? m_count - dec : 0);}

private:
    // blockMap的指针是一直不为空的
    std::unique_ptr<BriefBlockMap> m_blockMap[CommonConstant::MAX_MESSAGE_LEVEL_COUNT];
    uint64_t m_count{0};
    std::unique_ptr<ILock> m_pLock;
};


}}

