////////////////////////////////////////////////////////////////////////////////
//
// VariableRecordStoreMemory.h
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

#ifndef LIGHTMQ_LIGHTMQUTIL_INC_LIGHTMQVARIABLERECORDMEMORYSTORE_H_
#define LIGHTMQ_LIGHTMQUTIL_INC_LIGHTMQVARIABLERECORDMEMORYSTORE_H_

#include "VariableRecordStoreBase.h"

#include <chrono>
#include <cstdint>
#include <memory>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>
#include <deque>
#include <functional>
#include <algorithm>



#include "ByteBuffer.h"
#include "CachedFileStream.h"
#include "CircularRawBuffer.h"
#include "FsyncFileStream.h"
#include "MemoryBackport.h"
#include "ISeverityLog.h"
#include "MessageUtil.h"
#include "LightmqMessageHead.h"
#include "LogWrapper.h"



namespace lightmq { namespace util {

// class dep::ISeverityLog;

template <class LockType = dep::NoLock>
class VariableRecordStoreMemory : public VariableRecordStoreBase<LockType> {
protected:
    using typename VariableRecordStoreBase<LockType>::DataStartIndexType;
    using typename VariableRecordStoreBase<LockType>::DataLengthType;
    using typename VariableRecordStoreBase<LockType>::TimestampType;
    using typename VariableRecordStoreBase<LockType>::MessageAttributeType;
    using typename VariableRecordStoreBase<LockType>::MessageMarkType;
    using typename VariableRecordStoreBase<LockType>::ReservedByteType;
    using typename VariableRecordStoreBase<LockType>::AtrributeStringLengthType;
    using typename VariableRecordStoreBase<LockType>::StoreOptions;


    using VariableRecordStoreBase<LockType>::m_lock;
    using VariableRecordStoreBase<LockType>::m_deletedSequenceSet;
    using VariableRecordStoreBase<LockType>::m_remoteSequenceSet;
    using VariableRecordStoreBase<LockType>::m_notDeleteSequenceSet;
    // using VariableRecordStoreBase<LockType>::m_messagePropertiesLock;
    using VariableRecordStoreBase<LockType>::m_messageProperties;
    using VariableRecordStoreBase<LockType>::m_lastHasPropertiesMessageSeq;

    using VariableRecordStoreBase<LockType>::m_storeName;
    using VariableRecordStoreBase<LockType>::m_referenceTime;
    using VariableRecordStoreBase<LockType>::m_baseSequence;       /**< index of the base record */
    using VariableRecordStoreBase<LockType>::m_beginSequence;       /**< index of the base record */
    using VariableRecordStoreBase<LockType>::m_recordCount;

    using VariableRecordStoreBase<LockType>::DEFAULT_MESSAGE_ATTRIBUTE;
    using VariableRecordStoreBase<LockType>::DEFAULT_MESSAGE_MARK;
    using VariableRecordStoreBase<LockType>::HAS_ATTRIBUTE_MESSAGE_MASKER;
    using VariableRecordStoreBase<LockType>::DELETED_MESSAGE_MASKER;
    using VariableRecordStoreBase<LockType>::REMOTE_MESSAGE_MASKER;
    using VariableRecordStoreBase<LockType>::DEFAULT_MAX_FILE_SIZE;


public:

    struct MemoryIndexInfo{
        DataStartIndexType dataStart;//msg beging offset
        DataLengthType length;
        TimestampType timestamp;
        MessageAttributeType messageAttribute;
        MessageMarkType messageMark;
    };
public:
    VariableRecordStoreMemory(SessionID sessionID
            , const std::string& storeName
            , UnCompleteGroupMessageManagerPtr &ucgmmp 
            , const typename VariableRecordStoreBase<LockType>::StoreOptions& options
            , std::function<bool(uint64_t& result)>&& function
            , dep::ISeverityLog* pLogger);
    virtual ~VariableRecordStoreMemory() = default;
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
    virtual void getMany(uint64_t start, dep::ByteBuffer& buffer, std::vector<uint32_t>& indices) override;
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
    virtual void getMany(uint64_t start, uint64_t end, dep::ByteBuffer& buffer, std::vector<uint32_t>& indices) override;
    virtual uint32_t getMany(uint64_t start, uint64_t end, dep::ByteBuffer& buffer, std::pair<uint64_t, uint64_t> &startEndPosition, uint64_t& expiredCount) override;

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
    virtual uint32_t getManyWithPrefixSuffixProxy(uint64_t start, dep::ByteBuffer& buffer)  override;

