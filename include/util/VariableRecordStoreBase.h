////////////////////////////////////////////////////////////////////////////////
//
// VariableRecordStoreBase.h
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

#ifndef LIGHTMQ_LIGHTMQUTIL_INC_BASELIGHTMQVARIABLERECORDSTORE_H_
#define LIGHTMQ_LIGHTMQUTIL_INC_BASELIGHTMQVARIABLERECORDSTORE_H_

#include "IVariableRecordStore.h"

#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <string>
#include <chrono>
#include <tuple>
#include <mutex>
#include <functional>
#include <climits>
#include <map>

#include "DEPException.h"
#include "VariantMap.h"
#include "LightmqMessageHead.h"
#include "ISeverityLog.h"
#include "GroupMessageManager.h"
#include "CommonConstant.h"
#include "MultiLevelBlock.h"


namespace lightmq { namespace util {

struct RecordStoreDef {
protected:
    using FileNumberType = uint32_t; /**< Data type for file number in index file */
    using DataOffsetType = uint64_t; /**< Data type for data offset in data file */
    using DataStartIndexType = uint64_t; /**< Data type for data start offset in data file */
    using DataLengthType = uint32_t; /**< Data type for data length in data memory buffer */
    using InfoOffsetType = uint64_t; /**< INFO type for data offset in index file */
    using TimestampType = int64_t; /**< Data type for timestamp in index file */
    using MessageAttributeType = uint8_t; /**< Data type for messageAttribute */
    using MessageMarkType = uint8_t; /** <Data type for message mark */
    using ReservedByteType = uint8_t; /**< Data type for ReservedByteType */
    using AtrributeStringLengthType = uint16_t; /**< Message dep::Properties length */
public:
    static constexpr uint8_t DEFAULT_MESSAGE_ATTRIBUTE = 0;
    static constexpr uint8_t DEFAULT_MESSAGE_MARK = 0;
    static constexpr uint8_t HAS_ATTRIBUTE_MESSAGE_MASKER = 0x1;
    static constexpr uint8_t DELETED_MESSAGE_MASKER = 0x2;
    static constexpr uint8_t REMOTE_MESSAGE_MASKER = 0x4;
    static constexpr uint8_t EXPIRED_MESSAGE_MASKER = 0x8;

	static constexpr uint8_t MESSAGE_ATTR_MASK_TRANSACTION = 0x38;  //  事务性消息标记
	static constexpr uint8_t MESSAGE_TRANSACTION_NA        = 0;
	static constexpr uint8_t MESSAGE_TRANSACTION_START     = 1;
	static constexpr uint8_t MESSAGE_TRANSACTION_COMMIT    = 2;
	static constexpr uint8_t MESSAGE_TRANSACTION_ROLL_BACK = 3;

	inline static uint8_t getMessageTransactionState(MessageAttributeType attr) {
		return (attr & MESSAGE_ATTR_MASK_TRANSACTION) >> 3;
	}

	inline static void setMessageTransactionState(MessageAttributeType &attr, uint8_t transState) {
		attr &= (~MESSAGE_ATTR_MASK_TRANSACTION);
		attr |= (transState << 3);
	}

    static constexpr uint64_t DEFAULT_MAX_FILE_SIZE = 1024 * 1024 * 1024; /**< Default maximum data file size **/
    static constexpr uint64_t DEFAULT_MAX_MEMORY_DATA_SIZE = 1024 * 1024 * 1024; /**< Default maximum data memory size **/
    static constexpr uint64_t DEFAULT_MEMORY_DATA_SIZE = 1024 * 1024; /**< Default maximum data memory size **/
    static constexpr uint32_t INDEX_RECORD_LENGTH = sizeof(FileNumberType) + sizeof(DataOffsetType) + sizeof(InfoOffsetType) + sizeof(TimestampType) + sizeof(MessageMarkType) + sizeof(MessageAttributeType) + sizeof(ReservedByteType) + sizeof(ReservedByteType);

};

template <class LockType = dep::NoLock>
class VariableRecordStoreBase : public IVariableRecordStoreLocked<LockType>, public RecordStoreDef {
public:
    /**
     * Options to create record store
     */
    struct StoreOptions {
        /**
         * Constructor
         */
        StoreOptions() : append(false), deleteOnExit(false), maxFileSize(DEFAULT_MAX_FILE_SIZE), referenceTime(0), baseSequence(0), maxMemorySize(DEFAULT_MAX_MEMORY_DATA_SIZE), defaultMemorySize(DEFAULT_MEMORY_DATA_SIZE){}

