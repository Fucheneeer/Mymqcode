
#pragma once

#include <sstream>
#include <map>
#include <deque>
#include <memory>
#include <mutex>
#include <iostream>
#include "NolockImpl.h"
#include "BrokerTypes.h"
// #include "ISessionScheduleUtility.h"
#include "RecursiveLockImpl.h"
//#include "SessionStoreManager.h"
#include "ILock.h"
#include <functional>
#include "CommonConstant.h"
#include "LogWrapper.h"

namespace lightmq { namespace util {

struct BlockRange
{
    uint64_t start{0}; 
    uint64_t end{0};
    
    BlockRange() {}
    BlockRange(uint64_t s, uint64_t e): start(s), end(e) {}
    bool emptyRange() const;
    size_t messageCnt()const {return end-start+1;};
    void pushBack(std::vector<BlockRange> &ranges) const;
    void pushFront(std::vector<BlockRange> &ranges) const;
};

struct BlockElementBase
{
    typedef MessageID BlockKey;

    enum{
        LEFT_SIDE = 0, 
        INBLOCK, 
        RIGHT_SIDE
    };


    BlockElementBase() {}
    BlockElementBase(MessageID msgID, uint64_t endSeq): messageID(msgID), endSeq(endSeq) {}
    const BlockKey& getKey() const;
    std::string keyString() const;
    size_t messageCnt() const;
    // void absorb(BlockElementBase &);
    // 把ele追加到this后边
    bool append(const BlockElementBase &newEle);
    // test: 检测this是否是newEle的后继
    bool isNextTo(const BlockElementBase &newEle) const;
    void split(size_t head, BlockElementBase &tail);
    std::string toString() const;
    bool checkAndAmend();

    int compareCursor(const BlockKey &bk) const ;
    int getRange(const BlockKey &cursor, uint64_t limit, BlockRange &range, BlockRange &leftRange, BlockRange &rightRange) const;
    void fromRange(const BlockRange &range);
    bool equalRange(const BlockRange &range) const;

    const MessageID& startMessageID() const;
    MessageID& startMessageID() ;
    SessionID sessionID() const;

    MessageID messageID;
    uint64_t endSeq{ 0 };
};

struct BriefBlock  : public BlockElementBase
{
    BriefBlock(): BlockElementBase() {}
    BriefBlock(MessageID mid, uint64_t es, uint8_t level = util::CommonConstant::DEFAULT_MESSAGE_LEVEL, bool isRetransmit = false, bool isGroup = false): BlockElementBase(mid, es), m_level(level), m_isRetransmit(isRetransmit), m_isGroup(isGroup) {}

    // 把ele追加到this后边
    bool append(const BriefBlock &newEle);
    // test: 检测this是否是newEle的后继
    bool isNextTo(const BriefBlock &newEle) const;
    void split(size_t head, BriefBlock &tail);
    std::string toString() const;

    void set(MessageID mid, uint64_t es, uint8_t level = util::CommonConstant::DEFAULT_MESSAGE_LEVEL, bool isRetransmit = false, bool isGroup = false)
    {
        messageID = mid;        
        endSeq = es;
        m_level = level;
        m_isRetransmit = isRetransmit;
        m_isGroup = isGroup;
    }
    uint8_t level() const {return m_level;}

    uint8_t m_level {util::CommonConstant::DEFAULT_MESSAGE_LEVEL};
    bool m_isRetransmit {false};
    bool m_isGroup {false};
};


template <class BlockKey, class BlockElement>
class IMessageBlocks
{
protected:
    typedef IMessageBlocks<BlockKey, BlockElement> MessageBlocksBase;

public:
	class IIterator {
	public:
		IIterator() = default;

		virtual ~IIterator() = default;

		virtual void get(BlockElement &be) const = 0;
		virtual bool next() = 0;
	};
public:
	IMessageBlocks(bool isLock = false)
    {
    	if (isLock) { m_pLock = std::unique_ptr<ILock>(new RecursiveLockImpl()); } 
        else { m_pLock = std::unique_ptr<ILock>(new NolockImpl()); }
    }