    virtual uint32_t getManyWithPrefixSuffixProxy(uint64_t start, uint64_t end, dep::ByteBuffer& buffer) override;


    /**
     * A first consecutive batch of message which are ready
     * (OutputDataFormat = InputDataFormat) so no template is defined
     * @param start
     * @param end
     * @param buffer
     * @return number of message read
     */
    virtual uint32_t getManyReady(uint64_t start, uint64_t end, dep::ByteBuffer& buffer, std::pair<uint64_t, uint64_t> &startEndPosition, uint64_t & expiredCount) override;

    /**
     * Get record by RRN.
     *
     * @param seq     Relative record number
     * @param buffer  Output buffer
     */
    virtual void readSingleMessageBuffer(uint64_t seq, dep::ByteBuffer& buffer) override;

    /**
     * Get record size by RRN.
     *
     * @param seq     Relative record number
     * @return        length of the record
     */
    virtual uint32_t getRecordSizeWithPrefixSuffix(uint64_t seq) override;

    /**
     * Add record
     *
     * @param buffer     Input buffer
     * @param isRemoteMessage     is message from remote broker
     * @param messageProperties  the properties of the message
     * @param timestamp  Record timestamp
     */
    virtual  uint64_t add(RecordStoreAddMessageParam &addParam) override;


    /**
     * Add date
     *
     * @param buffer     Input buffer
     */
    virtual std::tuple<uint32_t, uint64_t> addToDataFile(dep::ByteBuffer& buffer) override;

    // virtual uint64_t addToInfoFile(const std::unordered_map<std::string, std::string>& attributes) override;

    /**
     * Synchronize kernel buffer with storage device
     */
    virtual void sync() override;
protected:
    void addIndexRecord(DataStartIndexType dataStart, DataLengthType length, const TimestampType& timestamp, uint8_t messageAttribute=DEFAULT_MESSAGE_ATTRIBUTE, uint8_t messageMark=DEFAULT_MESSAGE_MARK);
    void readIndexRecord(uint64_t seq, DataStartIndexType &dataStart, DataLengthType &length);
    
