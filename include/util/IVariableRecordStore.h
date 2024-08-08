////////////////////////////////////////////////////////////////////////////////
//
// IVariableRecordStore.h
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

#ifndef ILIGHTMQVARIABLERECORDSTORE_H_
#define ILIGHTMQVARIABLERECORDSTORE_H_

#ifndef _NDEBUG
//#define _DEBUG_LOG
#endif

#ifdef _DEBUG_LOG
#include <iostream>
#endif

#include <string>
#include <chrono>
#include <tuple>
#include <mutex>
#include <functional>
#include <memory>
#include <unordered_map>
#include <climits>
#include <map>
#include <vector>
#include <chrono>


#include "ByteBuffer.h"
#include "NoLock.h"
#include "DEPException.h"
#include "ISQLExpression.h"
#include "Message.h"
#include "CommonConstant.h"


//namespace lightmq { namespace protocol { struct MessageSerializeAgent; }}

namespace lightmq { namespace util {
class VariantMap;
class GroupMessage;

class DEPStorageException : public dep::DEPException {
public:
    explicit DEPStorageException(const std::string& message) noexcept
        : dep::DEPException(message)
    {}
};


struct RecordStoreAddMessageParam {
	protocol::MessageSerializeAgent *m_msgAgent { nullptr };
	dep::ByteBuffer *m_buffer { nullptr };
	bool m_isRemoteMessage {false};
	bool m_isDeletedMessage {false};
    // lwq add expired messsage
    bool m_isExpiredMessage {false};
	std::chrono::system_clock::time_point m_timestamp; // = std::chrono::high_resolution_clock::now()) = 0;


	//RecordStoreAddMessageParam() {
	//}

	RecordStoreAddMessageParam()
	{}

	RecordStoreAddMessageParam(dep::ByteBuffer *buffer, protocol::MessageSerializeAgent *msgAgent = nullptr)
		: m_msgAgent(msgAgent), m_buffer(buffer)
	{}


	void makeTimeStamp() {
		m_timestamp  = std::chrono::system_clock::now();
	}
    void setTimeStamp(std::chrono::system_clock::time_point timePoint) {
        m_timestamp = timePoint;
    }
    uint8_t getPriority()
    {
        if (nullptr == m_msgAgent)
        {
            return util::CommonConstant::DEFAULT_MESSAGE_LEVEL;
        }
        return m_msgAgent->m_header.getPriority();
    }
};

class IVariableRecordStore {
public:

struct ChkGroupResult
{
    MessageID messageID;
    bool isEndGroupMessage;
};
       
struct ChkLevelResult
{
    uint64_t sequence;
    int level;
};



    /**
     * Constructor
     *
     * @param storePath  Full path to the record store
     * @param storeName  Name of the record store
     * @param options    Create options
     */
    IVariableRecordStore() = default;

    /**
     * Destructor
     */
    virtual ~IVariableRecordStore() = default;

    /**
     * Get consecutive records starting from RRN "start" up to buffer capacity, obtained
     * records are without data prefix and suffix.
     *
     * Data positions are indicated by indices.
     *
     * @param start    Start RRN
     * @param buffer   Output buffer
     * @param indices  Data indices
     */
    virtual void getMany(uint64_t start, dep::ByteBuffer& buffer, std::vector<uint32_t>& indices) = 0;
    /**
     * Get consecutive records starting from RRN "start" up to buffer capacity or RRN "end",
     * obtained records are without data prefix and suffix.
     *
     * Data positions are indicated by indices.
     *
     * @param start    Start RRN
     * @param end      End RRN
     * @param buffer   Output buffer
     * @param indices  Data indices
     */
    virtual void getMany(uint64_t start, uint64_t end, dep::ByteBuffer& buffer, std::vector<uint32_t>& indices) = 0;
    virtual uint32_t getMany(uint64_t start, uint64_t end, dep::ByteBuffer& buffer, std::pair<uint64_t, uint64_t> &startEndPosition, uint64_t& expiredCount) = 0;
    /**
     * Get consecutive records starting from RRN "start" up to buffer capacity, obtained
     * records are with data prefix and suffix. Prefix and suffix are constructed according
     * to OutputDataFormat.
     *
     * Normally should not be used to obtain plain data format (without prefix/suffix)
     * because record boundaries cannot be identified.
     *
     * @tparam OutputDataFormat  Output data format
     * @param start              Start RRN
     * @param buffer             Output buffer
     * @return
     */
    virtual uint32_t getManyWithPrefixSuffixProxy(uint64_t start, dep::ByteBuffer& buffer)  = 0;

    virtual uint32_t getManyWithPrefixSuffixProxy(uint64_t start, uint64_t end, dep::ByteBuffer& buffer) = 0;