        bool append;           /**< Open in append mode */
        bool deleteOnExit;     /**< Delete all index and data files on exit */
        uint64_t maxFileSize;  /**< Maximum data file size for each data file */
        uint32_t referenceTime; /**< Reference time for record timestamps */
        uint32_t baseSequence;     /**< index of the base record */
        uint64_t maxMemorySize;  /**< Maximum memory size for each data memory store */
        uint64_t defaultMemorySize;  /**< default memory size for each data memory store */

    };

public:
    VariableRecordStoreBase(SessionID sessionID, const StoreOptions& options, const std::string& storeName, UnCompleteGroupMessageManagerPtr &ucgmmp);
    virtual ~VariableRecordStoreBase() = default;

protected:
    bool testGroupMessage(VariantMap *messageProperties) const;
    void testAndAddGroupInfo(uint64_t seq, VariantMap *messageProperties);
    uint8_t addMessageAttribute(uint64_t seq,
        MessageAttributeType msgAttrMask,
        VariantMap *messageProperties);
    // 初始化，把普通级别的消息变更为特殊级别
    bool moveRecordLevel(uint64_t seq, uint8_t level) ;
    // 
    void addLevelInfo(uint64_t seq, uint8_t level) ;
    bool getDefaultLevelBrief(uint64_t *count, uint64_t *firstSeq, uint64_t *lastSeq) const;
    void fillDefaultLevelInfo(uint64_t recordCount);
    void initRecordLevel(uint64_t seq, uint8_t level);
    void checkLevelInfo(uint64_t seq, uint8_t level);

public:
    virtual uint64_t getOldestDeletedSequence() const override;
    virtual uint64_t getLastHasProperitesMessageSequence() const override;
    virtual bool isDeleted(uint64_t seq) const override;
    virtual bool isDeletedOrRemoteMessage(uint64_t seq) const override;
    virtual const VariantMap* getMessageProperties(uint64_t sequence) const override;
    void markDeleted(uint64_t start, uint64_t end) override;
    bool markExpired(uint64_t start, uint64_t end) override;
    virtual std::vector<uint64_t> getDeletedSequences(uint64_t start, uint64_t end) const override;
    virtual std::vector<uint64_t> getExpiredSequences(uint64_t start, uint64_t end) const override;
    virtual std::vector<uint64_t> getRemoteSequences(uint64_t start, uint64_t end) const override;
    virtual std::vector<uint64_t> getRemoteOrDeletedSequences(uint64_t start, uint64_t end) const override;
    virtual std::vector<uint64_t> getNotDeletedSequences() const override;
    virtual void setSequenceReady(uint64_t seq) override;
    virtual void setSequenceNotReady(uint64_t seq) override;
    virtual void setAllSequenceReady() override;
    virtual std::tuple<uint64_t, uint64_t> getLastLocalMessageSequence() const override;
	virtual uint64_t getBeginSequence() const override;
	virtual uint64_t getRecordCount() override;
	virtual void setNewBeginSequence(uint64_t newBeginSequence) override;
    virtual LockType &getLock() override;
    virtual std::vector<uint64_t> queryMessageIDs(const std::string& providerID, std::shared_ptr<ISQLExpression> spSelector, const std::string& msgKeyWord, uint64_t startTime, uint64_t endTime, uint64_t timeoutMs, bool& searchCompleted) override;
	virtual uint8_t getTransState(uint64_t seq) const override; // m_transactionSequences

    virtual GroupMessage* getGroupMessagePtr(const MessageID &messageID) const override;
    virtual bool checkGroupMessage(const MessageID &begin, uint64_t seq, IVariableRecordStore::ChkGroupResult *first) const override;

    // uint64_t getLevelBlocks(int lev, MessageID &cursor, BriefBlocksBase & blocks, bool needRemove);
    bool checkDefaultLevel(uint64_t startSeq, uint64_t endSeq, IVariableRecordStore::ChkLevelResult *result) const override;
    uint8_t getRecordLevel(uint64_t sequence) const override;
    bool getLevelBrief(uint8_t lev, uint64_t *count, uint64_t *firstSeq, uint64_t *lastSeq) const override;
    // cursor 是level级别下，最后一条回复的记录的sequence,需要查找该级别下是否还有消息
    uint64_t getFirstNoAckByLevel(uint8_t lev, uint64_t cursor) const override;

protected:
    std::string m_storeName;
    int64_t m_referenceTime;
    uint32_t m_baseSequence;       /**< index of the base record */
    uint64_t m_beginSequence;       /**< index of the begin record , Its same in file session store*/
    uint64_t m_recordCount;
    SessionID m_sessionID;
    mutable LockType m_lock;
    
    std::unordered_set<uint64_t> m_expiredSequenceSet; // expired message 
    std::unordered_set<uint64_t> m_deletedSequenceSet;
    std::unordered_set<uint64_t> m_remoteSequenceSet;
    std::unordered_set<uint64_t> m_notDeleteSequenceSet;
    std::unordered_set<uint64_t> m_notReadySequenceSet;
	std::unordered_map<uint64_t, uint8_t> m_transactionSequences;

    uint64_t m_lastHasPropertiesMessageSeq{ 0 };
    std::unordered_map<uint64_t, std::shared_ptr<VariantMap> > m_messageProperties;