    virtual void markAttribute(uint64_t start, uint64_t end, MessageAttributeType marker) override;
	void setNewBeginSequence(uint64_t newBeginSequence);
private:
    void deleteRecord(uint64_t deleteEnd);
    void extendMemoryBuffer(const uint32_t &intputLength);

private:
    std::unique_ptr<CircularRawBuffer> m_pDataMemory;       // Memory handle to the memory data session store
    std::unique_ptr<std::deque<MemoryIndexInfo>> m_pIndexMemory; // Memory handle to the memory index session store
    std::function<bool(uint64_t& result)> m_getNonAckSeqFunction;
    uint64_t m_maxMemorySize;
    dep::ISeverityLog* m_pLogger;

protected:
    virtual uint64_t getIndexTimeStamp(uint64_t index) override;
    virtual std::tuple<std::string, std::string> getRecordData(uint64_t index) override;

};



template <class LockType /*= dep::NoLock*/>
uint64_t lightmq::util::VariableRecordStoreMemory<LockType>::getIndexTimeStamp(uint64_t index)
{
    auto offset = index - m_beginSequence;
    if (offset < m_pIndexMemory->size()) {
        const MemoryIndexInfo& indexinfo = m_pIndexMemory->at(offset);//-1
        return indexinfo.timestamp;
    }
    return 0;
}

template <class LockType /*= dep::NoLock*/>
std::tuple<std::string, std::string> lightmq::util::VariableRecordStoreMemory<LockType>::getRecordData(uint64_t index)
{
    dep::ByteBuffer dataBuffer(10*1024);
    readSingleMessageBuffer(index, dataBuffer);
    dataBuffer.flip();
    std::string providerID;
    ExtendBmpRemoteBrokerInfo extendRemoteBrokerInfo;
    MessageUtil::parseStoreData(dataBuffer, &extendRemoteBrokerInfo , &providerID, nullptr, nullptr, true);
    LightmqMessageHead msgHead;
    msgHead.parseAll(dataBuffer);
    bool isPlainMsg = std::find_if(msgHead.getProcessFlags().begin(), msgHead.getProcessFlags().end(), [](int val)->bool{return val == LightmqMessageHead::COMPRESS ||val == LightmqMessageHead::ENCRYPT;}) == msgHead.getProcessFlags().end();
    std::string msgContent = isPlainMsg ? std::string((const char*)dataBuffer.getArray() + dataBuffer.getPosition(), dataBuffer.remaining()) : "";
    return std::tuple<std::string, std::string>(std::move(msgContent), std::move(providerID));
}


template <class LockType>
VariableRecordStoreMemory<LockType>::VariableRecordStoreMemory(
        SessionID sessionID
        , const std::string& storeName
        , UnCompleteGroupMessageManagerPtr &ucgmmp 
        , const typename VariableRecordStoreBase<LockType>::StoreOptions& options
        , std::function<bool(uint64_t& result)>&& function
        , dep::ISeverityLog* pLogger)
      : VariableRecordStoreBase<LockType>(sessionID, options, storeName, ucgmmp)
      , m_getNonAckSeqFunction(function)
      , m_maxMemorySize(options.maxMemorySize)
      , m_pLogger(pLogger){
    m_pDataMemory = std::unique_ptr<CircularRawBuffer>(new CircularRawBuffer(options.defaultMemorySize, m_maxMemorySize));
    //m_pIndexMemory = std::unique_ptr<dep::CircularBuffer<MemoryIndexInfo>>(new dep::CircularBuffer<MemoryIndexInfo>(DEFAULT_MEMORY_INDEX_SIZE));
    m_pIndexMemory = std::unique_ptr<std::deque<MemoryIndexInfo>>(new std::deque<MemoryIndexInfo>());

    //m_recordCount is all history msg count, Real record count in memory is  m_recordCount - m_beginSequence
}