    // virtual void pushBack(MessageID messageID, uint64_t endSeq = 0, bool isRetransmit = false) = 0;
    virtual void pushBack(const BlockElement &be) = 0;

	virtual bool getFirst(BlockElement &be) = 0;

	virtual bool getFirst(uint64_t size, BlockElement &be) = 0;

    virtual bool isEmpty() const = 0;

    virtual uint64_t split(uint64_t count, MessageBlocksBase& blockMessage) = 0;

    virtual uint64_t removeExpired(std::function<bool(BlockElement &block)> expiredFun) = 0;

	virtual uint64_t getMessageCount() const = 0;

    virtual void clear() = 0;

	void lock() { m_pLock->lock(); }
	void unlock() { m_pLock->unlock(); }
	bool try_lock() { return m_pLock->try_lock(); }

    virtual void dump(const std::string& message) const = 0;
    virtual std::string toString(int maxPrint, uint32_t *blockCnt = nullptr) const = 0;

	virtual std::unique_ptr<IIterator> getIterator() = 0;

    inline void subtract(uint64_t &t, uint64_t sub)
    {
        if (t <= sub) t = 0;
        else t -= sub;
    }

protected:
    std::unique_ptr<ILock> m_pLock;
};

template <class BlockKey, class BlockElement>
class MessageBlocksMapImpl : public IMessageBlocks<BlockKey, BlockElement>
{
    typedef std::map<BlockKey, BlockElement> BlockMap;
    typedef IMessageBlocks<BlockKey, BlockElement> MessageBlocksBase;
    using MessageBlocksBase::m_pLock;

    class Iterator : public MessageBlocksBase::IIterator {
    public:
        Iterator(typename BlockMap::reverse_iterator rend, typename BlockMap::reverse_iterator rbegin);
        virtual void get(BlockElement &be) const override;
        virtual bool next() override;
    private:
        typename BlockMap::reverse_iterator m_rend;
        typename BlockMap::reverse_iterator m_rbegin;
        typename BlockMap::reverse_iterator m_current;
    };
public:
    MessageBlocksMapImpl(bool isLock = false);

    void insert(const BlockElement &be);

    virtual void pushBack(const BlockElement &be) override;
    virtual bool getFirst(BlockElement &be) override;
    virtual bool getFirst(uint64_t size, BlockElement &be) override;

    // 从cursor处遍历，不包括cursor,也不移动cursor
    // 不修改数据
    void forward(const BlockKey &cursor, 
        std::function<bool(const BlockElement &be, const BlockRange &range, bool isEnd)> f);

    void getNextMatched(const BlockKey &cursor, 
            uint64_t limitCount, 
            std::function<bool(const BlockElement &be, const BlockRange &range, bool isEnd, bool &removed, std::vector<BlockRange> &newElement)> f);

    //bool remove(MessageBlocks& blockMessage);

    virtual bool isEmpty() const override;

    virtual uint64_t split(uint64_t count, MessageBlocksBase& messageBlocks) override;

    // virtual void removeExpired(ISessionScheduleUtility* pSessionScheduleUtility) override;
    virtual uint64_t removeExpired(std::function<bool(BlockElement &block)> expiredFun) override;


    virtual uint64_t getMessageCount() const override {
        return m_totalCount;
    }

    virtual void clear() override;

    virtual void dump(const std::string& message) const override;
    virtual std::string toString(int maxPrint, uint32_t *blockCnt = nullptr) const;

    virtual std::unique_ptr<typename MessageBlocksBase::IIterator> getIterator() override;

private:
    std::map<BlockKey, BlockElement> m_messageMap;
    uint64_t m_totalCount{ 0 };
};


template <class BlockKey, class BlockElement>
class MessageBlocksDequeImpl : public IMessageBlocks<BlockKey, BlockElement>
{
    typedef std::deque<BlockElement> BlockDeque;
    typedef IMessageBlocks<BlockKey, BlockElement> MessageBlocksBase;
    using MessageBlocksBase::m_pLock;