    /**
     * A first consecutive batch of message which are ready
     * (OutputDataFormat = InputDataFormat) so no template is defined
     * @param start
     * @param end
     * @param buffer
     * @return number of message read
     */
    virtual uint32_t getManyReady(uint64_t start, uint64_t end, dep::ByteBuffer& buffer, std::pair<uint64_t, uint64_t> &startEndPosition, uint64_t& expiredCount) = 0;

    /**
     * Get record by RRN.
     *
     * @param seq     Relative record number
     * @param buffer  Output buffer
     */
    virtual void readSingleMessageBuffer(uint64_t seq, dep::ByteBuffer& buffer) = 0;

    /**
     * Get record size by RRN.
     *
     * @param seq     Relative record number
     * @return        length of the record
     */
    virtual uint32_t getRecordSizeWithPrefixSuffix(uint64_t seq) = 0;

    /**
     * Add record
     *
     * @param buffer     Input buffer
     * @param isRemoteMessage     is message from remote broker
     * @param messageProperties  the properties of the message
     * @param timestamp  Record timestamp
     */
    virtual  uint64_t add(RecordStoreAddMessageParam &addParam) = 0;


    /**
     * Add date
     *
     * @param buffer     Input buffer
     */
    virtual std::tuple<uint32_t, uint64_t> addToDataFile(dep::ByteBuffer& buffer) = 0;

    // virtual uint64_t addToInfoFile(const std::unordered_map<std::string, std::string>& attributes) = 0;
    /**
    * mark the message from start to end as deleted
    * @param uint64_t start
    * @param uint64_t end
    */
    virtual void markDeleted(uint64_t start, uint64_t end) = 0;

    virtual bool markExpired(uint64_t start, uint64_t end) = 0;

    virtual bool isDeleted(uint64_t seq) const = 0;

    virtual bool isDeletedOrRemoteMessage(uint64_t seq) const = 0;

    virtual std::vector<uint64_t> getDeletedSequences(uint64_t start, uint64_t end) const = 0;

    virtual std::vector<uint64_t> getExpiredSequences(uint64_t start, uint64_t end) const = 0;

    virtual std::vector<uint64_t> getRemoteSequences(uint64_t start, uint64_t end) const = 0;

    virtual std::vector<uint64_t> getRemoteOrDeletedSequences(uint64_t start, uint64_t end) const = 0;

    virtual std::vector<uint64_t> getNotDeletedSequences() const = 0;

    virtual uint64_t getLastHasProperitesMessageSequence() const = 0;

    virtual uint64_t getOldestDeletedSequence() const = 0;

    virtual const VariantMap* getMessageProperties(uint64_t sequence) const = 0;
    /**
     * Get record count
     */
    virtual uint64_t getRecordCount() = 0;

    /**
     * Synchronize kernel buffer with storage device
     */
    virtual void sync() = 0;

    virtual void setSequenceReady(uint64_t seq) = 0;

    virtual void setSequenceNotReady(uint64_t seq) = 0;

    virtual void setAllSequenceReady() = 0;

    virtual std::tuple<uint64_t, uint64_t> getLastLocalMessageSequence() const = 0;


	virtual uint64_t getBeginSequence() const = 0;

	virtual void setNewBeginSequence(uint64_t newBeginSequence) = 0;

//     virtual uint8_t addMessageAttribute(bool isRemoteMessage,
//         VariantMap *messageProperties,
//         bool isDeletedMessage,
//         uint64_t seq) = 0;

    virtual std::vector<uint64_t> queryMessageIDs(const std::string& providerID, std::shared_ptr<ISQLExpression> spSelector, const std::string& msgKeyWord, uint64_t startTime, uint64_t endTime, uint64_t timeoutMs, bool& searchCompleted) = 0;

    virtual uint8_t getTransState(uint64_t seq) const = 0;
    virtual GroupMessage* getGroupMessagePtr(const MessageID &messageID) const = 0;
    virtual bool checkGroupMessage(const MessageID &begin, uint64_t seq, ChkGroupResult *first) const = 0;

    virtual bool checkDefaultLevel(uint64_t startSeq, uint64_t endSeq, ChkLevelResult *result) const = 0;
    virtual uint8_t getRecordLevel(uint64_t sequence) const = 0;
    virtual bool getLevelBrief(uint8_t lev, uint64_t *count, uint64_t *firstSeq, uint64_t *lastSeq) const = 0;
    virtual uint64_t getFirstNoAckByLevel(uint8_t lev, uint64_t cursor) const = 0;
};

// modify ghli 22/10/27-15:37:01
// 原始接口不需要模板
template <class LockType = dep::NoLock>
class IVariableRecordStoreLocked : public IVariableRecordStore {
public:
    virtual LockType &getLock() = 0;
};

}} // namespace lightmq { namespace util {

#endif /* LIGHTMQVARIABLERECORDSTORE_H_ */