    template <class LockType>
    void VariableRecordStoreMemory<LockType>::getMany(uint64_t start, dep::ByteBuffer& buffer, std::vector<uint32_t>& indices) {
        getMany(start, m_baseSequence + m_recordCount - 1, buffer, indices);
    }
    template <class LockType>
    void VariableRecordStoreMemory<LockType>::getMany(uint64_t start, uint64_t end, dep::ByteBuffer& buffer, std::vector<uint32_t>& indices) {
        std::lock_guard<LockType> lock(m_lock);
        if (m_recordCount == 0)
            throw dep::DEPException{"VariableRecordStoreMemory<LockType>::getMany(): Memory session store is empty"};

        const uint64_t validStart = m_baseSequence;
        const uint64_t validEnd = m_baseSequence + m_recordCount - 1;

        if (start < validStart || start > validEnd)
            throw dep::DEPException{"Requested start sequence [" + std::to_string(start) + "] is invalid for memory session store, valid Seq[" + std::to_string(validStart) + "] to [" + std::to_string(validEnd) + "]"};
        if (end < validStart || end > validEnd)
            throw dep::DEPException{"Requested end sequence [" + std::to_string(end) + "] is invalid for memory session store, valid Seq[" + std::to_string(validStart) + "] to [" + std::to_string(validEnd) + "]"};
        if (start > end)
            throw dep::DEPException{"End sequence [" + std::to_string(end) + "] is greater than start sequence [" + std::to_string(start) + "] for memory session store"};

        //uint32_t readCount = 0;
        uint32_t byteCount = 0;


        DataStartIndexType dataStart;
        DataLengthType recordLength;

        auto maxBytes = buffer.remaining();
        auto seq = std::max(start, m_beginSequence);//read max(start, m_beginSequence) to end
        readIndexRecord(seq, dataStart, recordLength);

	    if (recordLength > maxBytes) {
		    buffer.ensureCapacity(recordLength);
            maxBytes = buffer.remaining();
	    }

        auto startIndex = dataStart;

        while (seq <= end && byteCount + recordLength <= maxBytes) {
            indices.emplace_back(byteCount);
            byteCount += recordLength;
            ++seq;

     #ifdef _DEBUG_LOG
     std::cout
     << "getMany"
     << " seq=" << seq
     << " byteCount=" << byteCount
     << " buffer.remaining()=" << buffer.remaining()
     << std::endl;
     #endif
         readIndexRecord(seq, dataStart, recordLength);
     #ifdef _DEBUG_LOG
         std::cout
         << "getMany"
         << " seq=" << seq
         << " byteCount=" << byteCount
         << " buffer.remaining()=" << buffer.remaining()
         << std::endl;
     #endif
         }
         // Read data into dep::byte buffer
         if (byteCount > 0) {
             m_pDataMemory->copy(buffer, startIndex, byteCount);
         }
    }
    template <class LockType>
    uint32_t VariableRecordStoreMemory<LockType>::getMany(uint64_t start, uint64_t end, dep::ByteBuffer& buffer, std::pair<uint64_t, uint64_t> &startEndPosition, uint64_t& expiredCount){
        std::lock_guard<LockType> lock(m_lock);
        if (m_recordCount == 0)
            throw dep::DEPException{"VariableRecordStoreMemory<LockType>::getMany(): Memory session store is empty"};

        const uint64_t validStart = m_baseSequence;
        const uint64_t validEnd = m_baseSequence + m_recordCount - 1;

        if (start < validStart || start > validEnd)
            throw dep::DEPException{"Requested start sequence [" + std::to_string(start) + "] is invalid for memory session store, valid Seq[" + std::to_string(validStart) + "] to [" + std::to_string(validEnd) + "]"};
        if (end < validStart || end > validEnd)
            throw dep::DEPException{"Requested end sequence [" + std::to_string(end) + "] is invalid for memory session store, valid Seq[" + std::to_string(validStart) + "] to [" + std::to_string(validEnd) + "]"};
        if (start > end)
            throw dep::DEPException{"End sequence [" + std::to_string(end) + "] is greater than start sequence [" + std::to_string(start) + "] for memory session store"};
		expiredCount = 0;
        if (end < m_beginSequence) {
            startEndPosition.first = end + 1;
            startEndPosition.second = end + 1;
			expiredCount = end - start + 1;
			return 0;
        }
		if (m_beginSequence > start) {
			expiredCount = m_beginSequence - start;
		}
        uint32_t readCount = 0;
        uint32_t byteCount = 0;


        DataStartIndexType dataStart;
        DataLengthType recordLength;

        auto maxBytes = buffer.remaining();
        auto seq = std::max(start, m_beginSequence);//read max(start, m_beginSequence) to end
        startEndPosition.first = seq;
        readIndexRecord(seq, dataStart, recordLength);

	    if (recordLength > maxBytes) {
		    buffer.ensureCapacity(recordLength);
            maxBytes = buffer.remaining();
	    }

        auto startIndex = dataStart;

         do {
            ++readCount;
            byteCount += recordLength;
            ++seq;
            readIndexRecord(seq, dataStart, recordLength);
		 } while (seq <= end && byteCount + recordLength <= maxBytes);
         // Read data into dep::byte buffer
         if (byteCount > 0) {
             m_pDataMemory->copy(buffer, startIndex, byteCount);
         }
         startEndPosition.second = startEndPosition.first + readCount - 1;
         return readCount;
    }