    // group Message 
    std::map<uint64_t, GroupMessagePtr > m_groupMessages; // for lower_bound
    std::map<uint64_t, GroupMessagePtr > m_groupEndMessages; // for lower_bound
    std::set<GroupMessageID> m_groupMessage; //
    // 记录对应的priority.  // 默认的priority不记录
    // sequence, level
    std::map<uint64_t, uint8_t> m_recordPriority;
    // 每一级包含的消息
    std::vector<uint64_t> m_levelRecords[CommonConstant::MAX_MESSAGE_LEVEL_COUNT];
    // 默认级别的消息
    // 生产环境下，大部分消息都是默认级别消息，这里是对性能的一个优化,思路如下：
    // 默认全是default level. 碰到其他级别的消息，从map中扣掉这个元素,最终初始化完毕之后，级别数据就生成了。
    // <key: range start, key: range end.>
    std::map<uint64_t, uint64_t> m_defaultLevelRecord; // 

    
    UnCompleteGroupMessageManagerPtr m_unCompleteGroupMessage;
    

protected:
    virtual void markAttribute(uint64_t start, uint64_t end, MessageAttributeType marker) = 0;

    virtual bool getIsSequenceReady(uint64_t seq);

    virtual void initQueryContext() {}
    virtual void destroyQueryContext() {};
    virtual uint64_t getIndexTimeStamp(uint64_t seq) = 0;
    virtual uint64_t findSeqByTimeStamp(uint64_t timeStamp, uint64_t startSeq, uint64_t endSeq, bool findHead);
    //return message data and providerID
    virtual std::tuple<std::string, std::string> getRecordData(uint64_t index) = 0;
};



template <class LockType /*= dep::NoLock*/>
uint64_t lightmq::util::VariableRecordStoreBase<LockType>::findSeqByTimeStamp(uint64_t timeStamp, uint64_t startSeq, uint64_t endSeq, bool findHead)
{
    if (endSeq == startSeq) {
        return startSeq;
    }
    if (endSeq - startSeq == 1  ) {
        if (findHead) {
            return timeStamp == getIndexTimeStamp(startSeq) ? startSeq : endSeq;
        }
        else {
            return timeStamp == getIndexTimeStamp(endSeq) ? endSeq : startSeq;
        }
    }
    uint64_t middleSeg = startSeq + (endSeq - startSeq) / 2;
    uint64_t middleTimeStamp = getIndexTimeStamp(middleSeg);
    if (middleTimeStamp == timeStamp) {
        return middleSeg;
    }
    else if (middleTimeStamp > timeStamp) {
        return findSeqByTimeStamp(timeStamp, startSeq, middleSeg, findHead);
    }
    else {
        return findSeqByTimeStamp(timeStamp, middleSeg, endSeq, findHead);
    }
}

template <class LockType /*= dep::NoLock*/>
std::vector<uint64_t> lightmq::util::VariableRecordStoreBase<LockType>::queryMessageIDs(const std::string& providerID, std::shared_ptr<ISQLExpression> spSelector, const std::string& msgKeyWord, uint64_t startTime, uint64_t endTime, uint64_t timeoutMs, bool& searchCompleted)
{
    searchCompleted = true;
    auto startTimePoint = std::chrono::system_clock::now();
    std::vector<uint64_t> result;

    if (0 == m_recordCount) {
        return result;
    }

    if (startTime > endTime) {
        std::swap(startTime, endTime);
    }

    initQueryContext();

    uint64_t startSeq = 1;
    uint64_t endSeq = m_recordCount;
    {
        std::lock_guard<decltype(m_lock)> lock(m_lock);
        TimestampType firstTimeStamp = getIndexTimeStamp(m_baseSequence);
        TimestampType lastTimeStamp = getIndexTimeStamp(m_recordCount);

        //not in the time stamp range
        if ( (startTime != 0 && startTime > lastTimeStamp) || (endTime != 0 && endTime < firstTimeStamp)) {
            return result;
        }
        
        if (startTime != 0 && startTime >= firstTimeStamp) {
            startSeq = findSeqByTimeStamp(startTime, 1, m_recordCount, true);
        }

        if (endTime != 0 && endTime <= lastTimeStamp) {
            endSeq = findSeqByTimeStamp(endTime, 1, m_recordCount, false);
        }
    }

    size_t matchingCount = 1;
    for (uint64_t i = startSeq; i <= endSeq; ++i, ++matchingCount) {
        //match seletor
        bool selectorMatched = false;
        if (spSelector) {
            std::lock_guard<decltype(m_lock)> lock(m_lock);
            auto findRes = m_messageProperties.find(i);
            if (findRes != m_messageProperties.end() && spSelector->match(findRes->second->getMap())) {
                selectorMatched = true;
            }
        }
        if (spSelector && !selectorMatched)
            continue;

        if (!providerID.empty() || !msgKeyWord.empty()) {
            auto dataRes = getRecordData(i);
            std::string& messageBody = std::get<0>(dataRes);
            std::string& outProviderID = std::get<1>(dataRes);

            if ((providerID.empty() || providerID == outProviderID)
                && (msgKeyWord.empty() || messageBody.find(msgKeyWord) != std::string::npos)
                )
            {
                result.push_back(i);
            }
        }
        else{
            result.push_back(i);
        }
        if (matchingCount % 1000 == 0) {
            auto costMs = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - startTimePoint).count();
            if (costMs >= timeoutMs && i != endSeq) {
                searchCompleted = false;
            }
        }
    }
    destroyQueryContext();
    return result;
}

template <class LockType>
VariableRecordStoreBase<LockType>::VariableRecordStoreBase(SessionID sessionID, const StoreOptions& options, const std::string& storeName, UnCompleteGroupMessageManagerPtr &ucgmmp)
    :m_storeName(storeName)
    ,m_referenceTime(0)
     ,m_baseSequence(options.baseSequence)
     ,m_beginSequence(m_baseSequence)
     , m_recordCount(0)
     , m_sessionID(sessionID)
     , m_unCompleteGroupMessage(ucgmmp)
     {}

// 级联topic模式，可能整组重发，因此要对组消息去重
// // 对于已经完成的组消息，订阅的时候, 足够去重, 对于还没有完成的组消息，可能会全部重发，针对这个情况去重
template <class LockType>
bool VariableRecordStoreBase<LockType>::testGroupMessage(VariantMap *messageProperties) const
{
    if (nullptr == messageProperties || messageProperties->empty()) return true;

    if (nullptr == messageProperties->getVariantProperty(CommonConstant::INNER_GROUP_ID)) return true;
    if (nullptr == messageProperties->getVariantProperty(CommonConstant::INNER_GROUP_SEQ)) return true;

    auto groupID = uuidFun::propGetGroupMessageID(messageProperties);
    auto groupSeq = uuidFun::propGetGroupMessageSequence(messageProperties);
 
    if (m_groupMessage.find(groupID) != m_groupMessage.end())
    {
        return false;
    }

    if (!m_unCompleteGroupMessage->testMessage(groupID, groupSeq)) return false;
    
    return true; 
}

template <class LockType>
void VariableRecordStoreBase<LockType>::testAndAddGroupInfo(uint64_t seq, VariantMap *messageProperties)
{
    if (nullptr == messageProperties || messageProperties->empty()) return ;

    if (nullptr == messageProperties->getVariantProperty(CommonConstant::INNER_GROUP_ID)) return;
    if (nullptr == messageProperties->getVariantProperty(CommonConstant::INNER_GROUP_SEQ)) return;

    auto groupID = uuidFun::propGetGroupMessageID(messageProperties);
    auto groupSeq = uuidFun::propGetGroupMessageSequence(messageProperties);

    if (m_groupMessage.find(groupID) != m_groupMessage.end())
    {
        return ;
    }

    bool isEnd = false;
    messageProperties->getBooleanProperty(CommonConstant::INNER_GROUP_ISEND, isEnd);
        
    if (CommonConstant::BASE_GROUP_SEQUENCE == groupSeq)
    // if (1 == groupSeq)
    { // 
        auto groupMessagePtr = m_unCompleteGroupMessage->addGroupMessage(groupID, MessageID(m_sessionID, seq), isEnd);
        if (nullptr != groupMessagePtr)
        {
            m_groupMessages.emplace(seq, groupMessagePtr);
            if (isEnd)
            { // group only has one message.
                m_groupEndMessages.emplace(seq, groupMessagePtr);
                m_groupMessage.insert(groupID);
            }
        }

    }
    else
    {
        auto groupMessagePtr = m_unCompleteGroupMessage->appendGroupMessage(groupID, groupSeq, MessageID(m_sessionID, seq), isEnd);
        if (nullptr != groupMessagePtr)
        {
            m_groupMessages.emplace(seq, groupMessagePtr);
            if (isEnd)
            { // group only has one message.
                m_groupEndMessages.emplace(seq, groupMessagePtr);
                m_groupMessage.insert(groupID);
            }
        }
    }
}

template <class LockType>
uint8_t VariableRecordStoreBase<LockType>::addMessageAttribute(uint64_t seq,
    MessageAttributeType msgAttrMask,
    VariantMap *messageProperties
    ){
    MessageAttributeType attribute = DEFAULT_MESSAGE_ATTRIBUTE;
    if (msgAttrMask & EXPIRED_MESSAGE_MASKER) {
        attribute |= EXPIRED_MESSAGE_MASKER;
        m_expiredSequenceSet.insert(seq);
    }
    if (msgAttrMask & REMOTE_MESSAGE_MASKER) {
        attribute |= REMOTE_MESSAGE_MASKER;
        m_remoteSequenceSet.insert(seq);
    }

    if (msgAttrMask & DELETED_MESSAGE_MASKER) {
        attribute |= DELETED_MESSAGE_MASKER;
        m_deletedSequenceSet.insert(seq);
    }

	auto transState = getMessageTransactionState(msgAttrMask);
	if (transState) {
		setMessageTransactionState(attribute, transState);
		m_transactionSequences.emplace(seq, transState);
	}

    if (messageProperties && !messageProperties->empty()) {
        attribute |= HAS_ATTRIBUTE_MESSAGE_MASKER;
        //this->addToInfoFile(*messageProperties);
        auto m = std::make_shared<VariantMap>();
        m->swap(*messageProperties);
        m_messageProperties[seq] = m;
        m_lastHasPropertiesMessageSeq = seq;

        testAndAddGroupInfo(seq, m.get());
    }

    if (!m_notDeleteSequenceSet.empty()) {
        auto noDeleteSeqRes = m_notDeleteSequenceSet.find(seq);
        if (noDeleteSeqRes != m_notDeleteSequenceSet.end()) {
            attribute |= DELETED_MESSAGE_MASKER;
            m_notDeleteSequenceSet.erase(noDeleteSeqRes);
            m_deletedSequenceSet.insert(seq);
        }
    }
    return attribute;
}



template <class LockType>
uint64_t VariableRecordStoreBase<LockType>::getOldestDeletedSequence() const
{
    std::lock_guard<LockType> lock(m_lock);
    uint64_t oldestSeq = ULLONG_MAX;
    if (!m_deletedSequenceSet.empty()) {
        oldestSeq = *m_deletedSequenceSet.begin();
    }
    return oldestSeq;
}

template <class LockType>
uint64_t VariableRecordStoreBase<LockType>::getLastHasProperitesMessageSequence() const
{
    std::lock_guard<LockType> lock(m_lock);
    return m_lastHasPropertiesMessageSeq;
}

template <class LockType>
bool VariableRecordStoreBase<LockType>::isDeleted(uint64_t seq) const{
    std::lock_guard<LockType> lock(m_lock);
    return m_deletedSequenceSet.find(seq) != m_deletedSequenceSet.end();
}

template <class LockType>
uint8_t VariableRecordStoreBase<LockType>::getTransState(uint64_t seq) const {
	std::lock_guard<LockType> lock(m_lock);
	auto iter = m_transactionSequences.find(seq);
	if (iter == m_transactionSequences.end())
		return MESSAGE_TRANSACTION_NA;
	return iter->second;
}


template <class LockType>
bool VariableRecordStoreBase<LockType>::isDeletedOrRemoteMessage(uint64_t seq) const{
    std::lock_guard<LockType> lock(m_lock);
    return m_deletedSequenceSet.find(seq) != m_deletedSequenceSet.end()
        || m_remoteSequenceSet.find(seq) != m_remoteSequenceSet.end();
}

template <class LockType>

const VariantMap* VariableRecordStoreBase<LockType>::getMessageProperties(uint64_t sequence) const {

    std::lock_guard<decltype(m_lock)> lock(m_lock);
    const uint64_t validStart = m_baseSequence;
    const uint64_t validEnd = m_baseSequence + m_recordCount - 1;
    if (sequence < validStart || sequence > validEnd) {
        throw dep::DEPException{ "Requested sequence [" + std::to_string(sequence) + "] is invalid, valid Seq[" + std::to_string(validStart) + "] to [" + std::to_string(validEnd) + "]" };
    }
    auto findRes = m_messageProperties.find(sequence);
    if (findRes == m_messageProperties.end()) {
        return nullptr;
    }
    return findRes->second.get();
}

template <class LockType>
uint64_t VariableRecordStoreBase<LockType>::getRecordCount() {
    return m_recordCount;
}

template<class LockType>
void VariableRecordStoreBase<LockType>::setNewBeginSequence(uint64_t newBeginSequence)
{
}

template <class LockType>
void VariableRecordStoreBase<LockType>::markDeleted(uint64_t start, uint64_t end){
    std::lock_guard<LockType> lock(m_lock);
    const uint64_t validEnd = m_baseSequence + m_recordCount - 1;
    if (start > validEnd) {
        for (uint64_t i = start; i <= end; ++i) {
            m_notDeleteSequenceSet.insert(i);
        }
    }
    else {
        if (end > validEnd) {
            for (uint64_t i = validEnd + 1; i <= end; ++i) {
                m_notDeleteSequenceSet.insert(i);
            }
            end = validEnd;
        }
        markAttribute(start, end, DELETED_MESSAGE_MASKER);
        for (uint64_t i = start; i <= end; ++i) {
            m_deletedSequenceSet.insert(i);
        }
    }
}



template <class LockType>
std::vector<uint64_t> VariableRecordStoreBase<LockType>::getNotDeletedSequences() const {
    std::lock_guard<LockType> lock(m_lock);
    std::vector<uint64_t> res;
    if (m_notDeleteSequenceSet.empty()) {
        return res;
    }

    for (auto i = m_notDeleteSequenceSet.begin(); i != m_notDeleteSequenceSet.end(); ++i) {
        res.push_back(*i);
    }
    return res;
}


template <class LockType>
std::vector<uint64_t> VariableRecordStoreBase<LockType>::getDeletedSequences(uint64_t start, uint64_t end)  const {
    std::lock_guard<LockType> lock(m_lock);
    std::vector<uint64_t> res;
    if (m_deletedSequenceSet.empty() || start > end) {
        return res;
    }
    const uint64_t validStart = m_baseSequence;
    const uint64_t validEnd = m_baseSequence + m_recordCount - 1;
    if (start < validStart || start > validEnd)
        throw dep::DEPException{ "Requested start sequence [" + std::to_string(start) + "] is invalid, valid Seq[" + std::to_string(validStart) + "] to [" + std::to_string(validEnd) + "]" };
    if (end < validStart || end > validEnd)
        throw dep::DEPException{ "Requested end sequence [" + std::to_string(end) + "] is invalid, valid Seq[" + std::to_string(validStart) + "] to [" + std::to_string(validEnd) + "]" };
//     if (start > end)
//         throw dep::DEPException{ "End sequence [" + std::to_string(end) + "] is greater than start sequence [" + std::to_string(start) + "]" };

    for (auto i = std::max(start, m_beginSequence); i <= end; ++i) {
        if (m_deletedSequenceSet.find(i) != m_deletedSequenceSet.end()) {
            res.push_back(i);
        }
    }
    return res;
}

template <class LockType>
std::vector<uint64_t> VariableRecordStoreBase<LockType>::getExpiredSequences(uint64_t start, uint64_t end)  const {
    std::lock_guard<LockType> lock(m_lock);
    std::vector<uint64_t> res;
    if (m_expiredSequenceSet.empty() || start > end) {
        return res;
    }
    const uint64_t validStart = m_baseSequence;
    const uint64_t validEnd = m_baseSequence + m_recordCount - 1;
    if (start < validStart || start > validEnd)
        throw dep::DEPException{ "Requested start sequence [" + std::to_string(start) + "] is invalid, valid Seq[" + std::to_string(validStart) + "] to [" + std::to_string(validEnd) + "]" };
    if (end < validStart || end > validEnd)
        throw dep::DEPException{ "Requested end sequence [" + std::to_string(end) + "] is invalid, valid Seq[" + std::to_string(validStart) + "] to [" + std::to_string(validEnd) + "]" };

    for (auto i = std::max(start, m_beginSequence); i <= end; ++i) {
        if (m_expiredSequenceSet.find(i) != m_expiredSequenceSet.end()) {
            res.push_back(i);
        }
    }
    return res;
}

template <class LockType>
bool VariableRecordStoreBase<LockType>::markExpired(uint64_t start, uint64_t end){
    std::lock_guard<LockType> lock(m_lock);
    const uint64_t validEnd = m_baseSequence + m_recordCount - 1;
    if (start > validEnd) {
        for (uint64_t i = start; i <= end; ++i) {
            // TODO m_notExpiredSequenceSet.insert(i);
        }
    }
    else {
        if (end > validEnd) {
            for (uint64_t i = validEnd + 1; i <= end; ++i) {
            // TODO m_notExpiredSequenceSet.insert(i);
            }
            end = validEnd;
        }
        markAttribute(start, end, EXPIRED_MESSAGE_MASKER);
        for (uint64_t i = start; i <= end; ++i) {
            m_expiredSequenceSet.insert(i);
        }
    }
    return true;
}

template <class LockType>
std::vector<uint64_t> VariableRecordStoreBase<LockType>::getRemoteSequences(uint64_t start, uint64_t end)  const {
    std::lock_guard<LockType> lock(m_lock);
    std::vector<uint64_t> res;
    if (m_remoteSequenceSet.empty()) {
        return res;
    }
    const uint64_t validStart = m_baseSequence;
    const uint64_t validEnd = m_baseSequence + m_recordCount - 1;
    if (start < validStart || start > validEnd)
        throw dep::DEPException{ "Requested start sequence [" + std::to_string(start) + "] is invalid, valid Seq[" + std::to_string(validStart) + "] to [" + std::to_string(validEnd) + "]" };
    if (end < validStart || end > validEnd)
        throw dep::DEPException{ "Requested end sequence [" + std::to_string(end) + "] is invalid, valid Seq[" + std::to_string(validStart) + "] to [" + std::to_string(validEnd) + "]" };
    if (start > end)
        throw dep::DEPException{ "End sequence [" + std::to_string(end) + "] is greater than start sequence [" + std::to_string(start) + "]" };

    for (auto i = std::max(start, m_beginSequence); i <= end; ++i) {
        if (m_remoteSequenceSet.find(i) != m_remoteSequenceSet.end()) {
            res.push_back(i);
        }
    }
    return res;
}

template <class LockType>
std::vector<uint64_t> VariableRecordStoreBase<LockType>::getRemoteOrDeletedSequences(uint64_t start, uint64_t end) const {
    std::lock_guard<LockType> lock(m_lock);
    std::vector<uint64_t> res;
    if (m_remoteSequenceSet.empty() && m_deletedSequenceSet.empty()) {
        return res;
    }
    const uint64_t validStart = m_baseSequence;
    const uint64_t validEnd = m_baseSequence + m_recordCount - 1;
    if (start < validStart || start > validEnd)
        throw dep::DEPException{ "Requested start sequence [" + std::to_string(start) + "] is invalid, valid Seq[" + std::to_string(validStart) + "] to [" + std::to_string(validEnd) + "]" };
    if (end < validStart || end > validEnd)
        throw dep::DEPException{ "Requested end sequence [" + std::to_string(end) + "] is invalid, valid Seq[" + std::to_string(validStart) + "] to [" + std::to_string(validEnd) + "]" };
    if (start > end)
        throw dep::DEPException{ "End sequence [" + std::to_string(end) + "] is greater than start sequence [" + std::to_string(start) + "]" };

    for (auto i = std::max(start, m_beginSequence); i <= end; ++i) {
        if (m_remoteSequenceSet.find(i) != m_remoteSequenceSet.end() || m_deletedSequenceSet.find(i) != m_deletedSequenceSet.end()) {
            res.push_back(i);
        }
    }
    return res;
}


template <class LockType>
void VariableRecordStoreBase<LockType>::setSequenceReady(uint64_t seq) {
    std::lock_guard<LockType> lock(m_lock);
    uint64_t adjustedSeq = seq - m_baseSequence;
    //printf("setMark seq %u , mark %u\n", adjustedSeq, mark);
    m_notReadySequenceSet.erase(adjustedSeq);
}

template <class LockType>
void VariableRecordStoreBase<LockType>::setSequenceNotReady(uint64_t seq) {
    std::lock_guard<LockType> lock(m_lock);
    uint64_t adjustedSeq = seq - m_baseSequence;
    //printf("setMark seq %u , mark %u\n", adjustedSeq, mark);
    m_notReadySequenceSet.insert(adjustedSeq);
}

template <class LockType>
void VariableRecordStoreBase<LockType>::setAllSequenceReady() {
    m_notReadySequenceSet.clear();
}


template <class LockType>
std::tuple<uint64_t, uint64_t> VariableRecordStoreBase<LockType>::getLastLocalMessageSequence() const{
    std::lock_guard<LockType> lock(m_lock);
    uint64_t localMsgCount = m_recordCount - m_remoteSequenceSet.size();
    auto lastSeq = m_recordCount + m_baseSequence -1;
    while (lastSeq >= m_beginSequence) {
        if (m_remoteSequenceSet.find(lastSeq) == m_remoteSequenceSet.end()) {
            break;
        }
        if (lastSeq == m_beginSequence) {
            break;
        }
        else {
            --lastSeq;
        }
    }
    return std::make_tuple(localMsgCount, lastSeq);
}

template<class LockType>
inline uint64_t VariableRecordStoreBase<LockType>::getBeginSequence() const
{
	return m_beginSequence;
}

template <class LockType>
LockType &VariableRecordStoreBase<LockType>::getLock() {
    return m_lock;
}

template <class LockType>
bool VariableRecordStoreBase<LockType>::getIsSequenceReady(uint64_t seq) {
    return (m_notReadySequenceSet.find(seq) ==m_notReadySequenceSet.end());
}


// template <class K, class V>
// inline typename std::map<K,V>::iterator testMapRange(std::map<K,V> &m, K &b, K &e)
// {
//     auto it = m.lower_bound(b);
//     if (it == m.end() || it->first > e) return m.end();
// 
//     return it; 
// }

template <class K, class V>
inline typename std::map<K,V>::const_iterator testMapRange(const std::map<K,V> &m, const K &b, const K &e)
{
    auto it = m.lower_bound(b);
    if (it == m.end() || it->first > e) return m.end();

    return it; 
}

template <class LockType>
GroupMessage* VariableRecordStoreBase<LockType>::getGroupMessagePtr(const MessageID &messageID) const
{
    std::lock_guard<decltype(m_lock)> lock(m_lock);

    auto it = m_groupMessages.find(messageID.sequence);
    if (m_groupMessages.end() == it) return nullptr;

    return it->second.get();
}

template <class LockType>
bool VariableRecordStoreBase<LockType>::checkGroupMessage(const MessageID &begin, uint64_t seq, IVariableRecordStore::ChkGroupResult *firstGroupMessage) const
{
    std::lock_guard<decltype(m_lock)> lock(m_lock);
    bool ret = false;

    auto groupIt = testMapRange(m_groupMessages, begin.sequence, seq);
    if (groupIt != m_groupMessages.end())
    {
        if (nullptr != firstGroupMessage)
        {
            firstGroupMessage->messageID.sessionID = begin.sessionID;
            firstGroupMessage->messageID.sequence = groupIt->first;
            firstGroupMessage->isEndGroupMessage = (groupIt->second->endMessageID() == firstGroupMessage->messageID);
        }
        ret = true;
    }

    return ret;
}


// 从默认级别中拿出来放到对应的级别去,外部需要顺序调用
template <class LockType>
bool VariableRecordStoreBase<LockType>::moveRecordLevel(uint64_t seq, uint8_t level)
{
    if (level > CommonConstant::MAX_MESSAGE_LEVEL) return false;

    if (m_defaultLevelRecord.empty()) return false;

    auto rit = m_defaultLevelRecord.rbegin();
    if (rit->first > seq || rit->second < seq) return false; // seq必须在最后一个段中

    if (seq == rit->first)
    {
        if (seq < rit->second) 
        { // 最后一段有多个记录
            m_defaultLevelRecord.emplace(seq + 1, rit->second);
        }
        m_defaultLevelRecord.erase(rit->first);
    }
    else if (seq == rit->second)
    {
        rit->second--; 
    }
    else
    {
        auto tmpEnd = rit->second;  
        rit->second = seq-1;

        m_defaultLevelRecord.emplace(seq+1, tmpEnd);
    }

    m_levelRecords[LEVEL_IDX(level)].push_back(seq);
    m_recordPriority.emplace(seq, level);

    return true;
}

// 这个函数只能追加
template <class LockType>
void VariableRecordStoreBase<LockType>::addLevelInfo(uint64_t seq, uint8_t level)
{
    if (CommonConstant::DEFAULT_MESSAGE_LEVEL != level)
    {
        m_recordPriority.emplace(seq, level);
        m_levelRecords[LEVEL_IDX(level)].push_back(seq);
    }
    else 
    {
        if (m_defaultLevelRecord.empty())
        {
            m_defaultLevelRecord.emplace(seq, seq);
            return ;
        }

        auto rit = m_defaultLevelRecord.rbegin();
        if (rit->second + 1 == seq) rit->second++;
        else m_defaultLevelRecord.emplace(seq, seq);
    }
}

// range start, range end, target range start, target range end;
#define rangeInclude(rs, re, trs, tre) (rs <= trs && re >= tre)

// 全部是默认级别(default level)，返回true.
// 否则，把第一条非默认级别的消息号设置到first上
template <class LockType>
bool VariableRecordStoreBase<LockType>::checkDefaultLevel(uint64_t startSeq, uint64_t endSeq, IVariableRecordStore::ChkLevelResult *result) const
{
    auto it = m_recordPriority.lower_bound(startSeq);
    if (m_recordPriority.end() == it) return true;
    if (it->first > endSeq) return true;

    if (nullptr != result)
    {
        result->sequence = it->first;
        result->level = it->second;
    }

    return false;
}

template <class LockType>
uint8_t VariableRecordStoreBase<LockType>::getRecordLevel(uint64_t sequence) const
{
    auto it = m_recordPriority.find(sequence);
    if (m_recordPriority.end() == it) return CommonConstant::DEFAULT_MESSAGE_LEVEL;
    return it->second;
}

template <class LockType>
bool VariableRecordStoreBase<LockType>::getLevelBrief(uint8_t lev, uint64_t *count, uint64_t *firstSeq, uint64_t *lastSeq) const
{
    if (lev > CommonConstant::MAX_MESSAGE_LEVEL) return false;
    if (CommonConstant::DEFAULT_MESSAGE_LEVEL == lev)
        return getDefaultLevelBrief(count, firstSeq, lastSeq);

    auto levVec = m_levelRecords[LEVEL_IDX(lev)];
    if (levVec.empty()) return false;
    if (nullptr != count) { *count = levVec.size(); }
    if (nullptr != firstSeq) { *firstSeq = levVec[0]; }
    if (nullptr != lastSeq) { *lastSeq = levVec.back(); }
    
    return true;
}

template <class LockType>
uint64_t VariableRecordStoreBase<LockType>::getFirstNoAckByLevel(uint8_t lev, uint64_t cursor) const
{
    if (lev > CommonConstant::MAX_MESSAGE_LEVEL) return false;
    if (lev != CommonConstant::DEFAULT_MESSAGE_LEVEL)
    {
        auto &records = m_levelRecords[LEVEL_IDX(lev)];
        auto it = std::upper_bound(records.begin(), records.end(), cursor);
        if (it == records.end()) return 0;
        return *it;
    }
    else
    {
        if (m_defaultLevelRecord.empty()) return 0;
        auto it = m_defaultLevelRecord.lower_bound(cursor);

        // 前边
        if (it != m_defaultLevelRecord.begin())
        {
            auto pit = it; pit--;
            if (pit->second > cursor) return cursor + 1; // tip: pit->first < cursor;
        }

        // it; 
        if (it != m_defaultLevelRecord.end())
        {
            if (it->first > cursor) return it->first; // tip: it->first >= cursor;
            if (it->second > cursor) return cursor + 1; // it->first == cursor;
        }

        // next
        if (it != m_defaultLevelRecord.end() && ++it != m_defaultLevelRecord.end())
        {
            return it->first;
        }

        return 0;
    }


}

template <class LockType>
bool VariableRecordStoreBase<LockType>::getDefaultLevelBrief(uint64_t *count, uint64_t *firstSeq, uint64_t *lastSeq) const
{
    if (m_defaultLevelRecord.empty()) return false;

    if (nullptr != count)
    {
        *count = 0;
        for (auto it = m_defaultLevelRecord.begin(); it != m_defaultLevelRecord.end(); ++it ) { *count += (it->second - it->first + 1); }
    }

    if (nullptr != firstSeq) { *firstSeq = m_defaultLevelRecord.begin()->first; }
    if (nullptr != lastSeq) { *lastSeq = m_defaultLevelRecord.rbegin()->second; }

    return true;
}

template <class LockType>
void VariableRecordStoreBase<LockType>::fillDefaultLevelInfo(uint64_t recordCount)
{
    m_defaultLevelRecord.emplace(1, recordCount);
}

template <class LockType>
void VariableRecordStoreBase<LockType>::initRecordLevel(uint64_t seq, uint8_t level)
{
    if (level == CommonConstant::DEFAULT_MESSAGE_LEVEL|| 
            level > util::CommonConstant::MAX_MESSAGE_LEVEL) return ;
    moveRecordLevel(seq, level);
}


template <class LockType>
void VariableRecordStoreBase<LockType>::checkLevelInfo(uint64_t seq, uint8_t level)
{
    if (level > util::CommonConstant::MAX_MESSAGE_LEVEL) return ;

    addLevelInfo(seq, level);
}

}} // namespace lightmq { namespace util {

#endif /* LIGHTMQ_LIGHTMQUTIL_INC_BASELIGHTMQVARIABLERECORDSTORE_H_ */