    class Iterator : public MessageBlocksBase::IIterator {
    public:
        Iterator(typename BlockDeque::reverse_iterator rend, typename BlockDeque::reverse_iterator rbegin);
        virtual void get(BlockElement &be) const override;
        virtual bool next() override;
    private:
        typename BlockDeque::reverse_iterator m_rend;
        typename BlockDeque::reverse_iterator m_rbegin;
        typename BlockDeque::reverse_iterator m_current;
    };

public:

    MessageBlocksDequeImpl(bool isLock = false);

    virtual void pushBack(const BlockElement &be) override;
    virtual bool getFirst(BlockElement &be) override;
    virtual bool getFirst(uint64_t size, BlockElement &be) override;

    virtual bool isEmpty() const override;

    virtual uint64_t split(uint64_t count, MessageBlocksBase &blockMessage) override;
    // virtual void removeExpired(ISessionScheduleUtility* pSessionScheduleUtility) override;
    virtual uint64_t removeExpired(std::function<bool(BlockElement &block)> expiredFun) override;

    virtual uint64_t getMessageCount() const override{
        return m_totalCount;
    }

    virtual void clear() override;

    virtual void dump(const std::string& message) const override;
    virtual std::string toString(int maxPrint, uint32_t *blockCnt = nullptr) const ;