    template <class LockType>
    uint32_t VariableRecordStoreMemory<LockType>::getManyWithPrefixSuffixProxy(uint64_t start, dep::ByteBuffer& buffer)  {
        //memory session store without prefix and suffix
        //return getMany(start, m_baseSequence + m_recordCount - 1, buffer);
        return 0;
    }
    template <class LockType>
    uint32_t VariableRecordStoreMemory<LockType>::getManyWithPrefixSuffixProxy(uint64_t start, uint64_t end, dep::ByteBuffer& buffer) {
        //return getMany(start, end, buffer);
        return 0;
    }
    template <class LockType>
    uint32_t VariableRecordStoreMemory<LockType>::getManyReady(uint64_t start, uint64_t end, dep::ByteBuffer& buffer, std::pair<uint64_t, uint64_t> &startEndPosition, uint64_t & expiredCount) {
        std::lock_guard<LockType> lock(m_lock);
        if (m_recordCount == 0)
            throw dep::DEPException{"VariableRecordStoreMemory<LockType>::getMany(): Memory session store is empty"};

        const uint64_t validStart = m_baseSequence;
        const uint64_t validEnd = m_baseSequence + m_recordCount - 1;

        if (start < validStart || start > validEnd)
            throw dep::DEPException{"Requested start sequence [" + std::to_string(start) + "] is invalid for memory session store, valid Seq[" + std::to_string(validStart) + "] to [" + std::to_string(validEnd) + "]"};
        if (end < validStart || end > validEnd)
            throw dep::DEPException{"Requested end sequence [" + std::to_string(end) + "] is invalid for memory session store, valid Seq[" + std::to_string(validStart) + "] to [" + std::to_string(validEnd) + "]"};
        if (start > end)
            throw dep::DEPException{"End sequence [" + std::to_string(end) + "] is greater than start sequence [" + std::to_string(start) + "] for memory session store"};
		expiredCount = 0;
		if (end < m_beginSequence) {
            startEndPosition.first = end + 1;
            startEndPosition.second = end + 1;
			expiredCount = end - start + 1;
            return 0;
        }
		if (m_beginSequence > start) {
			expiredCount = m_beginSequence - start;
		}
		uint32_t readCount = 0;
        uint32_t byteCount = 0;
        bool isReady = false;


        DataStartIndexType dataStart;
        DataLengthType recordLength;

        auto maxBytes = buffer.remaining();
        auto seq = std::max(start, m_beginSequence);//read max(start, m_beginSequence) to end
        startEndPosition.first = seq;
        readIndexRecord(seq, dataStart, recordLength);
        isReady = this->getIsSequenceReady(seq - m_baseSequence);
        if (!isReady) {
            startEndPosition.second = seq;
            return 0;
        }

	    if (recordLength > maxBytes) {
		    buffer.ensureCapacity(recordLength);
            maxBytes = buffer.remaining();
	    }
        
        auto startIndex = dataStart;
        while (seq <= end && byteCount + recordLength <= maxBytes) {
            ++readCount;
            byteCount += recordLength;
            ++seq;
            readIndexRecord(seq, dataStart, recordLength);
            isReady = this->getIsSequenceReady(seq - m_baseSequence);
            if (!isReady) {
                break;
            }
         }
         // Read data into dep::byte buffer
         if (byteCount > 0) {
             m_pDataMemory->copy(buffer, startIndex, byteCount);
         }
         startEndPosition.second = startEndPosition.first + readCount - 1;
         return readCount;
    }

    template <class LockType>
    void VariableRecordStoreMemory<LockType>::readSingleMessageBuffer(uint64_t seq, dep::ByteBuffer& buffer) {
        std::lock_guard<LockType> lock(m_lock);
        if (m_recordCount == 0)
            throw dep::DEPException{"get(): Memory session store is empty"};

        const uint64_t validStart = m_beginSequence;
        const uint64_t validEnd = m_baseSequence + m_recordCount - 1;

        if (seq < validStart || seq > validEnd)
            throw dep::DEPException{"VariableRecordStoreMemory<LockType>::get(): Requested sequence [" + std::to_string(seq) + "] is invalid, valid Seq[" + std::to_string(validStart) + "] to [" + std::to_string(validEnd) + "]"};

        DataStartIndexType dataStart;
        DataLengthType length;
        readIndexRecord(seq, dataStart, length);

        if (length) {
            m_pDataMemory->copy(buffer, dataStart, length);
        }
    }
    template <class LockType>
    uint32_t VariableRecordStoreMemory<LockType>::getRecordSizeWithPrefixSuffix(uint64_t seq) {
        std::lock_guard<LockType> lock(m_lock);

        if (m_recordCount == 0)
            throw dep::DEPException{"VariableRecordStoreMemory<LockType>::get(): Session store is empty"};

        const uint64_t validStart = m_beginSequence;
        const uint64_t validEnd = m_baseSequence + m_recordCount - 1;
        if (seq < validStart || seq > validEnd)
            throw dep::DEPException{"VariableRecordStoreMemory<LockType>::get(): Requested sequence [" + std::to_string(seq) + "] is invalid, valid Seq[" + std::to_string(validStart) + "] to [" + std::to_string(validEnd) + "]"};

        DataStartIndexType dataStart;
        DataLengthType length;
        readIndexRecord(seq, dataStart, length);

        return length;

    }
    template <class LockType>
    uint64_t VariableRecordStoreMemory<LockType>::add(RecordStoreAddMessageParam &addParam)
	{
        //PROF_NF_BEGIN("addToDataFile");
        std::lock_guard<LockType> lock(m_lock);

        if (!this->testGroupMessage(addParam.m_msgAgent ? &addParam.m_msgAgent->m_header.getVariantMap() : nullptr))
        {
            DEBUG_PRINT("VariableRecordStoreMemory::add got error groupMessage. storeName<%s> \n", m_storeName.c_str());
            return m_recordCount; 
        }

        auto addDataFileRes = addToDataFile(*addParam.m_buffer);
        //PROF_NF_END("addToDataFile");
        int64_t currentSeq = m_baseSequence + m_recordCount;
        ++m_recordCount;

		MessageAttributeType msgAttrMask = 0;
		if (addParam.m_isRemoteMessage)
			msgAttrMask |= REMOTE_MESSAGE_MASKER;
		if (addParam.m_isDeletedMessage)
			msgAttrMask |= DELETED_MESSAGE_MASKER;
		if (addParam.m_msgAgent && addParam.m_msgAgent->getTransactionID().m_transID)
            RecordStoreDef::setMessageTransactionState(msgAttrMask, RecordStoreDef::MESSAGE_TRANSACTION_COMMIT);
		MessageAttributeType attribute = this->addMessageAttribute(currentSeq, msgAttrMask, addParam.m_msgAgent ? &addParam.m_msgAgent->m_header.getVariantMap() : nullptr);
        this->checkLevelInfo(currentSeq, addParam.getPriority());

        addIndexRecord(std::get<1>(addDataFileRes),
		               std::get<0>(addDataFileRes),
		               addParam.m_timestamp.time_since_epoch().count(),
		               attribute);

        return m_recordCount;

    }