    virtual std::unique_ptr<typename MessageBlocksBase::IIterator> getIterator() override;

private:
    BlockDeque m_messageDeque;
    uint64_t m_totalCount{ 0 };
};

template <class BlockKey, class BlockElement>
MessageBlocksMapImpl<BlockKey, BlockElement>::MessageBlocksMapImpl(bool isLock) :MessageBlocksBase(isLock) {}


// 22/10/09-10:29:47 ghli add
// insert 没有检测前后元素，没有执行合并， todo...
template <class BlockKey, class BlockElement>
void MessageBlocksMapImpl<BlockKey, BlockElement>::insert(const BlockElement &be_)
{
	std::lock_guard<ILock> lock(*m_pLock);
    auto be = be_;
    auto *pbe = &be;
    if (!be.checkAndAmend()) return ;

    // 不允许重复插入
    // auto k = be.getKey();
    bool absorbed = false;
	auto it = m_messageMap.lower_bound(be.getKey());
    if (it->second.getKey() == be.getKey())
    { // 
        DEBUG_PRINT("Blocks map insert failed with dumplicated keys <%s>", be.keyString().c_str());
        return ; // key 不能重复 
    }

    if (it != m_messageMap.begin())
    {
        auto itLeft = it;
        --itLeft;
        if (it->second.isNextTo(be))
        {
            itLeft->second.append(be);
            pbe = &(itLeft->second);
            absorbed = true;
        }
    }

    do {
        if (m_messageMap.end() == it) break;
        if (!it->second.isNextTo(*pbe)) break;

        pbe->append(it->second);
        
        m_messageMap.erase(it);
        if (!absorbed) { // left absobed
            m_messageMap.emplace(pbe->getKey(), *pbe);
            absorbed = true;
        }
   
    } while (0);

    if (!absorbed) 
    {
        m_messageMap.emplace(pbe->getKey(), *pbe);
        absorbed = true;
    }

    m_totalCount += be_.messageCnt();
}

template <class BlockKey, class BlockElement>
void MessageBlocksMapImpl<BlockKey, BlockElement>::pushBack(const BlockElement &be_)
{
	std::lock_guard<ILock> lock(*m_pLock);
    auto be = be_;
    if (!be.checkAndAmend()) return ;

	auto iter = m_messageMap.rbegin();
	if (iter != m_messageMap.rend() && iter->second.append(be))
	{
		m_totalCount += be.messageCnt();
	}
	else
	{
		insert(be);
	}
}

template <class BlockKey, class BlockElement>
bool MessageBlocksMapImpl<BlockKey, BlockElement>::getFirst(BlockElement &be)
{
	std::lock_guard<ILock> lock(*m_pLock);
	if (m_messageMap.empty())
		return false;

	auto iter = m_messageMap.begin();
    be = iter->second;
    this->subtract(m_totalCount, iter->second.messageCnt());
	m_messageMap.erase(iter);
	return true;
}

template <class BlockKey, class BlockElement>
bool MessageBlocksMapImpl<BlockKey, BlockElement>::getFirst(uint64_t size, BlockElement &be)
{
	std::lock_guard<ILock> lock(*m_pLock);
	if (m_messageMap.empty() || size == 0)
		return false;

	auto mapIter = m_messageMap.begin();
    be = mapIter->second;

	while (mapIter != m_messageMap.end())
	{
        this->subtract(m_totalCount, mapIter->second.messageCnt());
		mapIter = m_messageMap.erase(mapIter);
		if (be.messageCnt() > size) {
			// we have already more message than size, so break it and insert back
            BlockElement rbe;
            be.split(size, rbe);
            insert(rbe);
			break;
		}
		else if (m_messageMap.end() != mapIter && mapIter->second.isNextTo(be)) {
			// if next block is next to current block, merge it
			be.append(mapIter->second);
		}
		else {
			// if not , break it, as we only need to return 1 block
			break;
		}
	}
	return true;
}


template <class BlockKey, class BlockElement>
void MessageBlocksMapImpl<BlockKey, BlockElement>::forward(const BlockKey &cursor, 
        std::function<bool(const BlockElement &be, const BlockRange &range, bool isEnd)> f)
{
    thread_local std::vector<BlockRange> vecElement;
	std::lock_guard<ILock> lock(*m_pLock);
	if (m_messageMap.empty()) {
		return ;
	}

	auto iter = m_messageMap.lower_bound(cursor);  //  lower_boud: iterator where >= cursorMesssageID
	if (iter != m_messageMap.begin()) {
		--iter;
        // 检测前边的这个元素是否也包含corsor.
        if (iter->second.compareCursor(cursor) == BlockElementBase::RIGHT_SIDE) ++iter;
	}

#define ISLAST(is, m, it) bool is(false); do {auto i(it); is = ((++i) == m.end());}while(0)

    // uint64_t beginPos{0}, endPos{0};
	while (iter != m_messageMap.end())
    {
        BlockRange range, leftRange, rightRange;

        iter->second.getRange(cursor, 0, range, leftRange, rightRange);
        if (range.emptyRange())
        {
            ++iter;
            continue;
        }

        ISLAST(isLast, m_messageMap, iter);
        bool stopLoop = !f(iter->second, range, isLast);
        ++iter; 
        
        if (stopLoop) break;
    }
    
    return ;
}

template <class BlockKey, class BlockElement>
void MessageBlocksMapImpl<BlockKey, BlockElement>::getNextMatched(const BlockKey &cursor, 
        uint64_t limitCount, 
        std::function<bool(const BlockElement &be, const BlockRange &range, bool isEnd, bool &removed, std::vector<BlockRange> &newElement)> f)
{
    thread_local std::vector<BlockRange> vecElement;
	//dump("getNextMatched start");
	std::lock_guard<ILock> lock(*m_pLock);
	if (m_messageMap.empty()) {
		return ;
	}

	int64_t remain = (uint64_t)limitCount;
	auto iter = m_messageMap.lower_bound(cursor);  //  lower_boud: iterator where >= cursorMesssageID
	if (iter != m_messageMap.begin()) {
		--iter;			
        // 检测前边的这个元素是否也包含corsor.
        if (iter->second.compareCursor(cursor) == BlockElementBase::RIGHT_SIDE) ++iter;
	}

#define ISLAST(is, m, it) bool is(false); do {auto i(it); is = ((++i) == m.end());}while(0)

    // uint64_t beginPos{0}, endPos{0};
	while (iter != m_messageMap.end() && remain > 0)
    {
        BlockRange range, leftRange, rightRange;

        remain -= iter->second.getRange(cursor, remain, range, leftRange, rightRange);
        if (range.emptyRange())
        {
            ++iter;
            continue;
        }
        BlockElement backBe = iter->second;
        bool removed = false;
        vecElement.clear();
        ISLAST(isLast, m_messageMap, iter);
        bool stopLoop = !f(iter->second, range, isLast && rightRange.emptyRange(), removed, vecElement);
        if (removed)
        {
            leftRange.pushFront(vecElement);
            rightRange.pushBack(vecElement);
            this->subtract(m_totalCount, iter->second.messageCnt());
            m_messageMap.erase(iter++);
        }
        else
        {
            ++iter; 
        }

        if (removed && !vecElement.empty())
        {
            for (size_t i = 0; i < vecElement.size(); ++i)
            {
                backBe.fromRange(vecElement[i]);
                insert(backBe);
            }
        }
        
        if (stopLoop) break;
    }
    
    return ;
}

template <class BlockKey, class BlockElement>
bool MessageBlocksMapImpl<BlockKey, BlockElement>::isEmpty() const
{
	std::lock_guard<ILock> lock(*m_pLock);
	return m_messageMap.empty();
}

template <class BlockKey, class BlockElement>
uint64_t MessageBlocksMapImpl<BlockKey, BlockElement>::split(uint64_t count, MessageBlocksBase& messageBlocks)
{
	std::lock_guard<ILock> lock(*m_pLock);
    if (m_messageMap.empty() || count == 0)
	    return 0;
	//dump("split start");
	//blockMessage.dump("blockMessage start");		
	count = std::min(count, m_totalCount);
	// uint64_t splitCount = count;
	auto iter = m_messageMap.begin();
	while (count > 0 && m_messageMap.end() != iter)
	{
		uint64_t blockCount = iter->second.messageCnt();

        if (blockCount <= count)
		{//need count more than the block
			messageBlocks.pushBack(iter->second);
			iter = m_messageMap.erase(iter);
			count -= blockCount;
			m_totalCount -= blockCount;
		}
		else
		{//block has message surplus
            BlockElement be = iter->second;
            BlockElement rbe;
            be.split(count, rbe);
			messageBlocks.pushBack(be);
			m_messageMap.erase(iter);
            m_messageMap.emplace(rbe.getKey(), rbe);
			m_totalCount -= count;
            count = 0;
            break;
		}
	}
	//dump("split end");
	//blockMessage.dump("blockMessage");
	return messageBlocks.getMessageCount();
}

// void MessageBlocksMapImpl::removeExpired(ISessionScheduleUtility* pSessionScheduleUtility)
template <class BlockKey, class BlockElement>
uint64_t MessageBlocksMapImpl<BlockKey, BlockElement>::removeExpired(std::function<bool(BlockElement &block)> expiredFun)
{
    uint64_t removedCnt = 0;
	std::lock_guard<ILock> lock(*m_pLock);
	while (!m_messageMap.empty())
	{
		auto iter = m_messageMap.begin();
		if (expiredFun(iter->second)) {
			// remove all expired block first
            removedCnt += iter->second.messageCnt();
			m_messageMap.erase(iter);
		}
		else {
			break;
		}
	}

    return removedCnt;
}

template <class BlockKey, class BlockElement>
void MessageBlocksMapImpl<BlockKey, BlockElement>::clear()
{
	std::lock_guard<ILock> lock(*m_pLock);
	m_messageMap.clear();
	m_totalCount = 0;
}

template <class BlockKey, class BlockElement>
void MessageBlocksMapImpl<BlockKey, BlockElement>::dump(const std::string& message) const {
	std::cout << message << " MessageBlocksMap::dump" << this->toString(0) << std::endl;
}

template <class BlockKey, class BlockElement>
std::string MessageBlocksMapImpl<BlockKey, BlockElement>::toString(int maxPrint, uint32_t *blockCnt) const
{
	std::lock_guard<ILock> lock(*m_pLock);

    if (maxPrint <= 0) maxPrint = -1;

    std::stringstream ss;
    ss << "MessageBlocksMap info: totalCount<" << m_totalCount << "> blockCount<" << m_messageMap.size() << "> ";
    for (auto it = m_messageMap.begin(); it != m_messageMap.end(); ++it)
    {
        if (nullptr != blockCnt) ++(*blockCnt);
        ss << "<" << it->second.toString() << ">, "; 

        if (maxPrint == 1) 
        {
            ss << "...";
            break;
        }

        maxPrint--;
    }

    return ss.str();
}

template <class BlockKey, class BlockElement>
std::unique_ptr<typename IMessageBlocks<BlockKey, BlockElement>::IIterator> MessageBlocksMapImpl<BlockKey, BlockElement>::getIterator() {
	return std::unique_ptr<typename MessageBlocksBase::IIterator>(new Iterator(m_messageMap.rend(), m_messageMap.rbegin()));
}

template <class BlockKey, class BlockElement>
MessageBlocksMapImpl<BlockKey, BlockElement>::Iterator::Iterator(typename BlockMap::reverse_iterator rend, typename BlockMap::reverse_iterator rbegin)
{
	m_rend = rend;
	m_rbegin = rbegin;
	m_current = m_rend;
}

template <class BlockKey, class BlockElement>
void MessageBlocksMapImpl<BlockKey, BlockElement>::Iterator::get(BlockElement &be) const
{
	if (m_current == m_rend) {
		return;
	}
    be = m_current->second;
}

template <class BlockKey, class BlockElement>
bool MessageBlocksMapImpl<BlockKey, BlockElement>::Iterator::next()
{
	if (m_current == m_rbegin)
		return false;
	--m_current;
	return true;
}



//////////////////////////////////////////////////////////////////////////
//  begin MessageBlocksDequeImpl
//////////////////////////////////////////////////////////////////////////
template <class BlockKey, class BlockElement>
MessageBlocksDequeImpl<BlockKey, BlockElement>::MessageBlocksDequeImpl(bool isLock) :MessageBlocksBase(isLock) {}

template <class BlockKey, class BlockElement>
void MessageBlocksDequeImpl<BlockKey, BlockElement>::pushBack(const BlockElement &be_)
{
	std::lock_guard<ILock> lock(*m_pLock);
    auto be = be_;	
    if (!be.checkAndAmend()) return ;
    ///////******
    // becare full...
	// if (endSeq == 0)
	// 	endSeq = messageID.sequence;
	// else if (endSeq < messageID.sequence){
	// 		return;
	// }
    ///////*****


	auto iter = m_messageDeque.rbegin();
	if (m_messageDeque.rend() != iter && be.isNextTo(*iter))
	{
		iter->append(be);
	}
	else {
		m_messageDeque.emplace_back(be);
	}
	m_totalCount += be.messageCnt();
}

template <class BlockKey, class BlockElement>
bool MessageBlocksDequeImpl<BlockKey, BlockElement>::getFirst(BlockElement &be)
{
	std::lock_guard<ILock> lock(*m_pLock);
	if (m_messageDeque.empty())
		return false;

	be = m_messageDeque.front();
	m_messageDeque.pop_front();
	m_totalCount -= be.messageCnt();
	return true;
}

template <class BlockKey, class BlockElement>
bool MessageBlocksDequeImpl<BlockKey, BlockElement>::getFirst(uint64_t size, BlockElement &be)
{
	std::lock_guard<ILock> lock(*m_pLock);
	if (m_messageDeque.empty() || size == 0)
		return false;

	auto iter = m_messageDeque.begin();

	if (iter != m_messageDeque.end()) {
		// messageID = iter->messageID;
		// isRetransmit = iter->isRetransmit;
		// endSeq = iter->endSeq;

		if (iter->messageCnt() > size) {
            be = *iter;
            be.split(size, *iter); // 不用删除
		}
		else
        {
            be = *iter;
			m_messageDeque.erase(iter);
        }
	}
	m_totalCount -= be.messageCnt();
	return true;
}

template <class BlockKey, class BlockElement>
bool MessageBlocksDequeImpl<BlockKey, BlockElement>::isEmpty() const
{
	std::lock_guard<ILock> lock(*m_pLock);
	return m_messageDeque.empty();
}

template <class BlockKey, class BlockElement>
uint64_t MessageBlocksDequeImpl<BlockKey, BlockElement>::split(uint64_t count, MessageBlocksBase& blockMessage)
{
	std::lock_guard<ILock> lock(*m_pLock);
	//dump("split start");
	//blockMessage.dump("blockMessage start");
	if (m_messageDeque.empty() || count == 0)
		return 0;
	//int64_t count_ = count;
	//auto totalCount = m_totalCount;
	auto iter = m_messageDeque.begin();
	count = std::min(count, m_totalCount);

	while (count > 0)
	{
		uint64_t blockCount = iter->messageCnt();
        if (blockCount <= count)
		{//need count more than the block
			const auto &be = m_messageDeque.front();
			blockMessage.pushBack(be);
			iter = m_messageDeque.erase(iter);
			count -= blockCount;
			m_totalCount -= blockCount;
		}
		else
		{//block has message surplus
			auto be = *iter;
            be.split(count, *iter);
			blockMessage.pushBack(be);

			m_totalCount -= count;
            count = 0;
			break;
		}
	}
	return blockMessage.getMessageCount();
}

// void MessageBlocksDequeImpl<BlockKey, BlockElement>::removeExpired(ISessionScheduleUtility* pSessionScheduleUtility)
template <class BlockKey, class BlockElement>
uint64_t MessageBlocksDequeImpl<BlockKey, BlockElement>::removeExpired(std::function<bool(BlockElement &block)> expiredFun)
{
    uint64_t removedCnt = 0;
	std::lock_guard<ILock> lock(*m_pLock);
	while (!m_messageDeque.empty())
	{
		auto iter = m_messageDeque.begin();
		if (expiredFun(*iter)) {
			// remove all expired block first
            removedCnt += iter->messageCnt();
			m_messageDeque.erase(iter);
		}
		else {
			break;
		}
	}

    return removedCnt;
}

template <class BlockKey, class BlockElement>
void MessageBlocksDequeImpl<BlockKey, BlockElement>::clear()
{
	std::lock_guard<ILock> lock(*m_pLock);
	m_messageDeque.clear();
	m_totalCount = 0;
}

template <class BlockKey, class BlockElement>
void MessageBlocksDequeImpl<BlockKey, BlockElement>::dump(const std::string& message) const {
	std::cout << message << " MessageBlocksDequeImpl::dump:" << toString(0) << std::endl;;
}

template <class BlockKey, class BlockElement>
std::string MessageBlocksDequeImpl<BlockKey, BlockElement>::toString(int maxPrint, uint32_t *blockCnt) const
{
    std::stringstream ss;
    ss << "MessageBlocksDequeImpl info: total count<" << m_totalCount << "> blockCount <" << m_messageDeque.size() << "> ";
	for (auto& kv : m_messageDeque) {
        ss << "<" << kv.toString() << "> ";
        if (nullptr != blockCnt) ++(*blockCnt);
        if (maxPrint == 1)
        {
            ss << "..."; 
            break;
        }
        maxPrint--;
    }

    return ss.str();
}

template <class BlockKey, class BlockElement>
std::unique_ptr<typename IMessageBlocks<BlockKey, BlockElement>::IIterator> MessageBlocksDequeImpl<BlockKey, BlockElement>::getIterator()
{
	return std::unique_ptr<typename MessageBlocksBase::IIterator>(new Iterator(m_messageDeque.rend(), m_messageDeque.rbegin()));
}

template <class BlockKey, class BlockElement>
MessageBlocksDequeImpl<BlockKey, BlockElement>::Iterator::Iterator(typename BlockDeque::reverse_iterator rend, typename BlockDeque::reverse_iterator rbegin)
{
	m_rend = rend;
	m_rbegin = rbegin;
	m_current = m_rend;
}

template <class BlockKey, class BlockElement>
void MessageBlocksDequeImpl<BlockKey, BlockElement>::Iterator::get(BlockElement &be) const
{
	if (m_current == m_rend) {
		return;
	}
    be = *m_current;

}

template <class BlockKey, class BlockElement>
bool MessageBlocksDequeImpl<BlockKey, BlockElement>::Iterator::next()
{
	if (m_current == m_rbegin)
		return false;
	--m_current;
	return true;
}

typedef IMessageBlocks<MessageID, BriefBlock> BriefBlocksBase;
typedef MessageBlocksMapImpl<MessageID, BriefBlock> BriefBlockMap;
typedef MessageBlocksDequeImpl<MessageID, BriefBlock> BriefBlockDeque;



}}