    template <class LockType>
    std::tuple<uint32_t, uint64_t> VariableRecordStoreMemory<LockType>::addToDataFile(dep::ByteBuffer& buffer) {
        if (buffer.remaining() > m_maxMemorySize)
            throw dep::DEPException{ "Data to write [" + std::to_string(buffer.remaining()) + "] exceeds maximum memory size [" + std::to_string(m_maxMemorySize) + "]" };

        const auto dataLength = buffer.remaining();

        // 重复了
        // if (dataLength > m_maxMemorySize)
        //     throw dep::DEPException{ "Data to add is too large for memory session store" };

        if (dataLength + m_pDataMemory->size() > m_pDataMemory->capacity()) {
            //delete acked seq, for no expand
            uint64_t minNonAckRecord = 0;
            m_getNonAckSeqFunction(minNonAckRecord);

            
            if(minNonAckRecord > m_beginSequence) {
				DEP_LOG_INFO(m_pLogger, "VariableRecordStoreMemory<LockType>::addToDataFile data memory is full, ready to delete acked record, store name[", m_storeName,
					"] current data memory size[", m_pDataMemory->size(),
					"] add data size[", dataLength,
					"] current data memory capacity[", m_pDataMemory->capacity(),
					"] delete end sequence[", minNonAckRecord - 1,
					"].");
				deleteRecord(minNonAckRecord - 1);
			}
        }

        while (dataLength + m_pDataMemory->size() > m_pDataMemory->maxCapacity()) {
            //release half of circle raw buffer
            auto deleteEndIndex = (m_beginSequence + m_baseSequence + m_recordCount) / 2;
            DEP_LOG_INFO(m_pLogger, "VariableRecordStoreMemory<LockType>::addToDataFile data memory is full, ready to delete half of buffer, store name[", m_storeName,
                                      "] current data memory size[", m_pDataMemory->size(),
                                      "] current data memory max capacity[", m_pDataMemory->maxCapacity(),
                                      "] delete end sequence[", deleteEndIndex,
                                      "].");
            deleteRecord(deleteEndIndex);
        }

        if(dataLength + m_pDataMemory->size() > m_pDataMemory->capacity()) {
            //extend  buf
            extendMemoryBuffer(dataLength);
        }
        size_t writeIndex = 0;
        if (dataLength) {
            writeIndex = m_pDataMemory->write(buffer);
        }
        return std::make_tuple(dataLength, writeIndex);
    }

    // template <class LockType>
    // uint64_t VariableRecordStoreMemory<LockType>::addToInfoFile(const std::unordered_map<std::string, std::string>& attributes) {
    //     //no need to impl
    //     return 0;
    // }
    template <class LockType>
    void VariableRecordStoreMemory<LockType>::sync() {}
    template <class LockType>
    void VariableRecordStoreMemory<LockType>::addIndexRecord(DataStartIndexType dataStart, DataLengthType length, const TimestampType& timestamp, uint8_t messageAttribute, uint8_t messageMark){
        MemoryIndexInfo indexinfo{dataStart, length, timestamp, messageAttribute, messageMark};
        m_pIndexMemory->push_back(std::move(indexinfo));
    }

    template <class LockType>
    void VariableRecordStoreMemory<LockType>::readIndexRecord(uint64_t seq, DataStartIndexType &dataStart, DataLengthType &length){
        auto offset = seq - m_beginSequence;
        if (offset < m_pIndexMemory->size()){
            const MemoryIndexInfo &indexinfo = m_pIndexMemory->at(offset);
            dataStart = indexinfo.dataStart;
            length = indexinfo.length;
        }
    }
    template <class LockType>
    void VariableRecordStoreMemory<LockType>::markAttribute(uint64_t start, uint64_t end, MessageAttributeType marker) {
        if (m_recordCount == 0)
            throw dep::DEPException{ "Session store is empty" };

        const uint64_t validStart = m_baseSequence;
        const uint64_t validEnd = m_baseSequence + m_recordCount - 1;
        if (start < validStart || start > validEnd)
            throw dep::DEPException{ "Requested start sequence [" + std::to_string(start) + "] is invalid, valid Seq[" + std::to_string(validStart) + "] to [" + std::to_string(validEnd) + "]" };
        if (end < validStart || end > validEnd)
            throw dep::DEPException{ "Requested end sequence [" + std::to_string(end) + "] is invalid, valid Seq[" + std::to_string(validStart) + "] to [" + std::to_string(validEnd) + "]" };
        if (start > end)
            throw dep::DEPException{ "End sequence [" + std::to_string(end) + "] is greater than start sequence [" + std::to_string(start) + "]" };


        //uint64_t adjustStart = start - m_baseSequence;
        //uint64_t adjustEnd = end - m_baseSequence;

        MessageAttributeType attribute = DEFAULT_MESSAGE_ATTRIBUTE;

        auto seq = std::max(start, m_beginSequence);
        for (; seq <= end; ++seq) {
            auto offset = seq - m_beginSequence;
            attribute |= marker;
            MemoryIndexInfo &indexinfo = m_pIndexMemory->at(offset);
            indexinfo.messageAttribute = attribute;
        }
    }
    template <class LockType>
    void VariableRecordStoreMemory<LockType>::deleteRecord(uint64_t deleteEnd){
        uint64_t endSequence = m_baseSequence + m_recordCount - 1;
        uint64_t deleteEndIndex = std::min(deleteEnd, endSequence);
        DEP_LOG_INFO(m_pLogger, "VariableRecordStoreMemory<LockType>::deleteRecord , store name[", m_storeName,
                                  "] delete begin seq[", m_beginSequence,
                                  "] delete end sequence[", deleteEndIndex,
                                  "].");
        //uint32_t readCount = 0;
        uint32_t byteCount = 0;

        DataStartIndexType dataStart;
        DataLengthType recordLength;

        auto seq = m_beginSequence; //delete begin seq
        readIndexRecord(seq, dataStart, recordLength);
        //uint32_t popCount = 0;
        //这里实现不是很好，可以后续优化

        while (seq <= deleteEndIndex) {
            byteCount += recordLength;
            m_pIndexMemory->pop_front();
            ++seq;
            //readIndexRecord(seq, dataStart, recordLength);
            if(!m_pIndexMemory->empty()) {
                const auto &indexinfo = m_pIndexMemory->front();
                dataStart = indexinfo.dataStart;
                recordLength = indexinfo.length;
            }
        }
        m_pDataMemory->ignore(byteCount);
        m_beginSequence = seq;
    }

	template<class LockType>
	void VariableRecordStoreMemory<LockType>::setNewBeginSequence(uint64_t newBeginSequence)
	{
		if (m_beginSequence >= newBeginSequence) {
			return;
		}
		DEP_LOG_INFO(m_pLogger, "VariableRecordStoreMemory<LockType>::setNewBeginSequence , store name[", m_storeName,
			"] new begin seq[", newBeginSequence,
			"].");
        if(newBeginSequence != 0){
		    if (m_recordCount < newBeginSequence - 1) {
			    deleteRecord(m_recordCount);
			    m_recordCount = newBeginSequence - 1;
				DEP_LOG_INFO(m_pLogger, "VariableRecordStoreMemory<LockType>::setNewBeginSequence , store name[", m_storeName,
					"] new begin seq[", newBeginSequence,
					"] reset new record count[", m_recordCount,
                    "].");
		    }
		    else{
			    deleteRecord(newBeginSequence - 1);
		    }
        }
		m_beginSequence = newBeginSequence;
	}
    template <class LockType>
    void VariableRecordStoreMemory<LockType>::extendMemoryBuffer(const uint32_t &intputLength){
        int64_t tmpDataStart = m_pDataMemory->extendBuffer(intputLength);
        if(tmpDataStart >= 0){
            for(auto i = 0; i < m_pIndexMemory->size(); ++i) {
                auto &indexinfo = m_pIndexMemory->at(i);
                indexinfo.dataStart = tmpDataStart;
                tmpDataStart = tmpDataStart + indexinfo.length;
            }
        }
    }


}} // namespace lightmq { namespace util {


#endif /* LIGHTMQ_LIGHTMQUTIL_INC_LIGHTMQVARIABLERECORDMEMORYSTORE_H_ */
