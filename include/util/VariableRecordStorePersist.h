////////////////////////////////////////////////////////////////////////////////
//
// VariableRecordStorePersist.h
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

#ifndef LIGHTMQVARIABLERECORDSTORE_H_
#define LIGHTMQVARIABLERECORDSTORE_H_

#ifndef _NDEBUG
//#define _DEBUG_LOG
#endif

#include <sys/vfs.h>

#include <limits> 
#ifdef _DEBUG_LOG
#include <iostream>
#endif
#include <boost/algorithm/string.hpp>
#include "VariableRecordStoreBase.h"
#include "ByteBuffer.h"
#include "DEPException.h"
#include "FileUtility.h"
#include "MPL.h"
#include "BufferedFileStream.h"
#include "NoLock.h"
#include "MemoryBackport.h"
#include "StringUtility.h"
#include "FsyncFileStream.h"
#include "MessageUtil.h"
#include "LightmqMessageHead.h"
#include "ISQLExpression.h"
#include "LightmqMessageHead.h"
#include "Util.h"
#include "LogWrapper.h"

namespace lightmq { namespace util {

struct DataBlock {
    DataBlock() : fileBuffer(new dep::ByteBuffer(32 * 1024 * 1024)) {

    }
    uint32_t fileNo{ std::numeric_limits<uint32_t>::max() };
    uint64_t fileOffset{ 0 };
    std::unique_ptr<dep::ByteBuffer> fileBuffer;
};

template <class FileChannel>
class FileRecordUtil {

public:
    template <typename T>
    static void readFromFile(FileChannel& fileChannel, T& value);
    template <typename T>
    static void writeToFile(FileChannel& fileChannel, const T& value);
    static void writeToFile(FileChannel& fileChannel, const char* s, uint32_t length);
    static void writeToFileAtPosition(FileChannel& fileChannel, const char* s, uint32_t length, uint64_t position);
protected:
    template <typename T>
    static void readFromFileImpl(FileChannel& fileChannel, T& value, const std::true_type&); // For integral type, handle conversion
    template <typename T>
    static void readFromFileImpl(FileChannel& fileChannel, T& value, const std::false_type&); // For non-integral type, no conversion

    template <typename T>
    static void writeToFileImpl(FileChannel& fileChannel, const T& value, const std::true_type&); // For integral type, handle conversion
    template <typename T>
    static void writeToFileImpl(FileChannel& fileChannel, const T& value, const std::false_type&); // For non-integral type, no conversion
};


/**
 * Plain data format without any prefix and suffix
 */
struct VariableRecordClassic {
    static constexpr auto INDEX_EXTENSION = ".VRI"; /**< Index file extension **/
    static constexpr auto DATA_EXTENSION = ".VRD";  /**< Data file extension **/
	static constexpr auto INFO_EXTENSION = ".VRF"; /**< INFO file extension **/
    static constexpr auto PREFIX_SIZE = 0;          /**< Data prefix size **/
    static constexpr auto SUFFIX_SIZE = 0;          /**< Data suffix size **/
};


template<typename FileChannel>
std::unique_ptr<FileChannel> defaultCreateFileChannelFunc() {
    return dep::make_unique<FileChannel>();
}

/**
 * A VSAM relative-record data set (RRDS) contains record ordered by their
 * relative key. The relative key is the relative record number (RRN) that
 * represents the location of the record relative to where the file begins.
 * The relative record number identifies the variable-length record.
 *
 * Data format should define at least 4 constant expressions to use this store
 * 1. INDEX_EXTENSION - Index file extension
 * 2. DATA_EXTENSION - Data file extension
 * 3. PREFIX_SIZE - Prefix size for each record
 * 4. SUFFIX_SIZE - Suffix size for each record
 *
 * If PREFIX_SIZE is non-zero, function "void putPrefix(uint32_t, dep::ByteBuffer&)"
 * must be defined to handle record prefix.
 *
 * If SUFFIX_SIZE is non-zero, function "void putSuffix(uint32_t, dep::ByteBuffer&)"
 * must be defined to handle record suffix.
 */
template <class DataFormat, class LockType = dep::NoLock, class FileChannel = dep::BufferedFileStream>
class VariableRecordStorePersist : public VariableRecordStoreBase<LockType> {
protected:
    using typename VariableRecordStoreBase<LockType>::FileNumberType;
    using typename VariableRecordStoreBase<LockType>::DataOffsetType;
    using typename VariableRecordStoreBase<LockType>::InfoOffsetType;
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
    using VariableRecordStoreBase<LockType>::m_transactionSequences;
    //using VariableRecordStoreBase<LockType>::m_messagePropertiesLock;
    using VariableRecordStoreBase<LockType>::m_messageProperties;
    using VariableRecordStoreBase<LockType>::m_lastHasPropertiesMessageSeq;
    using VariableRecordStoreBase<LockType>::m_notReadySequenceSet;

    using VariableRecordStoreBase<LockType>::m_storeName;
    //using VariableRecordStoreBase<LockType>::m_referenceTime;
    using VariableRecordStoreBase<LockType>::m_baseSequence;       /**< index of the base record */
    using VariableRecordStoreBase<LockType>::m_recordCount;

    using VariableRecordStoreBase<LockType>::DEFAULT_MESSAGE_ATTRIBUTE;
    using VariableRecordStoreBase<LockType>::DEFAULT_MESSAGE_MARK;
    using VariableRecordStoreBase<LockType>::HAS_ATTRIBUTE_MESSAGE_MASKER;
    using VariableRecordStoreBase<LockType>::DELETED_MESSAGE_MASKER;
    using VariableRecordStoreBase<LockType>::REMOTE_MESSAGE_MASKER;


public:
    static constexpr uint32_t VERSION = 1;
    /*
    static constexpr uint8_t DEFAULT_MESSAGE_ATTRIBUTE = 0;
	static constexpr uint8_t DEFAULT_MESSAGE_MARK = 0;
	static constexpr uint8_t HAS_ATTRIBUTE_MESSAGE_MASKER = 0x1;
    static constexpr uint8_t DELETED_MESSAGE_MASKER = 0x2;
	static constexpr uint8_t REMOTE_MESSAGE_MASKER = 0x4;
*/

    /**
     * Constructor
     *
     * @param storePath  Full path to the record store
     * @param storeName  Name of the record store
     * @param options    Create options
     */
    VariableRecordStorePersist(SessionID sessionID
                            , const std::string& storePath
                            , const std::string& storeName
                            , UnCompleteGroupMessageManagerPtr &ucgmmp 
                            , const typename VariableRecordStoreBase<LockType>::StoreOptions& options = StoreOptions{}
                            , std::function<std::unique_ptr<FileChannel>()>&& createFileChannelFunc = defaultCreateFileChannelFunc<FileChannel>);

    /**
     * Destructor
     */
    virtual ~VariableRecordStorePersist();

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
    template <class OutputDataFormat>
    uint32_t getManyWithPrefixSuffix(uint64_t start, dep::ByteBuffer& buffer);
    //Suppose dataformat is the same
    virtual uint32_t getManyWithPrefixSuffixProxy(uint64_t start, dep::ByteBuffer& buffer) override;


    /**
     * Get consecutive records starting from RRN "start" up to buffer capacity or RRN "end,
     * obtained records are with data prefix and suffix. Prefix and suffix are constructed
     * according to OutputDataFormat.
     *
     * Normally should not be used to obtain plain data format (without prefix/suffix)
     * because record boundaries cannot be identified.
     *
     * @tparam OutputDataFormat  Output data format
     * @param start              Start RRN
     * @param end                End RRN
     * @param buffer             Output buffer
     * @return
     */
    template <class OutputDataFormat>
    uint32_t getManyWithPrefixSuffix(uint64_t start, uint64_t end, dep::ByteBuffer& buffer);
    //Suppose dataformat is the same
    virtual uint32_t getManyWithPrefixSuffixProxy(uint64_t start, uint64_t end, dep::ByteBuffer& buffer) override;



    /**
     * A first consecutive batch of message which are ready
     * (OutputDataFormat = InputDataFormat) so no template is defined
     * @param start
     * @param end
     * @param buffer
     * @return number of message read
     */
    virtual uint32_t getManyReady(uint64_t start, uint64_t end, dep::ByteBuffer& buffer, std::pair<uint64_t, uint64_t> &startEndPosition, uint64_t& expiredCount) override;

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
     * @param timestamp  Record timestamp
     */
    //void add(dep::ByteBuffer& buffer, const std::chrono::high_resolution_clock::time_point& timestamp = std::chrono::high_resolution_clock::now());

    /**
     * Add record
     *
	 * @param buffer     Input buffer
	 * @param isRemoteMessage     is message from remote broker
	 * @param messageProperties  the properties of the message
	 * @param timestamp  Record timestamp
     */
    virtual uint64_t add(RecordStoreAddMessageParam &param) override;


    /**
     * Add date
     *
     * @param buffer     Input buffer
     */
    virtual std::tuple<uint32_t, uint64_t> addToDataFile(dep::ByteBuffer& buffer) override;

    // virtual uint64_t addToInfoFile(const std::unordered_map<std::string, std::string>& attributes) override;
    /**
    * mark the message from start to end as deleted
	* @param uint64_t start
	* @param uint64_t end
    */

    //void markDeleted(uint64_t start, uint64_t end) override;

	//bool isDeleted(uint64_t seq) const override;

	//bool isDeletedOrRemoteMessage(uint64_t seq) const override;

    //std::vector<uint64_t> getDeletedSequences(uint64_t start, uint64_t end) const override;

    //std::vector<uint64_t> getRemoteSequences(uint64_t start, uint64_t end) const override;

    //std::vector<uint64_t> getRemoteOrDeletedSequences(uint64_t start, uint64_t end) const override;

	//std::vector<uint64_t> getNotDeletedSequences() const override;

    //uint64_t getLastHasProperitesMessageSequence() const override;

    //uint64_t getOldestDeletedSequence() const override;

    //const VariantMap* getMessageProperties(uint64_t sequence) const override;


    /**
     * Check if a session store exists
     *
     * @param storePath  Full path to the record store
     * @param storeName  Name of the record store
     * @return  True if the store exists
     */
    static bool isStoreExists(const std::string& storePath, const std::string& storeName);



    /**
     * Synchronize kernel buffer with storage device
     */
    virtual void sync() override;

    //void setSequenceReady(uint64_t seq)override;

    //void setSequenceNotReady(uint64_t seq)override;

    //void setAllSequenceReady() override;

    //std::tuple<uint64_t, uint64_t> getLastLocalMessageSequence() const override;

    //LockType &getLock() override {
    //    return m_lock;
    //}

protected:
    static constexpr auto INDEX_RECORD_SIZE = sizeof(FileNumberType) + sizeof(DataOffsetType) + sizeof(InfoOffsetType) + sizeof(TimestampType) + sizeof(MessageAttributeType) + 3*sizeof(ReservedByteType); /**< Data size for each record in index file */
    static constexpr auto INDEX_HEADER_SIZE = 32;
    static constexpr auto INDEX_ATTRIBUTE_OFFSET = sizeof(FileNumberType) + sizeof(DataOffsetType) + sizeof(InfoOffsetType) + sizeof(TimestampType);
    //static constexpr auto INDEX_MARK_OFFSET = sizeof(FileNumberType) + sizeof(DataOffsetType) + sizeof(TimestampType) + sizeof(MessageAttributeType);
    /**
     * Initialize record store with index and data files
     *
     * @param append  True if append to an existing store
     */
    void init(bool append, uint32_t referenceTime);

    bool parseSessionData(dep::ByteBuffer &buffer, VariantMap &vmap, uint8_t &level);
    void rebuildInfo();
    void rebuildInfoFromIndex();
    void rebuildAttributeInfo();

    /**
     * Remove record store - delete all index and data files
     */
    void remove();

protected:

    void validateDataFile(FileNumberType fileNumber, DataOffsetType fileOffset );
    // Data File IO
    static int64_t readFromDataFile(FileChannel& dataFile, dep::ByteBuffer& buffer, uint64_t length);
    void writeToDataFile(dep::ByteBuffer& buffer);

    std::string getDataFileName(FileNumberType number) const;

    // Index File IO
    void generateIndexFileHeader(uint32_t version, uint32_t referenceTime);
    void readIndexRecord(FileNumberType& number, DataOffsetType& offset);
    void readIndexRecord(FileNumberType& number, DataOffsetType& offset, TimestampType& timestamp);
    void readIndexRecord(FileNumberType& number, DataOffsetType& offset, TimestampType& timestamp, MessageAttributeType& attribute);
    void readIndexRecord(FileNumberType& number, DataOffsetType& offset, TimestampType& timestamp, MessageAttributeType& attribute, MessageMarkType& mark);

    void readIndexRecord(uint64_t recordOffset, FileNumberType& number, DataOffsetType& offset, TimestampType& timestamp);
	void readIndexRecord(uint64_t recordOffset, FileNumberType& number, DataOffsetType& offset, TimestampType& timestamp, MessageAttributeType& attribute);
	void readIndexRecord(uint64_t recordOffset, FileNumberType& number, DataOffsetType& offset, TimestampType& timestamp, MessageAttributeType& attribute, MessageMarkType& mark);

	void readIndexRecordAttribute(MessageAttributeType &attr/*, InfoOffsetType& infoFileOffset*/);


    void addIndexRecord(FileNumberType number, DataOffsetType offset, const TimestampType& timestamp, uint8_t messageAttribute=DEFAULT_MESSAGE_ATTRIBUTE, uint8_t messageMark=DEFAULT_MESSAGE_MARK);
    uint64_t seqToOffset(uint64_t seq) const { return (seq - m_baseSequence + 1) * INDEX_RECORD_SIZE; }
    uint64_t offsetToSeq(uint64_t offset) const {return (offset / INDEX_RECORD_SIZE) + m_baseSequence - 1;}

    template <typename T>
    void readFromIndexFile(T& value);
    template <typename T>
    void writeToIndexFile(const T& value);
    void writeToIndexFile(const char* s, uint32_t length);

    // Access to data file handle
    FileChannel* acquireDataFile(FileNumberType fileNumber);
    void releaseDataFile(FileNumberType fileNumber);

    // Reference time generation
    void initReferenceTime(uint32_t referenceTime);
    static uint32_t generateReferenceTime();

    // Get may without prefix/suffix implementations
    template <bool CheckEnd>
    void getManyImpl(uint64_t start, uint64_t end, dep::ByteBuffer& buffer, std::vector<uint32_t>& indices, const std::true_type&);
    template <bool CheckEnd>
    void getManyImpl(uint64_t start, uint64_t end, dep::ByteBuffer& buffer, std::vector<uint32_t>& indices, const std::false_type&);

    template <bool CheckEnd>
    void getManyImpl_DELETE(uint64_t start, uint64_t end, dep::ByteBuffer& buffer, std::vector<uint32_t>& indices, const std::false_type&);
    // Get many with prefix/suffix implementations
    template <bool CheckEnd, class OutputDataFormat>
    uint32_t getManyWithPrefixSuffixImpl(uint64_t start, uint64_t end, dep::ByteBuffer& buffer, const std::true_type&);
    template <bool CheckEnd, class OutputDataFormat>
    uint32_t getManyWithPrefixSuffixImpl(uint64_t start, uint64_t end, dep::ByteBuffer& buffer, const std::false_type&);
    template <bool CheckEnd, class OutputFormat>
    uint32_t getManyWithPrefixSuffixConversion(uint64_t start, uint64_t end, dep::ByteBuffer& buffer);
    template <bool CheckEnd>
    uint32_t getManyWithPrefixSuffixNoConversion(uint64_t start, uint64_t end, dep::ByteBuffer& buffer);

    virtual uint64_t getIndexTimeStamp(uint64_t index) override;
    void getIndexDataInfo(dep::ByteBuffer& buffer, uint64_t index, FileNumberType& fileNo, DataOffsetType& dataOffset); 
    virtual std::tuple<std::string, std::string> getRecordData(uint64_t index) override;

    // Data format related functions
    static constexpr const char* getIndexExtension();
    static constexpr const char* getDataExtension();
	static constexpr const char* getInfoExtension();
    static uint64_t getDataStart(uint64_t recordStart);
    static uint64_t getDataEnd(uint64_t recordEnd);

    template <class TargetFormat = DataFormat>
    static constexpr uint32_t getPrefixSize();
    template <class TargetFormat = DataFormat>
    static constexpr uint32_t getSuffixSize();
    template <class TargetFormat = DataFormat>
    static void putPrefix(uint32_t length, dep::ByteBuffer& buffer);
    template <class TargetFormat = DataFormat>
    static void putSuffix(uint32_t length, dep::ByteBuffer& buffer);

    template <class TargetFormat>
    static void putPrefixImpl(uint32_t length, dep::ByteBuffer& buffer, const std::true_type&);
    template <class TargetFormat>
    static void putPrefixImpl(uint32_t length, dep::ByteBuffer& buffer, const std::false_type&);
    template <class TargetFormat>
    static void putSuffixImpl(uint32_t length, dep::ByteBuffer& buffer, const std::true_type&);
    template <class TargetFormat>
    static void putSuffixImpl(uint32_t length, dep::ByteBuffer& buffer, const std::false_type&);

	virtual void markAttribute(uint64_t start, uint64_t end, MessageAttributeType marker) override;

    std::shared_ptr<VariantMap> readAttributeRecord(uint64_t startPos, uint32_t length);

    std::string m_storePath;

    //std::unique_ptr<FileChannel> m_pIndexFile;
    std::unique_ptr<dep::FsyncFileStream> m_pIndexFile;
	// std::unique_ptr<dep::FsyncFileStream> m_pInfoFile;
    std::unique_ptr<FileChannel> m_pDataFile;       // File handle to the latest data file
    std::unique_ptr<FileChannel> m_pTempDataFile;   // File handle to history data files for read only

    //uint64_t m_recordCount;//move to VariableRecordStoreBase
    uint32_t m_dataFileCount;
    uint64_t m_maxFileSize;
    bool m_deleteOnExit;
    //int64_t m_referenceTime;
    dep::ByteBuffer m_prefixBuffer;
    dep::ByteBuffer m_suffixBuffer;

    static std::unique_ptr<dep::ByteBuffer>& getQueryIndexBuffer();
    static std::unique_ptr<DataBlock>& getQueryDataBlock();
    
    //uint32_t m_baseSequence;       /**< index of the base record *///move to VariableRecordStoreBase

    //mutable LockType m_lock; //move to VariableRecordStoreBase
    //std::unordered_set<uint64_t> m_notReadySequenceSet;//move to VariableRecordStoreBase
	//std::unordered_set<uint64_t> m_deletedSequenceSet;//move to VariableRecordStoreBase
	//std::unordered_set<uint64_t> m_remoteSequenceSet;//move to VariableRecordStoreBase
	//std::unordered_set<uint64_t> m_notDeleteSequenceSet;//move to VariableRecordStoreBase

    //mutable LockType m_messagePropertiesLock;//move to VariableRecordStoreBase
    //std::unordered_map<uint64_t, std::shared_ptr<std::unordered_map<std::string, std::string>>> m_messageProperties;//move to VariableRecordStoreBase

    //uint64_t m_lastHasPropertiesMessageSeq{ 0 };

    dep::ByteBuffer m_indexMessage{ 32 };
	dep::ByteBuffer m_propertiseMessage{ 64 };

protected:
    virtual void initQueryContext() override;
    virtual void destroyQueryContext() override;
};

template <class DataFormat, class LockType /*= dep::NoLock*/, class FileChannel /*= dep::BufferedFileStream*/>
std::unique_ptr<dep::ByteBuffer>& lightmq::util::VariableRecordStorePersist<DataFormat, LockType, FileChannel>::getQueryIndexBuffer()
{
    static thread_local std::unique_ptr<dep::ByteBuffer> queryIndexBuffer;
    return queryIndexBuffer;
}

template <class DataFormat, class LockType /*= dep::NoLock*/, class FileChannel /*= dep::BufferedFileStream*/>
std::unique_ptr<lightmq::util::DataBlock>& lightmq::util::VariableRecordStorePersist<DataFormat, LockType, FileChannel>::getQueryDataBlock()
{
    static thread_local std::unique_ptr<DataBlock> dataBlock;
    return dataBlock;
}



template <class DataFormat, class LockType /*= dep::NoLock*/, class FileChannel /*= dep::BufferedFileStream*/>
void lightmq::util::VariableRecordStorePersist<DataFormat, LockType, FileChannel>::initQueryContext()
{
    getQueryIndexBuffer().reset(new dep::ByteBuffer());
 
    {
        std::lock_guard<decltype(m_lock)> lock(m_lock);
        dep::ByteBuffer& buffer = *getQueryIndexBuffer();
        buffer = m_pIndexFile->readAllFile();        
        // std::cout << "============";      
        // for (int i = 0; i < buffer.getLimit(); ++i) {
        //     std::cout << (int)(int8_t)buffer.getArray()[i] << ",";
        // }
        // std::cout << std::endl;
    }
   

    getQueryDataBlock().reset(new DataBlock());
}

template <class DataFormat, class LockType /*= dep::NoLock*/, class FileChannel /*= dep::BufferedFileStream*/>
void lightmq::util::VariableRecordStorePersist<DataFormat, LockType, FileChannel>::destroyQueryContext()
{
    getQueryIndexBuffer().reset();
    getQueryDataBlock().reset();
}

template <class DataFormat, class LockType /*= dep::NoLock*/, class FileChannel /*= dep::BufferedFileStream*/>
uint64_t lightmq::util::VariableRecordStorePersist<DataFormat, LockType, FileChannel>::getIndexTimeStamp(uint64_t index)
{
    dep::ByteBuffer& indexBuffer = *getQueryIndexBuffer();
    indexBuffer.setPosition(RecordStoreDef::INDEX_RECORD_LENGTH* (index -1) + sizeof(FileNumberType) + sizeof(DataOffsetType) + sizeof(InfoOffsetType));
    TimestampType timeStamp{ 0 };
    indexBuffer >> timeStamp;
    //timeStamp += m_referenceTime;
    return timeStamp;
}

template <class DataFormat, class LockType /*= dep::NoLock*/, class FileChannel /*= dep::BufferedFileStream*/>
void lightmq::util::VariableRecordStorePersist<DataFormat, LockType, FileChannel>::getIndexDataInfo(dep::ByteBuffer& buffer, uint64_t index, FileNumberType& fileNo, DataOffsetType& dataOffset){
    buffer.setPosition(RecordStoreDef::INDEX_RECORD_LENGTH * index);
    buffer >> fileNo;
    buffer >> dataOffset;   
}

template <class DataFormat, class LockType /*= dep::NoLock*/, class FileChannel /*= dep::BufferedFileStream*/>
std::tuple<std::string, std::string> lightmq::util::VariableRecordStorePersist<DataFormat, LockType, FileChannel>::getRecordData( uint64_t index)
{
    dep::ByteBuffer& indexBuffer = *getQueryIndexBuffer();
    DataBlock& dataBlock = *getQueryDataBlock();
    FileNumberType lastFileNo{ 0 };
    FileNumberType fileNo{ 0 };
    DataOffsetType dataStart{ 0 };
    DataOffsetType dataEnd{ 0 };
    getIndexDataInfo(indexBuffer, index, fileNo, dataEnd);
    if (1 != index) {
        getIndexDataInfo(indexBuffer, index - 1, lastFileNo, dataStart);
    }

    if (lastFileNo != fileNo) {
        dataStart = 0;
    }
    uint64_t dataBlockEnd = dataBlock.fileOffset + dataBlock.fileBuffer->getLimit();

    dataStart = getDataStart(dataStart);
    dataEnd = getDataEnd(dataEnd);
    uint64_t dataLength = dataEnd - dataStart;

    if (fileNo != dataBlock.fileNo ||
        dataStart < dataBlock.fileOffset || dataStart >= dataBlockEnd ||
        dataEnd < dataBlock.fileOffset || dataEnd > dataBlockEnd) //数据不在区间内
    {
        std::lock_guard<decltype(m_lock)> lock(m_lock);
        dataBlock.fileBuffer->clear();
        if (dataBlock.fileBuffer->getLimit() < dataLength) {
            dataBlock.fileBuffer.reset(new dep::ByteBuffer(dataLength));
        }

        auto pFileHandle = acquireDataFile(fileNo);
        pFileHandle->seekg(dataStart);
        readFromDataFile(*pFileHandle, *dataBlock.fileBuffer, dataBlock.fileBuffer->remaining());
        releaseDataFile(fileNo);
        dataBlock.fileBuffer->flip();

        dataBlock.fileOffset = dataStart;
        dataBlock.fileNo = fileNo;
    }
    dep::ByteBuffer dataBuffer(dataBlock.fileBuffer->getArray() + dataStart - dataBlock.fileOffset, dataLength, false);

    std::string providerID;
    ExtendBmpRemoteBrokerInfo extendRemoteBrokerInfo;
    MessageUtil::parseStoreData(dataBuffer, &extendRemoteBrokerInfo, &providerID, nullptr, nullptr, true);

    LightmqMessageHead msgHead;
    msgHead.parseAll(dataBuffer);
    bool isPlainMsg = std::find_if(msgHead.getProcessFlags().begin(), msgHead.getProcessFlags().end(), [](int val)->bool {return val == LightmqMessageHead::COMPRESS || val == LightmqMessageHead::ENCRYPT; }) == msgHead.getProcessFlags().end();
    std::string msgContent = isPlainMsg ? std::string((const char*)dataBuffer.getArray() + dataBuffer.getPosition(), dataBuffer.remaining()) : "";
    return std::tuple<std::string, std::string>(std::move(msgContent), std::move(providerID));
}


/*
template <class DataFormat, class LockType , class FileChannel >
uint64_t VariableRecordStorePersist<DataFormat, LockType, FileChannel>::getOldestDeletedSequence() const
{
    std::lock_guard<LockType> lock(m_lock);
    uint64_t oldestSeq = ULLONG_MAX;
    if (!m_deletedSequenceSet.empty()) {
        oldestSeq = *m_deletedSequenceSet.begin();
    }
    return oldestSeq;
}

template <class DataFormat, class LockType , class FileChannel >
uint64_t VariableRecordStorePersist<DataFormat, LockType, FileChannel>::getLastHasProperitesMessageSequence() const
{
    std::lock_guard<LockType> lock(m_lock);
    return m_lastHasPropertiesMessageSeq;
}

template <class DataFormat, class LockType , class FileChannel >
bool VariableRecordStorePersist<DataFormat, LockType, FileChannel>::isDeleted(uint64_t seq) const{
	std::lock_guard<LockType> lock(m_lock);
	return m_deletedSequenceSet.find(seq) != m_deletedSequenceSet.end();
}

template <class DataFormat, class LockType , class FileChannel >
bool VariableRecordStorePersist<DataFormat, LockType, FileChannel>::isDeletedOrRemoteMessage(uint64_t seq) const{
	std::lock_guard<LockType> lock(m_lock);
	return m_deletedSequenceSet.find(seq) != m_deletedSequenceSet.end()
        || m_remoteSequenceSet.find(seq) != m_remoteSequenceSet.end();
}

template <class DataFormat, class LockType , class FileChannel >
std::tuple<uint64_t, uint64_t> VariableRecordStorePersist<DataFormat, LockType, FileChannel>::getLastLocalMessageSequence() const{
    std::lock_guard<LockType> lock(m_lock);
    uint64_t localMsgCount = m_recordCount - m_remoteSequenceSet.size();
    auto lastSeq = m_recordCount + m_baseSequence -1;
    while (lastSeq >= m_baseSequence) {
        if (m_remoteSequenceSet.find(lastSeq) == m_remoteSequenceSet.end()) {
            break;
        }
        if (lastSeq == m_baseSequence) {
            break;
        }
        else {
            --lastSeq;
        }
    }
    return std::make_tuple(localMsgCount, lastSeq);
}
*/
/*
template <class DataFormat, class LockType , class FileChannel >
std::vector<uint64_t> VariableRecordStorePersist<DataFormat, LockType, FileChannel>::getNotDeletedSequences()  const {
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


template <class DataFormat, class LockType , class FileChannel >
std::vector<uint64_t> VariableRecordStorePersist<DataFormat, LockType, FileChannel>::getDeletedSequences(uint64_t start, uint64_t end)  const {
    std::lock_guard<LockType> lock(m_lock);
    std::vector<uint64_t> res;
    if (m_deletedSequenceSet.empty()) {
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

	for (auto i = start; i <= end; ++i) {
        if (m_deletedSequenceSet.find(i) != m_deletedSequenceSet.end()) {
            res.push_back(i);
        }
    }
    return res;
}

template <class DataFormat, class LockType , class FileChannel >
std::vector<uint64_t> VariableRecordStorePersist<DataFormat, LockType, FileChannel>::getRemoteSequences(uint64_t start, uint64_t end)  const {
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

	for (auto i = start; i <= end; ++i) {
        if (m_remoteSequenceSet.find(i) != m_remoteSequenceSet.end()) {
            res.push_back(i);
        }
    }
    return res;
}

template <class DataFormat, class LockType , class FileChannel >
std::vector<uint64_t> VariableRecordStorePersist<DataFormat, LockType, FileChannel>::getRemoteOrDeletedSequences(uint64_t start, uint64_t end) const {
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

	for (auto i = start; i <= end; ++i) {
        if (m_remoteSequenceSet.find(i) != m_remoteSequenceSet.end() || m_deletedSequenceSet.find(i) != m_deletedSequenceSet.end()) {
            res.push_back(i);
        }
    }
    return res;
}

template <class DataFormat, class LockType , class FileChannel>
const VariantMap* VariableRecordStorePersist<DataFormat, LockType, FileChannel>::getMessageProperties(uint64_t sequence) const {
    std::lock_guard<LockType> lock(m_messagePropertiesLock);
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
}*/

template <class DataFormat, class LockType /*= dep::NoLock*/, class FileChannel /*= dep::BufferedFileStream*/>
void VariableRecordStorePersist<DataFormat, LockType, FileChannel>::markAttribute(uint64_t start, uint64_t end, MessageAttributeType marker){
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

	uint64_t adjustStart = start - m_baseSequence;
	uint64_t adjustEnd = end - m_baseSequence;

    MessageAttributeType attribute;

	for (uint64_t seq = adjustStart; seq <= adjustEnd; ++seq) {
		int64_t curMarkOffset = seq * INDEX_RECORD_SIZE + INDEX_HEADER_SIZE + INDEX_ATTRIBUTE_OFFSET;
		m_pIndexFile->seekg(curMarkOffset);
		FileRecordUtil<dep::FsyncFileStream>::readFromFile(*m_pIndexFile, attribute);
        attribute |= marker;
		FileRecordUtil<dep::FsyncFileStream>::writeToFileAtPosition(*m_pIndexFile, (const char*)&attribute, sizeof(attribute), curMarkOffset);
	}
}

/*
template <class DataFormat, class LockType , class FileChannel >
void VariableRecordStorePersist<DataFormat, LockType, FileChannel>::markDeleted(uint64_t start, uint64_t end){
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
}*/

template <class DataFormat, class LockType, class FileChannel>
VariableRecordStorePersist<DataFormat, LockType, FileChannel>::VariableRecordStorePersist(SessionID sessionID
        , const std::string& storePath
        , const std::string& storeName
        , UnCompleteGroupMessageManagerPtr &ucgmmp 
        , const typename VariableRecordStoreBase<LockType>::StoreOptions& options
        , std::function<std::unique_ptr<FileChannel>()>&& createFileChannelFunc)
    : VariableRecordStoreBase<LockType>(sessionID, options, storeName, ucgmmp)
    , m_storePath(storePath)
    , m_dataFileCount(1)
    , m_maxFileSize(options.maxFileSize)
    , m_deleteOnExit(options.deleteOnExit)
    , m_prefixBuffer(getPrefixSize())
    , m_suffixBuffer(getSuffixSize()) {
    if (m_storePath.back() != '/')
        m_storePath += "/";

    if (!dep::FileUtility::isDirectoryExist(m_storePath))
        throw dep::DEPException{"Directory not exists: " + m_storePath};

    if (m_storeName.empty())
        throw dep::DEPException{"Store name cannot be empty"};

   // m_pIndexFile = createFileChannelFunc();
    m_pIndexFile= dep::make_unique<dep::FsyncFileStream>();
    // m_pInfoFile = dep::make_unique<dep::FsyncFileStream>();
    m_pDataFile = createFileChannelFunc();
    m_pTempDataFile = createFileChannelFunc();

    init(options.append, options.referenceTime);
}

template <class DataFormat, class LockType, class FileChannel>
VariableRecordStorePersist<DataFormat, LockType, FileChannel>::~VariableRecordStorePersist() {
    if (m_pIndexFile->is_open())
        m_pIndexFile->close();

	// if (m_pInfoFile->is_open())
	// 	m_pInfoFile->close();

    if (m_pDataFile->is_open())
        m_pDataFile->close();

    if (m_deleteOnExit)
        remove();
}

template <class DataFormat, class LockType, class FileChannel>
inline void VariableRecordStorePersist<DataFormat, LockType, FileChannel>::getMany(uint64_t start, dep::ByteBuffer& buffer, std::vector<uint32_t>& indices) {
    std::lock_guard<LockType> lock(m_lock);
    getManyImpl<false>(start, 0, buffer, indices, std::integral_constant<bool, (DataFormat::PREFIX_SIZE > 0 && DataFormat::SUFFIX_SIZE > 0)>{});
}

template <class DataFormat, class LockType, class FileChannel>
inline void VariableRecordStorePersist<DataFormat, LockType, FileChannel>::getMany(uint64_t start, uint64_t end, dep::ByteBuffer& buffer, std::vector<uint32_t>& indices) {
    std::lock_guard<LockType> lock(m_lock);
    getManyImpl<true>(start, end, buffer, indices, std::integral_constant<bool, (DataFormat::PREFIX_SIZE > 0 && DataFormat::SUFFIX_SIZE > 0)>{});
}

template <class DataFormat, class LockType, class FileChannel>
uint32_t VariableRecordStorePersist<DataFormat, LockType, FileChannel>::getMany(uint64_t start, uint64_t end, dep::ByteBuffer& buffer, std::pair<uint64_t, uint64_t> &startEndPosition, uint64_t& expiredCount) {
    std::lock_guard<LockType> lock(m_lock);
    std::vector<uint32_t> indices;
    getManyImpl<true>(start, end, buffer, indices, std::integral_constant<bool, (DataFormat::PREFIX_SIZE > 0 && DataFormat::SUFFIX_SIZE > 0)>{});
    if(indices.size() > 0) {
        startEndPosition.first = start;
        startEndPosition.second = start + indices.size() - 1;
    }
	expiredCount = 0;
    return indices.size();
}

template <class DataFormat, class LockType, class FileChannel>
template <bool CheckEnd>
void VariableRecordStorePersist<DataFormat, LockType, FileChannel>::getManyImpl(uint64_t start, uint64_t end, dep::ByteBuffer& buffer, std::vector<uint32_t>& indices, const std::true_type&) {
    if (m_recordCount == 0)
        throw dep::DEPException{"Session store is empty"};

    const uint64_t validStart = m_baseSequence;
    const uint64_t validEnd = m_baseSequence + m_recordCount - 1;

    if (start < validStart || start > validEnd)
        throw dep::DEPException{"Requested start sequence [" + std::to_string(start) + "] is invalid, valid Seq[" + std::to_string(validStart) + "] to [" + std::to_string(validEnd) + "]"};
    if (CheckEnd && (end < validStart || end > validEnd))
        throw dep::DEPException{"Requested end sequence [" + std::to_string(end) + "] is invalid, valid Seq[" + std::to_string(validStart) + "] to [" + std::to_string(validEnd) + "]"};
    if (CheckEnd && start > end)
        throw dep::DEPException{"End sequence [" + std::to_string(end) + "] is greater than start sequence [" + std::to_string(start) + "]"};

    uint64_t adjustStart = start - m_baseSequence;
    uint64_t adjustEnd = end - m_baseSequence;

    uint32_t readCount = 0;
    uint64_t byteCount = 0;
    auto currentSeq = adjustStart;
    auto endSeq = CheckEnd ? adjustEnd : m_recordCount - 1;

    FileNumberType fileStart = 0;
    FileNumberType fileEnd = 0;
    DataOffsetType readStart = 0;
    DataOffsetType readEnd = 0;

    if (adjustStart == 0)
        m_pIndexFile->seekg(INDEX_HEADER_SIZE);
    else {
        m_pIndexFile->seekg( (adjustStart -1) * INDEX_RECORD_SIZE + INDEX_HEADER_SIZE);
        readIndexRecord(fileStart, readStart);
    }

    readIndexRecord(fileEnd, readEnd);

    if (fileStart != fileEnd) {
        readStart = 0;
        fileStart = fileEnd;
    }

    auto dataStart = getDataStart(readStart);
    auto dataEnd = getDataEnd(readEnd);
    auto dataLength = dataEnd - dataStart;

    auto pFileHandle = acquireDataFile(fileEnd);

    do {
        // if (dataLength == 0) {
        //     //throw dep::DEPException{"Session store is empty"};
        //     break;
        // }

        indices.emplace_back(byteCount);

        pFileHandle->seekg(dataStart);
        readFromDataFile(*pFileHandle, buffer, dataLength);

        ++readCount;
        if (++currentSeq > endSeq)
            break;

        byteCount += dataLength;

        readStart = readEnd;
        readIndexRecord(fileEnd, readEnd);

        if (fileStart != fileEnd) {
            releaseDataFile(fileStart);
            pFileHandle = acquireDataFile(fileEnd);
            readStart = 0;
            fileStart = fileEnd;
        }

        dataStart = getDataStart(readStart);
        dataEnd = getDataEnd(readEnd);
        dataLength = dataEnd - dataStart;
	} while (dataLength <= buffer.remaining());

    if (m_pIndexFile->eof())
        m_pIndexFile->clear();

}

template <class DataFormat, class LockType, class FileChannel>
template <bool CheckEnd>
void VariableRecordStorePersist<DataFormat, LockType, FileChannel>::getManyImpl(uint64_t start, uint64_t end, dep::ByteBuffer& buffer, std::vector<uint32_t>& indices, const std::false_type&) {
    if (m_recordCount == 0)
        throw dep::DEPException{"Session store is empty"};

    const uint64_t validStart = m_baseSequence;
    const uint64_t validEnd = m_baseSequence + m_recordCount - 1;
    
    if (start < validStart || start > validEnd)
        throw dep::DEPException{"Requested start sequence [" + std::to_string(start) + "] is invalid, valid Seq[" + std::to_string(validStart) + "] to [" + std::to_string(validEnd) + "]"};
    if (CheckEnd && (end < validStart || end > validEnd))
        throw dep::DEPException{"Requested end sequence [" + std::to_string(end) + "] is invalid, valid Seq[" + std::to_string(validStart) + "] to [" + std::to_string(validEnd) + "]"};
    if (CheckEnd && start > end)
        throw dep::DEPException{"End sequence [" + std::to_string(end) + "] is greater than start sequence [" + std::to_string(start) + "]"};

    uint64_t adjustStart = start - m_baseSequence;
    uint64_t adjustEnd = end - m_baseSequence;

    auto maxRecords = CheckEnd ? adjustEnd - adjustStart + 1 : m_recordCount - start + m_baseSequence;

    FileNumberType fileStart = 0;
    FileNumberType fileEnd = 0;
    DataOffsetType recordStart = 0;
    DataOffsetType recordEnd = 0;

    if (adjustStart == 0)
        m_pIndexFile->seekg(INDEX_HEADER_SIZE);
    else {
        m_pIndexFile->seekg( (adjustStart -1) * INDEX_RECORD_SIZE + INDEX_HEADER_SIZE);
        readIndexRecord(fileStart, recordStart);
    }

    readIndexRecord(fileEnd, recordEnd);
    if (fileStart != fileEnd) {
        recordStart = 0;
        fileStart = fileEnd;
    }

    auto recordLength = recordEnd - recordStart;
    auto startIndex = recordStart;
    auto endIndex = recordEnd;

	//如果Buffer空间不够，进行扩容保证第一次有消息可以发送
    auto maxBytes = buffer.remaining();
	if (recordLength > maxBytes) {
		buffer.ensureCapacity(recordLength);
        // maxBytes = buffer.remaining();
	}

    // auto startPos = buffer.getPosition();

    bool isReadEnd = false;
    uint32_t readCount = 0;
    uint32_t byteCount = 0; // for indices
    do {
        // fileStart, startIndex, endIndex; // we will read file<fileStart> from <startIndex> to <endIndex>
        do
        {
            indices.emplace_back(byteCount);
            byteCount += recordLength;
            ++readCount;

            if (readCount >= maxRecords)
            {
                // DEBUG_PRINT("readManyImpl, storeName<%s> break with maxRecords<%u> readCount<%u> \n", m_storeName.c_str(), maxRecords, readCount);
                isReadEnd = true;
                break; 
            }

            recordStart = recordEnd;
            readIndexRecord(fileEnd, recordEnd);       
            if (fileEnd != fileStart)
            {
                recordLength = recordEnd;
            }
            else
            {
                recordLength = recordEnd - recordStart;            
            }

            if (byteCount + recordLength > maxBytes)
            {
                // DEBUG_PRINT("readManyImpl storeName<%s> break with maxBytes<%u> readCount<%u> byteCount<%u> recordLength<%u>\n", m_storeName.c_str(), maxBytes, readCount, byteCount, recordLength);
                isReadEnd = true;
                break; 
            }

            if (fileEnd != fileStart)
            {
                // DEBUG_PRINT("readManyImpl: storeName<%s> break with fileEnd<%u> readCount<%u> fileStart<%u>\n", m_storeName.c_str(),  fileEnd, readCount, fileStart);
                break;  // nextFile.
            }

            endIndex = recordEnd;
            // DEBUG_PRINT("readManyImpl: storeName<%s> readNext readCount<%u> \n", m_storeName.c_str(),  readCount);
        } while(true);

        // read here.
        // DEBUG_PRINT("readManyImpl: storeName<%s> read readCount<%u> file<%u,%u~%u> \n", m_storeName.c_str(), readCount, fileStart, startIndex, endIndex);
        auto pFileHandle = acquireDataFile(fileStart);
        pFileHandle->seekg(startIndex);
        int64_t bytesToRead = endIndex - startIndex;
        int64_t bytesReaded = readFromDataFile(*pFileHandle, buffer, bytesToRead);
        releaseDataFile(fileStart);
        // printf("read bytes <%ld> bytesToRead<%ld>....\n", bytesReaded, bytesToRead);

        if (bytesReaded < bytesToRead)
        {
            throw dep::DEPException{"Session store does has data size<" + std::to_string(bytesReaded) + "> less than expected size<" + std::to_string(bytesToRead) + ">" };
        }

        if (isReadEnd)
        {
            break; 
        }
        // 
        fileStart = fileEnd;
        startIndex = 0;
        recordStart = 0;
        endIndex = recordEnd;
        // printf("readManyImpl:%d: read readCount<%u> next file \n", __LINE__, readCount);
    } while (!isReadEnd);


    // auto s = util::hexdumpLimit((const char*)buffer.getArray()+startPos, buffer.getPosition() - startPos, 500);
    // printf("read messge from session store count<%u>.\n%s\n", buffer.getPosition() - startPos, s.c_str());

    if (m_pIndexFile->eof())
        m_pIndexFile->clear();
}


inline uint32_t fixMessageOffsets(std::vector<uint32_t>& indices, uint32_t bytesReaded) {
    int count = (int) indices.size();
    for (int i=0; i<count; ++i) {
        if (indices[i] > bytesReaded) {
            indices.resize(i);
            break;
        }
    }

    if (indices.empty()) {
        return 0;
    } else {
        uint32_t completeBytes = indices.back();
        indices.pop_back();
        return completeBytes;
    }
}


template <class DataFormat, class LockType, class FileChannel>
template <bool CheckEnd>
void VariableRecordStorePersist<DataFormat, LockType, FileChannel>::getManyImpl_DELETE(uint64_t start, uint64_t end, dep::ByteBuffer& buffer, std::vector<uint32_t>& indices, const std::false_type&) {
    if (m_recordCount == 0)
        throw dep::DEPException{"Session store is empty"};

    const uint64_t validStart = m_baseSequence;
    const uint64_t validEnd = m_baseSequence + m_recordCount - 1;
    
    if (start < validStart || start > validEnd)
        throw dep::DEPException{"Requested start sequence [" + std::to_string(start) + "] is invalid, valid Seq[" + std::to_string(validStart) + "] to [" + std::to_string(validEnd) + "]"};
    if (CheckEnd && (end < validStart || end > validEnd))
        throw dep::DEPException{"Requested end sequence [" + std::to_string(end) + "] is invalid, valid Seq[" + std::to_string(validStart) + "] to [" + std::to_string(validEnd) + "]"};
    if (CheckEnd && start > end)
        throw dep::DEPException{"End sequence [" + std::to_string(end) + "] is greater than start sequence [" + std::to_string(start) + "]"};

    uint64_t adjustStart = start - m_baseSequence;
    uint64_t adjustEnd = end - m_baseSequence;

    auto maxBytes = buffer.remaining();
    auto maxRecords = CheckEnd ? adjustEnd - adjustStart + 1 : m_recordCount - start + m_baseSequence;

    uint32_t readCount = 0;
    uint32_t byteCount = 0;
    uint32_t bytesReaded = 0;

    FileNumberType fileStart = 0;
    FileNumberType fileEnd = 0;
    DataOffsetType recordStart = 0;
    DataOffsetType recordEnd = 0;

    if (adjustStart == 0)
        m_pIndexFile->seekg(INDEX_HEADER_SIZE);
    else {
        m_pIndexFile->seekg( (adjustStart -1) * INDEX_RECORD_SIZE + INDEX_HEADER_SIZE);
        readIndexRecord(fileStart, recordStart);
    }

    readIndexRecord(fileEnd, recordEnd);
    if (fileStart != fileEnd) {
        recordStart = 0;
        fileStart = fileEnd;
    }

    auto recordLength = recordEnd - recordStart;
    auto startIndex = recordStart;
    auto endIndex = recordStart;

	//如果Buffer空间不够，进行扩容保证第一次有消息可以发送
	if (byteCount + recordLength > maxBytes) {
	 	buffer.ensureCapacity(byteCount + recordLength);
        maxBytes = buffer.remaining();
	}

    uint32_t posMessageStart = buffer.getPosition();

    bool isReadEnd = false;
    do {
        //  check invalid data file or invalid index file 
        if (recordLength == 0) {
            //throw dep::DEPException{"Session store is empty"};
            isReadEnd = true;
            goto lblReadData;
        }

        indices.emplace_back(byteCount);

        endIndex = recordEnd;
        recordStart = recordEnd;
        byteCount += recordLength;
        ++readCount;

        if (m_pIndexFile->eof()) {
            isReadEnd = true;
            goto lblReadData;
        }
        readIndexRecord(fileEnd, recordEnd);

        recordLength = recordEnd - recordStart;
        isReadEnd = readCount >= maxRecords || byteCount + recordLength > maxBytes;

        //  on new data file
        if (fileStart != fileEnd || isReadEnd) {
lblReadData:
            auto bytesToRead = endIndex - startIndex;
            if (bytesToRead > 0) {
                auto pFileHandle = acquireDataFile(fileStart);
                pFileHandle->seekg(startIndex);
                int64_t bytesReadedCurr = readFromDataFile(*pFileHandle, buffer, bytesToRead);
                releaseDataFile(fileStart);

                if (bytesReadedCurr > 0)
                    bytesReaded += bytesReadedCurr;

                if (bytesReadedCurr < bytesToRead) {
                    indices.emplace_back(byteCount);
                    uint32_t completeBytes = fixMessageOffsets(indices, bytesReaded);
                    uint32_t posComplete = posMessageStart + completeBytes;
                    buffer.setPosition(posComplete);
                    break;
                }
            }

            recordStart = 0;
            startIndex = 0;
            endIndex = 0;
            fileStart = fileEnd;
        }

    } while (!isReadEnd);

    if (m_pIndexFile->eof())
        m_pIndexFile->clear();
}

template <class DataFormat, class LockType, class FileChannel>
template <class OutputDataFormat>
inline uint32_t VariableRecordStorePersist<DataFormat, LockType, FileChannel>::getManyWithPrefixSuffix(uint64_t start, dep::ByteBuffer& buffer) {
    std::lock_guard<LockType> lock(m_lock);
    return getManyWithPrefixSuffixImpl<false, OutputDataFormat>(start, 0, buffer, std::is_same<DataFormat, OutputDataFormat>());
}

template <class DataFormat, class LockType, class FileChannel>
uint32_t VariableRecordStorePersist<DataFormat, LockType, FileChannel>::getManyWithPrefixSuffixProxy(uint64_t start, dep::ByteBuffer& buffer){
    return getManyWithPrefixSuffix<DataFormat>(start, buffer);
}

template <class DataFormat, class LockType, class FileChannel>
template <class OutputDataFormat>
uint32_t VariableRecordStorePersist<DataFormat, LockType, FileChannel>::getManyWithPrefixSuffix(uint64_t start, uint64_t end, dep::ByteBuffer& buffer) {
    std::lock_guard<LockType> lock(m_lock);
    return getManyWithPrefixSuffixImpl<true, OutputDataFormat>(start, end, buffer, std::is_same<DataFormat, OutputDataFormat>());
}
template <class DataFormat, class LockType, class FileChannel>
uint32_t VariableRecordStorePersist<DataFormat, LockType, FileChannel>::getManyWithPrefixSuffixProxy(uint64_t start, uint64_t end, dep::ByteBuffer& buffer){
    return getManyWithPrefixSuffix<DataFormat>(start, end, buffer);
}

template <class DataFormat, class LockType, class FileChannel>
template <bool CheckEnd, class OutputDataFormat>
inline uint32_t VariableRecordStorePersist<DataFormat, LockType, FileChannel>::getManyWithPrefixSuffixImpl(uint64_t start, uint64_t end, dep::ByteBuffer& buffer, const std::true_type&) {
    return getManyWithPrefixSuffixNoConversion<CheckEnd>(start, end, buffer);
}

template <class DataFormat, class LockType, class FileChannel>
template <bool CheckEnd, class OutputDataFormat>
inline uint32_t VariableRecordStorePersist<DataFormat, LockType, FileChannel>::getManyWithPrefixSuffixImpl(uint64_t start, uint64_t end, dep::ByteBuffer& buffer, const std::false_type&) {
    return getManyWithPrefixSuffixConversion<CheckEnd, OutputDataFormat>(start, end, buffer);
}

template <class DataFormat, class LockType, class FileChannel>
template <bool CheckEnd, class OutputFormat>
uint32_t VariableRecordStorePersist<DataFormat, LockType, FileChannel>::getManyWithPrefixSuffixConversion(uint64_t start, uint64_t end, dep::ByteBuffer& buffer){
    if (m_recordCount == 0)
        throw dep::DEPException{"Session store is empty"};

    const uint64_t validStart = m_baseSequence;
    const uint64_t validEnd = m_baseSequence + m_recordCount - 1;
    if (start < validStart || start > validEnd)
        throw dep::DEPException{"Requested start sequence [" + std::to_string(start) + "] is invalid, valid Seq[" + std::to_string(validStart) + "] to [" + std::to_string(validEnd) + "]"};
    if (CheckEnd && (end < validStart || end > validEnd))
        throw dep::DEPException{"Requested end sequence [" + std::to_string(end) + "] is invalid, valid Seq[" + std::to_string(validStart) + "] to [" + std::to_string(validEnd) + "]"};
    if (CheckEnd && start > end)
        throw dep::DEPException{"End sequence [" + std::to_string(end) + "] is greater than start sequence [" + std::to_string(start) + "]"};

    uint64_t adjustStart = start - m_baseSequence;
    uint64_t adjustEnd = end - m_baseSequence;

    uint32_t readCount = 0;
    auto currentSeq = adjustStart;
    auto endSeq = CheckEnd ? adjustEnd : m_recordCount - 1;

    FileNumberType fileStart = 0;
    FileNumberType fileEnd = 0;
    DataOffsetType readStart = 0;
    DataOffsetType readEnd = 0;

    if (adjustStart == 0)
        m_pIndexFile->seekg(INDEX_HEADER_SIZE);
    else {
        m_pIndexFile->seekg( (adjustStart -1) * INDEX_RECORD_SIZE + INDEX_HEADER_SIZE);
        readIndexRecord(fileStart, readStart);
    }

    readIndexRecord(fileEnd, readEnd);

    if (fileStart != fileEnd) {
        readStart = 0;
        fileStart = fileEnd;
    }

    auto dataStart = getDataStart(readStart);
    auto dataEnd = getDataEnd(readEnd);
    auto dataLength = dataEnd - dataStart;

    auto pFileHandle = acquireDataFile(fileEnd);

    while ((dataLength + getPrefixSize<OutputFormat>() + getSuffixSize<OutputFormat>()) <= buffer.remaining()) {
        putPrefix<OutputFormat>(dataLength, buffer);

        pFileHandle->seekg(dataStart);
        readFromDataFile(*pFileHandle, buffer, dataLength);

        putSuffix<OutputFormat>(dataLength, buffer);

        ++readCount;
        if (++currentSeq > endSeq)
            break;

        readStart = readEnd;
        readIndexRecord(fileEnd, readEnd);

        if (fileStart != fileEnd) {
            releaseDataFile(fileStart);
            pFileHandle = acquireDataFile(fileEnd);
            readStart = 0;
            fileStart = fileEnd;
        }

        dataStart = getDataStart(readStart);
        dataEnd = getDataEnd(readEnd);
        dataLength = dataEnd - dataStart;
    }

    if (m_pIndexFile->eof())
        m_pIndexFile->clear();

    return readCount;
}

template <class DataFormat, class LockType, class FileChannel>
uint32_t VariableRecordStorePersist<DataFormat, LockType, FileChannel>::getManyReady(uint64_t start, uint64_t end, dep::ByteBuffer& buffer, std::pair<uint64_t, uint64_t> &startEndPosition, uint64_t& expiredCount) {
    std::lock_guard<LockType> lock(m_lock);
    if (m_recordCount == 0)
        throw dep::DEPException{"Session store is empty"};

    const uint64_t validStart = m_baseSequence;
    const uint64_t validEnd = m_baseSequence + m_recordCount - 1;
    if (start < validStart || start > validEnd)
        throw dep::DEPException{"Requested start sequence [" + std::to_string(start) + "] is invalid, valid Seq[" + std::to_string(validStart) + "] to [" + std::to_string(validEnd) + "]"};
    if (end < validStart || end > validEnd)
        throw dep::DEPException{"Requested end sequence [" + std::to_string(end) + "] is invalid, valid Seq[" + std::to_string(validStart) + "] to [" + std::to_string(validEnd) + "]"};
    if (start > end)
        throw dep::DEPException{"End sequence [" + std::to_string(end) + "] is greater than start sequence [" + std::to_string(start) + "]"};
	
	expiredCount = 0;
    uint64_t adjustStart = start - m_baseSequence;
    uint64_t adjustEnd = end - m_baseSequence;


    uint32_t readCount = 0;

    auto endSeq = adjustEnd ;

    FileNumberType fileNumber = 0;
    FileNumberType nextFileNumber = 0;
    DataOffsetType startIndex = 0;
    DataOffsetType nextStartIndex = 0;
    TimestampType timestamp=0;
    MessageAttributeType attribute=0;
    bool isReady=0;
   // MessageMarkType nextMark=0;



    if (adjustStart == 0)
        m_pIndexFile->seekg(INDEX_HEADER_SIZE);
    else {
        m_pIndexFile->seekg( (adjustStart -1) * INDEX_RECORD_SIZE + INDEX_HEADER_SIZE);
        readIndexRecord(fileNumber, startIndex);
//        printf("pt1 fileNumber %u startIndex %u\n", fileNumber, startIndex);
    }

    /* this code section is for handling first message not equal to mark and still return the following
     * right now we will return nothing , and therefore it is useless
     * bool firstRead = true;
     * do {
     *    if (!firstRead) {
     *        fileNumber=nextFileNumber;
     *        startIndex = nextStartIndex;
     *        currentSeq++;
     *    }
     *    firstRead=false;
     *
     *    readIndexRecord(nextFileNumber, nextStartIndex,timestamp, attribute, mark);
     *    printf("pt2 fileNumber %u startIndex %u mark %u\n", nextFileNumber, nextStartIndex, mark);
     * } while (mark !=expectedMark && currentSeq <= adjustEnd);
    */

    readIndexRecord(nextFileNumber, nextStartIndex,timestamp, attribute);
    isReady = this->getIsSequenceReady(adjustStart);
    //printf("pt3 fileNumber %u startIndex %u\n", fileNumber, startIndex);
    //printf("pt3 fileNumber %u startIndex %u mark %u adjustStart %u\n", nextFileNumber, nextStartIndex, mark, adjustStart);


    if (!isReady) {
       // printf("pt3 break\n");
        return 0;
    }


    if (nextFileNumber != fileNumber) {
        startIndex = 0;
        fileNumber = nextFileNumber;
    }

    auto maxBytes = buffer.remaining();
    auto firstRecordLength = nextStartIndex - startIndex;
	if (firstRecordLength > maxBytes) {
		buffer.ensureCapacity(firstRecordLength);
        maxBytes = buffer.remaining();
	}

    auto maxEndIndex = startIndex + maxBytes;
    auto endIndex = startIndex;
    auto currentSeq = adjustStart;
   // printf("pt3 currentSeq %u endSeq %u\n", currentSeq, endSeq);
    while (nextStartIndex <= maxEndIndex && currentSeq <= endSeq) {
        //printf("pt4 currentSeq %u endSeq %u\n", currentSeq, endSeq);
        endIndex = nextStartIndex;

        ++readCount;

        if (m_pIndexFile->eof()) {
            m_pIndexFile->clear();
            break;
        }
        ++currentSeq;

        readIndexRecord(nextFileNumber, nextStartIndex, timestamp, attribute);
        isReady = this->getIsSequenceReady(currentSeq);

        //printf("pt4 fileNumber %u startIndex %u mark %u currentSeq %u\n", nextFileNumber, nextStartIndex, mark, currentSeq);


        if (nextFileNumber != fileNumber || !isReady) {
            auto bytesToRead = endIndex - startIndex;
            auto pFileHandle = acquireDataFile(fileNumber);
            pFileHandle->seekg(startIndex);

            readFromDataFile(*pFileHandle, buffer, bytesToRead);
            releaseDataFile(fileNumber);

            startIndex = 0;
            endIndex = 0;
            fileNumber = nextFileNumber;
            maxEndIndex = buffer.remaining();

            if (!isReady)
                break;

        }
    }

    // Read data into dep::byte buffer
    auto bytesToRead = endIndex - startIndex;

    if (m_pIndexFile->eof())
        m_pIndexFile->clear();

    // this part is for the last file contain more than 1 message, so it cannot be read in the above file read part
    if (bytesToRead > 0 && bytesToRead <= buffer.remaining()) {
        auto pFileHandle = acquireDataFile(fileNumber);
        pFileHandle->seekg(startIndex);
        readFromDataFile(*pFileHandle, buffer, bytesToRead);
        releaseDataFile(fileNumber);
    }
    startEndPosition.first = start;
    startEndPosition.second = start + readCount - 1;
    return readCount;
}

template <class DataFormat, class LockType, class FileChannel>
template <bool CheckEnd>
uint32_t VariableRecordStorePersist<DataFormat, LockType, FileChannel>::getManyWithPrefixSuffixNoConversion(uint64_t start, uint64_t end, dep::ByteBuffer& buffer) {
    if (m_recordCount == 0)
        throw dep::DEPException{"Session store is empty"};

    const uint64_t validStart = m_baseSequence;
    const uint64_t validEnd = m_baseSequence + m_recordCount - 1;
    if (start < validStart || start > validEnd)
        throw dep::DEPException{"Requested start sequence [" + std::to_string(start) + "] is invalid, valid Seq[" + std::to_string(validStart) + "] to [" + std::to_string(validEnd) + "]"};
    if (CheckEnd && (end < validStart || end > validEnd))
        throw dep::DEPException{"Requested end sequence [" + std::to_string(end) + "] is invalid, valid Seq[" + std::to_string(validStart) + "] to [" + std::to_string(validEnd) + "]"};
    if (CheckEnd && start > end)
        throw dep::DEPException{"End sequence [" + std::to_string(end) + "] is greater than start sequence [" + std::to_string(start) + "]"};

    uint64_t adjustStart = start - m_baseSequence;
    uint64_t adjustEnd = end - m_baseSequence;


    uint32_t readCount = 0;

    auto maxBytes = buffer.remaining();
    auto endSeq = CheckEnd ? adjustEnd : m_recordCount - 1;

    FileNumberType fileNumber = 0;
    FileNumberType nextFileNumber = 0;
    DataOffsetType startIndex = 0;
    DataOffsetType nextStartIndex = 0;

    if (adjustStart == 0)
        m_pIndexFile->seekg(INDEX_HEADER_SIZE);
    else {
        m_pIndexFile->seekg( (adjustStart -1) * INDEX_RECORD_SIZE + INDEX_HEADER_SIZE);
        readIndexRecord(fileNumber, startIndex);
    }

    readIndexRecord(nextFileNumber, nextStartIndex);

    if (nextFileNumber != fileNumber) {
        startIndex = 0;
        fileNumber = nextFileNumber;
    }

    auto maxEndIndex = startIndex + maxBytes;
    auto endIndex = startIndex;
    auto currentSeq = adjustStart;

#ifdef _DEBUG_LOG
std::cout
<< "getManyWithPrefixSuffixNoConversion fileNumber=" << fileNumber
<< " startIndex=" << startIndex
<< " nextFileNumber=" << nextFileNumber
<< " nextStartIndex=" << nextStartIndex
<< " currentSeq=" << currentSeq
<< " maxEndIndex=" << maxEndIndex
<< " endSeq=" << endSeq
<< std::endl;
#endif

    while (nextStartIndex <= maxEndIndex && currentSeq <= endSeq) {

#ifdef _DEBUG_LOG
std::cout
<< "getManyWithPrefixSuffixNoConversion2 fileNumber=" << fileNumber
<< " startIndex=" << startIndex
<< " nextFileNumber=" << nextFileNumber
<< " nextStartIndex=" << nextStartIndex
<< " currentSeq=" << currentSeq
<< " maxEndIndex=" << maxEndIndex
<< " endSeq=" << endSeq
<< std::endl;
#endif
        endIndex = nextStartIndex;

        ++readCount;

        if (m_pIndexFile->eof()) {
            m_pIndexFile->clear();
            break;
        }

        readIndexRecord(nextFileNumber, nextStartIndex);

        ++currentSeq;

#ifdef _DEBUG_LOG
std::cout
<< "getManyWithPrefixSuffixNoConversion2b fileNumber=" << fileNumber
<< " startIndex=" << startIndex
<< " nextFileNumber=" << nextFileNumber
<< " nextStartIndex=" << nextStartIndex
<< " currentSeq=" << currentSeq
<< " maxEndIndex=" << maxEndIndex
<< " endSeq=" << endSeq
<< std::endl;
#endif
        if (nextFileNumber != fileNumber) {
            auto bytesToRead = endIndex - startIndex;
            auto pFileHandle = acquireDataFile(fileNumber);
            pFileHandle->seekg(startIndex);

#ifdef _DEBUG_LOG
std::cout
<< "getManyWithPrefixSuffixNoConversion3 fileNumber=" << fileNumber
<< " startIndex=" << startIndex
<< " nextFileNumber=" << nextFileNumber
<< " nextStartIndex=" << nextStartIndex
<< " buffer.remaining()=" << buffer.remaining()
<< " bytesToRead=" << bytesToRead
<< std::endl;
#endif
            readFromDataFile(*pFileHandle, buffer, bytesToRead);
            releaseDataFile(fileNumber);

            startIndex = 0;
			endIndex = 0;
            fileNumber = nextFileNumber;
            maxEndIndex = buffer.remaining();
        }
    }

    // Read data into dep::byte buffer
    auto bytesToRead = endIndex - startIndex;

#ifdef _DEBUG_LOG
std::cout
<< "getManyWithPrefixSuffixNoConversion4 fileNumber=" << fileNumber
<< " startIndex=" << startIndex
<< " nextFileNumber=" << nextFileNumber
<< " nextStartIndex=" << nextStartIndex
<< " startIndex=" << startIndex
<< " endIndex=" << endIndex
<< " buffer.remaining()=" << buffer.remaining()
<< " bytesToRead=" << bytesToRead
<< std::endl;
#endif

    if (m_pIndexFile->eof())
        m_pIndexFile->clear();

    if (bytesToRead > 0 && bytesToRead <= buffer.remaining()) {
        auto pFileHandle = acquireDataFile(fileNumber);
        pFileHandle->seekg(startIndex);
        readFromDataFile(*pFileHandle, buffer, bytesToRead);
        releaseDataFile(fileNumber);
    }

    return readCount;
}

template <class DataFormat, class LockType, class FileChannel>
void VariableRecordStorePersist<DataFormat, LockType, FileChannel>::readSingleMessageBuffer(uint64_t seq, dep::ByteBuffer& buffer) {
    std::lock_guard<LockType> lock(m_lock);

    if (m_recordCount == 0)
        throw dep::DEPException{"get(): Session store is empty"};

    const uint64_t validStart = m_baseSequence;
    const uint64_t validEnd = m_baseSequence + m_recordCount - 1;
    if (seq < validStart || seq > validEnd)
        throw dep::DEPException{"get(): Requested sequence [" + std::to_string(seq) + "] is invalid, valid Seq[" + std::to_string(validStart) + "] to [" + std::to_string(validEnd) + "]"};

    uint64_t adjustedSeq = seq - m_baseSequence;

    FileNumberType fileStart = 0;
    FileNumberType fileEnd = 0;
    DataOffsetType recordStart = 0;
    DataOffsetType recordEnd = 0;


    if (adjustedSeq == 0)
        m_pIndexFile->seekg(INDEX_HEADER_SIZE);
    else {
        m_pIndexFile->seekg( (adjustedSeq -1) * INDEX_RECORD_SIZE + INDEX_HEADER_SIZE);
        readIndexRecord(fileStart, recordStart);
    }

    readIndexRecord(fileEnd, recordEnd);

    // Clear the eof flag in case of reading the last record
    if (m_pIndexFile->eof())
        m_pIndexFile->clear();

    if (fileStart != fileEnd)
        recordStart = 0;

    auto dataStart = getDataStart(recordStart);
    auto dataEnd = getDataEnd(recordEnd);
    auto dataLength = dataEnd - dataStart;
    if (dataLength) {
        auto readLength = std::min(dataLength, static_cast<uint64_t>(buffer.remaining()));
        auto pFileHandle = acquireDataFile(fileEnd);
        pFileHandle->seekg(dataStart);
        readFromDataFile(*pFileHandle, buffer, readLength);
        releaseDataFile(fileEnd);
    }

}

template <class DataFormat, class LockType, class FileChannel>
uint32_t VariableRecordStorePersist<DataFormat, LockType, FileChannel>::getRecordSizeWithPrefixSuffix(uint64_t seq) {
    std::lock_guard<LockType> lock(m_lock);

    if (m_recordCount == 0)
        throw dep::DEPException{"getRecordSizeWithPrefixSuffix(): Session store is empty"};

    const uint64_t validStart = m_baseSequence;
    const uint64_t validEnd = m_baseSequence + m_recordCount - 1;
    if (seq < validStart || seq > validEnd)
        throw dep::DEPException{"getRecordSizeWithPrefixSuffix(): Requested sequence [" + std::to_string(seq) + "] is invalid, valid Seq[" + std::to_string(validStart) + "] to [" + std::to_string(validEnd) + "]"};

    uint64_t adjustedSeq = seq - m_baseSequence;


    FileNumberType fileStart = 0;
    FileNumberType fileEnd = 0;
    DataOffsetType recordStart = 0;
    DataOffsetType recordEnd = 0;

    if (adjustedSeq == 0)
         m_pIndexFile->seekg(INDEX_HEADER_SIZE);
     else {
         m_pIndexFile->seekg( (adjustedSeq -1) * INDEX_RECORD_SIZE + INDEX_HEADER_SIZE);
         readIndexRecord(fileStart, recordStart);
     }

    readIndexRecord(fileEnd, recordEnd);

    uint32_t recordSize{0};
    if (fileStart != fileEnd) {
        recordSize = recordEnd;
    } else {
        recordSize = recordEnd - recordStart;
    }

#ifdef _DEBUG_LOG
std::cout
<< "getRecordSizeWithPrefixSuffix"
<< " adjustedSeq=" << adjustedSeq
<< " fileStart=" << fileStart
<< " recordStart=" << recordStart
<< " fileEnd=" << fileEnd
<< " recordEnd=" << recordEnd
<< " recordSize=" << recordSize
<< std::endl;
#endif

    if (m_pIndexFile->eof())
        m_pIndexFile->clear();

    return recordSize;
}

template <class DataFormat, class LockType /*= dep::NoLock*/, class FileChannel /*= dep::BufferedFileStream*/>
std::tuple<uint32_t, uint64_t> VariableRecordStorePersist<DataFormat, LockType, FileChannel>::addToDataFile(dep::ByteBuffer& buffer){
	if (buffer.remaining() > m_maxFileSize)
		throw dep::DEPException{ "Data to write [" + std::to_string(buffer.remaining()) + "] exceeds maximum file size [" + std::to_string(m_maxFileSize) + "]" };

	const auto dataLength = buffer.remaining();
	const auto recordLength = getPrefixSize() + dataLength + getSuffixSize();
	if (recordLength > m_maxFileSize)
		throw dep::DEPException{ "Data to add is too large" };

	const auto currentFileSize = m_pDataFile->tellp();
	if (recordLength + currentFileSize > m_maxFileSize) {
		const auto dataFileName = getDataFileName(m_dataFileCount++);

		m_pDataFile->close();
		m_pDataFile->open(dataFileName, std::ios_base::in | std::ios_base::out | std::ios_base::trunc);
		if (!m_pDataFile->is_open())
			throw DEPStorageException{ "Fail to open new data file: " + dataFileName };
	}

    // DEBUG_PRINT("storeName<%s> write data<%u,%u,%u> \n", m_storeName.c_str(), getPrefixSize(), dataLength, getSuffixSize());
	if (getPrefixSize()) {
		m_prefixBuffer.clear();
		putPrefix(dataLength, m_prefixBuffer);
		m_prefixBuffer.flip();

		writeToDataFile(m_prefixBuffer);
	}

	if (dataLength)
		writeToDataFile(buffer);

	if (getSuffixSize()) {
		m_suffixBuffer.clear();
		putSuffix(dataLength, m_suffixBuffer);
		m_suffixBuffer.flip();

		writeToDataFile(m_suffixBuffer);
	}
	m_pDataFile->flush();
    return std::make_tuple(m_dataFileCount - 1, m_pDataFile->tellp());
}


// template <class DataFormat, class LockType, class FileChannel>
// void VariableRecordStorePersist<DataFormat, LockType, FileChannel>::add(dep::ByteBuffer& buffer, const std::chrono::high_resolution_clock::time_point& timestamp) {
//     std::lock_guard<LockType> lock(m_lock);
//     auto addDataFileRes = addToDataFile(buffer);
//
//     addIndexRecord(static_cast<FileNumberType>(std::get<0>(addDataFileRes)),
//         static_cast<DataOffsetType>(std::get<1>(addDataFileRes)),
//         m_pInfoFile->tellp(),
//         timestamp.time_since_epoch().count());
//
//     m_pIndexFile->flush();
//     ++m_recordCount;
//
// }


// template <class DataFormat, class LockType /*= dep::NoLock*/, class FileChannel /*= dep::BufferedFileStream*/>
// uint64_t VariableRecordStorePersist<DataFormat, LockType, FileChannel>::addToInfoFile(const std::unordered_map<std::string, std::string>& attributes){
//     AtrributeStringLengthType attributeCount = static_cast<AtrributeStringLengthType>(attributes.size());
//     m_propertiseMessage.clear();
// 	m_propertiseMessage << attributeCount;
// 
// 	//FileRecordUtil<dep::FsyncFileStream>::writeToFile(*m_pInfoFile, attributeCount);
//     for (auto& kv : attributes) {
// 		m_propertiseMessage << (AtrributeStringLengthType)kv.first.size();
// 		m_propertiseMessage.put((const dep::byte*)kv.first.c_str(), kv.first.length());
// 
// 		m_propertiseMessage << (AtrributeStringLengthType)kv.second.size();
// 		m_propertiseMessage.put((const dep::byte*)kv.second.c_str(), kv.second.length());
// 
// // 		FileRecordUtil<dep::FsyncFileStream>::writeToFile(*m_pInfoFile, (AtrributeStringLengthType)kv.first.size());
// // 		FileRecordUtil<dep::FsyncFileStream>::writeToFile(*m_pInfoFile, kv.first.c_str(), kv.first.length());
// //
// // 		FileRecordUtil<dep::FsyncFileStream>::writeToFile(*m_pInfoFile, (AtrributeStringLengthType)kv.second.size());
// // 		FileRecordUtil<dep::FsyncFileStream>::writeToFile(*m_pInfoFile, kv.second.c_str(), kv.second.length());
//     }
// 	m_propertiseMessage.flip();
// 	FileRecordUtil<dep::FsyncFileStream>::writeToFile(*m_pInfoFile, (const char*)m_propertiseMessage.getArray(), m_propertiseMessage.remaining());
//     m_pInfoFile->flush();
//     return m_pInfoFile->tellp();
// }


/*
 * add:
 *
 * if message is a transaction Message:
 *
 * Index attribute: has transaction Flag.
 */
template <class DataFormat, class LockType /*= dep::NoLock*/, class FileChannel /*= dep::BufferedFileStream*/>
uint64_t VariableRecordStorePersist<DataFormat, LockType, FileChannel>::add(RecordStoreAddMessageParam &addParam)
	//dep::ByteBuffer& buffer,
	//bool isRemoteMessage/* = false*/,
    //const std::shared_ptr<std::unordered_map<std::string, std::string>>& messageProperties/* = nullptr*/,
	//bool isDeletedMessage/* = false*/,
    //const std::chrono::high_resolution_clock::time_point& timestamp /*= std::chrono::high_resolution_clock::now()*/)
{

    // DEBUG_PRINT("storeName<%s> add message to session store count<%d>.\n%s\n", m_storeName.c_str(), addParam.m_buffer->remaining(), util::hexdumpLimit((const char*)addParam.m_buffer->getArray()+addParam.m_buffer->getPosition(), addParam.m_buffer->remaining(), 500).c_str());

	//PROF_NF_BEGIN("addToDataFile");
	std::lock_guard<LockType> lock(m_lock);
    if (!this->testGroupMessage(addParam.m_msgAgent ? &addParam.m_msgAgent->m_header.getVariantMap() : nullptr))
    {
        DEBUG_PRINT("VariableRecordStorePersist::add got error groupMessage. storeName<%s> \n", m_storeName.c_str());
        return m_recordCount;
    }

	auto addDataFileRes = addToDataFile(*addParam.m_buffer);
	//PROF_NF_END("addToDataFile");
    // DEBUG_PRINT("storeName<%s> write data result<%u, %u> \n", m_storeName.c_str(), static_cast<uint32_t>(std::get<0>(addDataFileRes)), static_cast<uint32_t>(std::get<1>(addDataFileRes)));

	int64_t currentSeq = m_baseSequence + m_recordCount;

	MessageAttributeType msgAttrMask = 0;
	if (addParam.m_isRemoteMessage)
		msgAttrMask |= REMOTE_MESSAGE_MASKER;
	if (addParam.m_isDeletedMessage)
		msgAttrMask |= DELETED_MESSAGE_MASKER;
	
	if (addParam.m_msgAgent && addParam.m_msgAgent->getTransactionID())
        RecordStoreDef::setMessageTransactionState(msgAttrMask, RecordStoreDef::MESSAGE_TRANSACTION_COMMIT);

	MessageAttributeType attribute = this->addMessageAttribute(currentSeq, msgAttrMask, addParam.m_msgAgent ? &addParam.m_msgAgent->m_header.getVariantMap() : nullptr);
    this->checkLevelInfo(currentSeq, addParam.getPriority());
	//MessageAttributeType attribute = this->addMessageAttribute(addParam.m_isRemoteMessage, addParam.m_msgAgent->m_messageProperty, addParam.m_isDeletedMessage, currentSeq);


	/*
    MessageAttributeType attribute = DEFAULT_MESSAGE_ATTRIBUTE;
    if (isRemoteMessage) {
        attribute |= REMOTE_MESSAGE_MASKER;
        m_remoteSequenceSet.insert(currentSeq);
	}
	if (isDeletedMessage) {
		attribute |= DELETED_MESSAGE_MASKER;
		m_deletedSequenceSet.insert(currentSeq);
	}
    if (messageProperties && !messageProperties->empty()) {
		attribute |= HAS_ATTRIBUTE_MESSAGE_MASKER;
        addToInfoFile(*messageProperties);
        std::lock_guard<LockType> lock(m_messagePropertiesLock);
        m_messageProperties[currentSeq] = messageProperties;
        m_lastHasPropertiesMessageSeq = currentSeq;
	}

	if (!m_notDeleteSequenceSet.empty()) {
		auto noDeleteSeqRes = m_notDeleteSequenceSet.find(currentSeq);
		if (noDeleteSeqRes != m_notDeleteSequenceSet.end()) {
			attribute |= DELETED_MESSAGE_MASKER;
			m_notDeleteSequenceSet.erase(noDeleteSeqRes);
			m_deletedSequenceSet.insert(currentSeq);
		}
	}*/
	addIndexRecord(static_cast<FileNumberType>(std::get<0>(addDataFileRes)),
        static_cast<DataOffsetType>(std::get<1>(addDataFileRes)),
        addParam.m_timestamp.time_since_epoch().count(),
        attribute);

	++m_recordCount;
	m_pIndexFile->flush();
	return m_recordCount;
}

template <class DataFormat, class LockType, class FileChannel>
bool VariableRecordStorePersist<DataFormat, LockType, FileChannel>::isStoreExists(const std::string& storePath, const std::string& storeName) {
    auto path = storePath;
    if (path.back() != '/')
        path += "/";
    const auto indexFileName = path + storeName + getIndexExtension();
    return dep::FileUtility::isFileExist(indexFileName);
}

template <class DataFormat, class LockType, class FileChannel>
void VariableRecordStorePersist<DataFormat, LockType, FileChannel>::sync() {
    m_pDataFile->flush();
    m_pDataFile->sync();
    m_pIndexFile->flush();
    m_pIndexFile->sync();
}

/*
template <class DataFormat, class LockType, class FileChannel>
void VariableRecordStorePersist<DataFormat, LockType, FileChannel>::setSequenceReady(uint64_t seq) {
    std::lock_guard<LockType> lock(m_lock);
    uint64_t adjustedSeq = seq - m_baseSequence;
    //printf("setMark seq %u , mark %u\n", adjustedSeq, mark);
    m_notReadySequenceSet.erase(adjustedSeq);
}

template <class DataFormat, class LockType, class FileChannel>
void VariableRecordStorePersist<DataFormat, LockType, FileChannel>::setSequenceNotReady(uint64_t seq) {
    std::lock_guard<LockType> lock(m_lock);
    uint64_t adjustedSeq = seq - m_baseSequence;
    //printf("setMark seq %u , mark %u\n", adjustedSeq, mark);
    m_notReadySequenceSet.insert(adjustedSeq);
}

template <class DataFormat, class LockType, class FileChannel>
void VariableRecordStorePersist<DataFormat, LockType, FileChannel>::setAllSequenceReady() {
    m_notReadySequenceSet.clear();
}*/

template <class DataFormat, class LockType, class FileChannel>
void VariableRecordStorePersist<DataFormat, LockType, FileChannel>::remove() {
    const auto indexFileName = m_storePath + m_storeName + getIndexExtension();
    std::remove(indexFileName.c_str());

    for (uint32_t i=0; i<m_dataFileCount; ++i) {
        const auto dataFileName = getDataFileName(i);
        std::remove(dataFileName.c_str());
    }
}


template <class DataFormat, class LockType /*= dep::NoLock*/, class FileChannel /*= dep::BufferedFileStream*/>
void VariableRecordStorePersist<DataFormat, LockType, FileChannel>::readIndexRecordAttribute(MessageAttributeType &attr/*, InfoOffsetType& infoFileOffset*/) {
	m_pIndexFile->ignore(sizeof(FileNumberType));
	m_pIndexFile->ignore(sizeof(DataOffsetType));
    m_pIndexFile->ignore(sizeof(InfoOffsetType));
	m_pIndexFile->ignore(sizeof(TimestampType));
	readFromIndexFile(attr);
	m_pIndexFile->ignore(sizeof(MessageMarkType));
	m_pIndexFile->ignore(sizeof(ReservedByteType) * 2);
}


// template <class DataFormat, class LockType /*= dep::NoLock*/, class FileChannel /*= dep::BufferedFileStream*/>
// std::shared_ptr<VariantMap> VariableRecordStorePersist<DataFormat, LockType, FileChannel>::readAttributeRecord(uint64_t startPos, uint32_t length){
//     if (length == 0) {
//         return std::make_shared <VariantMap>();
//     }
// 
//     m_pInfoFile->seekg(startPos, std::ios_base::beg);
//     dep::ByteBuffer buffer(length);
//     m_pInfoFile->read(buffer, length);
//     buffer.flip();
// 	AtrributeStringLengthType size, stringLength;
//     buffer >> size;
//     auto res = std::make_shared <VariantMap>();
//     std::string key, value;
//     //for (uint32_t i = 0; i < size; ++i) {
//     //    buffer >> stringLength;
//     //    buffer.get(key, stringLength);
// 	//	buffer >> stringLength;
// 	//	buffer.get(value, stringLength);
//     //    (*res)[key] = value;
//     //}
//     return res;
// }

template <class DataFormat, class LockType /*= dep::NoLock*/, class FileChannel /*= dep::BufferedFileStream*/>
void VariableRecordStorePersist<DataFormat, LockType, FileChannel>::rebuildInfo()
{
    rebuildInfoFromIndex();
    rebuildAttributeInfo();
}

template <class DataFormat, class LockType /*= dep::NoLock*/, class FileChannel /*= dep::BufferedFileStream*/>
void VariableRecordStorePersist<DataFormat, LockType, FileChannel>::rebuildInfoFromIndex(){
    m_pIndexFile->seekg(0, std::ios_base::end);
	auto indexFileLength = m_pIndexFile->tellg();
	if (indexFileLength > INDEX_HEADER_SIZE) {
        std::lock_guard<decltype(m_lock)> lock(m_lock);
        // std::lock_guard<LockType> lock(m_messagePropertiesLock);
		uint64_t readPos = INDEX_HEADER_SIZE;
		m_pIndexFile->seekg(readPos, std::ios_base::beg);
		uint64_t curSequence = m_baseSequence;
        //bool isDeleted{ false };
        //bool hasAttribute{ false };
        //bool isRemote{ false };
        //InfoOffsetType infoFileOffset{ 0 };
        //InfoOffsetType lastInfoFileOffset = infoFileOffset;
		while (readPos < indexFileLength) {
			//  TODO: HERE, Index文件每个Session都有一个的。但是此类本身就是Session相关的Store。奇怪的是consumerAck也是按照Session来的。
			MessageAttributeType attribute = 0;
            readIndexRecordAttribute(attribute);

			if (attribute & DELETED_MESSAGE_MASKER) {
				m_deletedSequenceSet.insert(curSequence);
			}
			// if (hasAttribute) {
            //     m_messageProperties[curSequence] = readAttributeRecord(lastInfoFileOffset, (infoFileOffset - lastInfoFileOffset));
			// 	m_lastHasPropertiesMessageSeq = curSequence;
			// }

            if (attribute & REMOTE_MESSAGE_MASKER) {
                m_remoteSequenceSet.insert(curSequence);
            }

			auto transState = RecordStoreDef::getMessageTransactionState(attribute);
			if (transState) {
				m_transactionSequences.emplace(curSequence, transState);
			}

			++curSequence;
			readPos += INDEX_RECORD_SIZE;
            //lastInfoFileOffset = infoFileOffset;
		}
	}
}


template <class DataFormat, class LockType /*= dep::NoLock*/, class FileChannel /*= dep::BufferedFileStream*/>
bool VariableRecordStorePersist<DataFormat, LockType, FileChannel>::parseSessionData(dep::ByteBuffer &buffer, VariantMap &vmap, uint8_t &level)
{
    int tempStartPos = buffer.getPosition(); 
    uint32_t byteWritten =0;

    // static int32_t index = 0;
    // printf("index<%d>,\n %s \n", index++, dep::StringUtility::hexdump((const char*)buffer.getArray() + buffer.getPosition(), 120).c_str());

    bool back = true;
    do
    {
        if (getPrefixSize())
        {
            if (buffer.remaining() < getPrefixSize()) break;

            buffer.setPosition(buffer.getPosition() + getPrefixSize());
        }

        if (!util::MessageUtil::parseStoreData(buffer, nullptr, nullptr, nullptr, &byteWritten, true))
        {
            break; 
        }

        if (buffer.remaining() < byteWritten)
        {
            break;
        }
        auto endPos = buffer.getPosition() + byteWritten; 

        util::LightmqMessageHead head;
        head.parseOnBroker(buffer, 0, true);

        level = head.getPriority();
        vmap.swap(head.getVariantMap());
        buffer.setPosition(endPos);
        
        if (getSuffixSize())
        {
            if (buffer.remaining() < getSuffixSize()) break;
            buffer.setPosition(buffer.getPosition() + getSuffixSize());
        }
    
        back = false;
    } while(0);

    if (back)
    {
        buffer.setPosition(tempStartPos);
        buffer.compact();
        return false;   
    }

    return true;
}

template <class DataFormat, class LockType /*= dep::NoLock*/, class FileChannel /*= dep::BufferedFileStream*/>
void VariableRecordStorePersist<DataFormat, LockType, FileChannel>::rebuildAttributeInfo()
{
    if ( 0 == m_recordCount ) return ;

    //dep::ByteBuffer dataBuf(32*1024*1024);
    //FIXME: 大消息有问题，临时改成200M，待解决
    dep::ByteBuffer dataBuf(200 * 1024 * 1024);
    std::shared_ptr<VariantMap> vmapPtr(new VariantMap);

    int64_t recordCount = 0;
    for (FileNumberType i=0; i<m_dataFileCount; ++i) {
        
        auto f = acquireDataFile(i);
        
        if (nullptr == f) return ;

        f->seekg(0, std::ios::beg);
        dataBuf.clear();

        int64_t fileSize = 0;
        int64_t count = 0;

        while (!f->eof())
        {
            f->read(dataBuf, dataBuf.remaining());
            dataBuf.flip();

            fileSize += dataBuf.remaining();
            count++;

            printf("current handled fileSize: %ld, count: %ld\n", fileSize, count);
            
            uint64_t sequence = 0;
            uint8_t level = 0;
            while( parseSessionData(dataBuf, *vmapPtr, level))
            {
                sequence = m_baseSequence + recordCount;
                if (!vmapPtr->empty())
                {
                    m_messageProperties[sequence] = vmapPtr;
                    this->testAndAddGroupInfo(sequence, vmapPtr.get());

                    vmapPtr.reset(new VariantMap); 
                }
                this->initRecordLevel(sequence, level);
                ++recordCount;
            }
        }
        f->clear();
        releaseDataFile(i);
    }

    return ;
}

template <class DataFormat, class LockType, class FileChannel>
void VariableRecordStorePersist<DataFormat, LockType, FileChannel>::init(bool append, uint32_t referenceTime) {
    const auto indexFileName = m_storePath + m_storeName + getIndexExtension();
	// const auto infoFileName = m_storePath + m_storeName + getInfoExtension();
    auto indexFileExists = dep::FileUtility::isFileExist(indexFileName);

    const auto firstDataFileName = getDataFileName(0);
    auto dataFileExists = dep::FileUtility::isFileExist(firstDataFileName);

    // Simple checking to see if store is corrupted
    if (indexFileExists && !dataFileExists)
        throw dep::DEPException{"Index file [" + indexFileName + "] exists but no data file in path [" + m_storePath + "]"};

    if (!indexFileExists && dataFileExists)
        throw dep::DEPException{"Data file exists in path [" + m_storePath + "] but no index file [" + indexFileName + "]"};

	//init index file
    auto openMode = std::ios_base::in | std::ios_base::out | (append && indexFileExists && dataFileExists ? std::ios_base::ate : std::ios_base::trunc);
    m_pIndexFile->open(indexFileName, openMode);
    if (!m_pIndexFile->is_open())
        throw dep::DEPException{"Fail to open index file: " + indexFileName};

    auto indexFileLength = m_pIndexFile->tellp();
    if (indexFileLength == 0) {
        // Generate content for new index file
        generateIndexFileHeader(VERSION, referenceTime);
    }
    else {
        // Get record count for appending to existing store
        m_recordCount = (indexFileLength - INDEX_HEADER_SIZE) / INDEX_RECORD_SIZE;
        if ((m_recordCount * INDEX_RECORD_SIZE + INDEX_HEADER_SIZE) != static_cast<uint64_t>(indexFileLength))
            throw dep::DEPException{"Corrupted index file: " + indexFileName};


        // Get reference time
        uint32_t version;
        uint32_t referenceTime;
        m_pIndexFile->seekg(0, std::ios_base::beg);
        readFromIndexFile(version);
        readFromIndexFile(referenceTime);
        initReferenceTime(referenceTime);

        /**
         * Check history data files for append mode
         * 1. Read last record in index file
         * 2. Get file count from the last record
         * 3. Check all history data files exists based on the record count
         */
        const auto indexRecordSize = static_cast<decltype(indexFileLength)>(INDEX_RECORD_SIZE);
        if (append && indexFileLength > INDEX_HEADER_SIZE) {
            FileNumberType lastFileNumber = 0;

            m_pIndexFile->seekg(indexFileLength - indexRecordSize);
            readFromIndexFile(lastFileNumber);

            for (FileNumberType i=0; i<lastFileNumber; ++i) {
                const auto dataFileName = getDataFileName(i);
                if (!dep::FileUtility::isFileExist(dataFileName))
                    throw dep::DEPException{"Missing data file: " + dataFileName};
            }

            m_dataFileCount = lastFileNumber + 1;
        }


        /**
         * Check every data file size is correct
         */


        dep::ByteBuffer indexFileBuffer(indexRecordSize * 10000);
        uint32_t i =0 ;
        FileNumberType oldFileNumber = 0;
        DataOffsetType oldFileOffset = 0;
        m_pIndexFile->seekg(INDEX_HEADER_SIZE); // we start to read after header

        while (INDEX_HEADER_SIZE + indexRecordSize* i < indexFileLength) {
            m_pIndexFile->read(indexFileBuffer, indexFileBuffer.remaining());
            indexFileBuffer.flip();

            if (indexFileBuffer.remaining() < indexRecordSize) {
                throw dep::DEPException("Index file not long enough, maybe it is corrupted, storeName:" + m_storeName);
            }


            indexFileBuffer.setMark();

            uint32_t bufferSize = indexFileBuffer.remaining();
            uint32_t lastRecordPos = bufferSize - (bufferSize % indexRecordSize) - indexRecordSize;

       //     printf("read buffer size = %u lastRecordPos =%u\n\n", (unsigned)bufferSize, (unsigned)lastRecordPos);

            indexFileBuffer.setPosition(lastRecordPos);
            FileNumberType lastRecordfileNumber = 0;
            DataOffsetType lastRecordfileOffset = 0;
            indexFileBuffer >> lastRecordfileNumber;
            indexFileBuffer >> lastRecordfileOffset;
       //     printf("lastRecordFileNumber = %u lastRecordfileOffset =%llu i=%u\n\n", (unsigned)lastRecordfileNumber, (unsigned long long)lastRecordfileOffset, (unsigned)i);

            indexFileBuffer.reset();

            bool compareToLastRecord=true;
            while (indexFileBuffer.remaining() >= indexRecordSize) {
                FileNumberType fileNumber = 0;
                DataOffsetType fileOffset = 0;
                indexFileBuffer >> fileNumber;
                indexFileBuffer >> fileOffset;
              //  printf("currentFileNumber=%u currentFileOffset=%llu\n", (unsigned)fileNumber, (unsigned long long)fileOffset);
                // set next record position
                indexFileBuffer.setPosition(indexFileBuffer.getPosition() + indexRecordSize - sizeof(fileNumber) - sizeof(fileOffset));

                if (compareToLastRecord && lastRecordfileNumber == fileNumber) {
                    // here we compare the first to last, if they are the same, we dont check this block
                //    printf("same file number skip 10000\n");
                    oldFileNumber = fileNumber;
                    oldFileOffset =lastRecordfileOffset;
                    indexFileBuffer.setPosition(lastRecordPos +indexRecordSize);
                    i = i+ (lastRecordPos/indexRecordSize) + 1;
                    break;
                }
                compareToLastRecord = false;

                if (fileNumber!=oldFileNumber) {
                    validateDataFile(oldFileNumber, oldFileOffset );
                    oldFileNumber = fileNumber;
                    oldFileOffset = fileOffset;
                }
                else {
                    oldFileOffset = fileOffset;
                }
                ++i;
            }
            indexFileBuffer.compact();
        }
        validateDataFile(oldFileNumber, oldFileOffset );
        m_pIndexFile->seekg(indexFileLength);
    }

	//init info
	// m_pInfoFile->open(infoFileName, openMode);
	// if (!m_pInfoFile->is_open())
	// 	throw dep::DEPException{ "Fail to open index file: " + indexFileName };

    const auto dataFileName = getDataFileName(m_dataFileCount-1);
    m_pDataFile->open(dataFileName, openMode);
    if (!m_pDataFile->is_open())
        throw dep::DEPException{"Fail to open data file: " + dataFileName};

    
    this->fillDefaultLevelInfo(m_recordCount);
	rebuildInfo();
    m_pIndexFile->seekg(0, std::ios_base::end);
    m_pIndexFile->seekp(0, std::ios_base::end);
}

template <class DataFormat, class LockType, class FileChannel>
void VariableRecordStorePersist<DataFormat, LockType, FileChannel>::validateDataFile(FileNumberType fileNumber, DataOffsetType fileOffset ) {
    const auto dataFileName = getDataFileName(fileNumber);
    std::unique_ptr<FileChannel> pDataFile = dep::make_unique<FileChannel>();
    pDataFile->open(dataFileName, std::ios_base::in|  std::ios_base::out | std::ios_base::ate);
    if (!pDataFile->is_open())
        throw dep::DEPException("Data file cannot be open:"+ dataFileName);
    auto dataFileLength = pDataFile->tellp();
    if (dataFileLength != fileOffset)
        throw dep::DEPException("Data file and index file length not match file["+ dataFileName
                           + "] expectLength["+ std::to_string(fileOffset)
                           + "] actualLength["+ std::to_string(dataFileLength)+ "]");

}

template <class DataFormat, class LockType, class FileChannel>
inline int64_t VariableRecordStorePersist<DataFormat, LockType, FileChannel>::readFromDataFile(FileChannel& dataFile, dep::ByteBuffer& buffer, uint64_t length) {
    int64_t readBytes = dataFile.readToBuf(reinterpret_cast<char*>(buffer.getArray() + buffer.getPosition()), length);
    if (readBytes > 0)
        buffer.setPosition(buffer.getPosition() + readBytes);

    // Always want the eof flag to be off, because this might mess up the next read
    if (dataFile.eof())
        dataFile.clear();
    
    return readBytes;
}

static std::string get_humanized_size(unsigned long long n)
{

    char buf[256] = "";
    if (n < 1024) {
        sprintf(buf, "%llu", n);
    } else if (n < 1024 * 1024) {
        sprintf(buf, "%.2fK", n / 1024.0);
    } else if (n < 1024 * 1024 * 1024) {
        sprintf(buf, "%.2fM", n / (1024.0 * 1024));
    } else {
        sprintf(buf, "%.2fG", n / (1024.0 * 1024 * 1024));
    }

    return buf;
}

template <class DataFormat, class LockType, class FileChannel>
inline void VariableRecordStorePersist<DataFormat, LockType, FileChannel>::writeToDataFile(dep::ByteBuffer& buffer) {
    m_pDataFile->seekp(0, std::ios_base::end);
    uint32_t bytesToWrite = buffer.remaining();
    int64_t bytesWriten = m_pDataFile->writeBuf(reinterpret_cast<char*>(buffer.getArray() + buffer.getPosition()), bytesToWrite);

    if (bytesWriten > 0) {
        buffer.setPosition(buffer.getPosition() + bytesWriten);
    }

    if (bytesWriten != bytesToWrite) {
        struct statfs fsinfo;
        statfs(m_storePath.c_str(), &fsinfo);


        unsigned long long totalDisk = fsinfo.f_bsize * fsinfo.f_blocks; 
        unsigned long long freeDisk  = fsinfo.f_bsize * fsinfo.f_bfree; 
        unsigned long long avalDisk  = fsinfo.f_bsize * fsinfo.f_bavail; 


        printf("totalDisk: %s\n", get_humanized_size(totalDisk).c_str());
        printf(" freeDisk: %s\n", get_humanized_size(freeDisk).c_str());
        printf(" avalDisk: %s\n", get_humanized_size(avalDisk).c_str());
        printf("storeBase: %s\n", m_storePath.c_str());


        printf("Call System command: df -h");
        system("df -h");
        printf("Call System command: du /home/lmq/lmq-test -h");
        system("du /home/lmq/lmq-test -h");
        printf("Call System command: du /home/lmq/corefile -h");
        system("du /home/lmq/corefile -h");


        throw DEPStorageException("failed to write buf, toWrite " + std::to_string(bytesToWrite) + ", writened " + std::to_string(bytesWriten)
                + ", totalDisk: " + get_humanized_size(totalDisk)
                + ", freeDisk: " + get_humanized_size(freeDisk)
                + ", avalDisk: " + get_humanized_size(avalDisk)
                + ", storeBase: " + m_storePath);
    }
}

template <class DataFormat, class LockType, class FileChannel>
inline std::string VariableRecordStorePersist<DataFormat, LockType, FileChannel>::getDataFileName(FileNumberType number) const {
    return m_storePath + m_storeName + getDataExtension() + "." + std::to_string(number);
}

template <class DataFormat, class LockType, class FileChannel>
void VariableRecordStorePersist<DataFormat, LockType, FileChannel>::generateIndexFileHeader(uint32_t version, uint32_t referenceTime) {
    uint64_t reserveBytes =0;

    if (referenceTime == 0)
        referenceTime = generateReferenceTime();

    //initReferenceTime(referenceTime);

    writeToIndexFile(version);
	writeToIndexFile(referenceTime);
	writeToIndexFile(reserveBytes);
    writeToIndexFile(reserveBytes);
    writeToIndexFile(reserveBytes);
    m_pIndexFile->flush();
}

template <class DataFormat, class LockType, class FileChannel>
void VariableRecordStorePersist<DataFormat, LockType, FileChannel>::readIndexRecord(FileNumberType& number, DataOffsetType& offset) {

    readFromIndexFile(number);
    readFromIndexFile(offset);
    m_pIndexFile->ignore(sizeof(InfoOffsetType));
    m_pIndexFile->ignore(sizeof(TimestampType));

    m_pIndexFile->ignore(sizeof(MessageAttributeType));
    m_pIndexFile->ignore(sizeof(MessageMarkType));
    m_pIndexFile->ignore(sizeof(ReservedByteType));
    m_pIndexFile->ignore(sizeof(ReservedByteType));

}

template <class DataFormat, class LockType, class FileChannel>
void VariableRecordStorePersist<DataFormat, LockType, FileChannel>::readIndexRecord(FileNumberType& number, DataOffsetType& offset, TimestampType& timestamp) {
    readFromIndexFile(number);
    readFromIndexFile(offset);
	m_pIndexFile->ignore(sizeof(InfoOffsetType));
    readFromIndexFile(timestamp);

    m_pIndexFile->ignore(sizeof(MessageAttributeType));
    m_pIndexFile->ignore(sizeof(MessageMarkType));
    m_pIndexFile->ignore(sizeof(ReservedByteType));
    m_pIndexFile->ignore(sizeof(ReservedByteType));
}

template <class DataFormat, class LockType, class FileChannel>
void VariableRecordStorePersist<DataFormat, LockType, FileChannel>::readIndexRecord(FileNumberType& number, DataOffsetType& offset, TimestampType& timestamp, MessageAttributeType& attribute) {
    readFromIndexFile(number);
    readFromIndexFile(offset);
	m_pIndexFile->ignore(sizeof(InfoOffsetType));
    readFromIndexFile(timestamp);
    readFromIndexFile(attribute);

    m_pIndexFile->ignore(sizeof(MessageMarkType));
    m_pIndexFile->ignore(sizeof(ReservedByteType));
    m_pIndexFile->ignore(sizeof(ReservedByteType));
}

template <class DataFormat, class LockType, class FileChannel>
void VariableRecordStorePersist<DataFormat, LockType, FileChannel>::readIndexRecord(FileNumberType& number, DataOffsetType& offset, TimestampType& timestamp, MessageAttributeType& attribute, MessageMarkType& mark) {
    readFromIndexFile(number);
	readFromIndexFile(offset);
	m_pIndexFile->ignore(sizeof(InfoOffsetType));
    readFromIndexFile(timestamp);
    readFromIndexFile(attribute);
    readFromIndexFile(mark);

    m_pIndexFile->ignore(sizeof(ReservedByteType));
    m_pIndexFile->ignore(sizeof(ReservedByteType));
}

template <class DataFormat, class LockType, class FileChannel>
void VariableRecordStorePersist<DataFormat, LockType, FileChannel>::readIndexRecord(uint64_t recordOffset, FileNumberType& number, DataOffsetType& offset, TimestampType& timestamp) {

    m_pIndexFile->seekg(recordOffset);
    readFromIndexFile(number);
	readFromIndexFile(offset);
	m_pIndexFile->ignore(sizeof(InfoOffsetType));
    readFromIndexFile(timestamp);

    m_pIndexFile->ignore(sizeof(MessageAttributeType));
    m_pIndexFile->ignore(sizeof(MessageMarkType));
    m_pIndexFile->ignore(sizeof(ReservedByteType));
    m_pIndexFile->ignore(sizeof(ReservedByteType));
}

template <class DataFormat, class LockType, class FileChannel>
void VariableRecordStorePersist<DataFormat, LockType, FileChannel>::readIndexRecord(uint64_t recordOffset, FileNumberType& number, DataOffsetType& offset, TimestampType& timestamp, MessageAttributeType& attribute) {

    m_pIndexFile->seekg(recordOffset);
    readFromIndexFile(number);
	readFromIndexFile(offset);
	m_pIndexFile->ignore(sizeof(InfoOffsetType));
    readFromIndexFile(timestamp);

    readFromIndexFile(attribute);
    m_pIndexFile->ignore(sizeof(MessageMarkType));
    m_pIndexFile->ignore(sizeof(ReservedByteType));
    m_pIndexFile->ignore(sizeof(ReservedByteType));
}

template <class DataFormat, class LockType, class FileChannel>
void VariableRecordStorePersist<DataFormat, LockType, FileChannel>::readIndexRecord(uint64_t recordOffset, FileNumberType& number, DataOffsetType& offset, TimestampType& timestamp, MessageAttributeType& attribute, MessageMarkType& mark) {

    m_pIndexFile->seekg(recordOffset);
    readFromIndexFile(number);
	readFromIndexFile(offset);
	m_pIndexFile->ignore(sizeof(InfoOffsetType));
    readFromIndexFile(timestamp);

    readFromIndexFile(attribute);
    readFromIndexFile(mark);
    m_pIndexFile->ignore(sizeof(ReservedByteType));
    m_pIndexFile->ignore(sizeof(ReservedByteType));
}


template <class DataFormat, class LockType, class FileChannel>
inline void VariableRecordStorePersist<DataFormat, LockType, FileChannel>::addIndexRecord(FileNumberType number, DataOffsetType offset, const TimestampType& timestamp,
    uint8_t messageAttribute /*= DEFAULT_MESSAGE_ATTRIBUTE*/,
    uint8_t messageMark /*= DEFAULT_MESSAGE_MARK*/)
{
    m_indexMessage.clear();
	m_indexMessage << number;
	m_indexMessage << offset;
	m_indexMessage << uint64_t{ 0 };
	m_indexMessage << timestamp;
	m_indexMessage << messageAttribute;
	m_indexMessage << messageMark;

	// add 2 more reserve dep::byte
	uint8_t reservedByte = 0x00;
	m_indexMessage << reservedByte;
	m_indexMessage << reservedByte;
	m_indexMessage.flip();
	writeToIndexFile((const char*)m_indexMessage.getArray(), m_indexMessage.remaining());
//     writeToIndexFile(number);
// 	   writeToIndexFile(offset);
//	   writeToIndexFile(infoOffset);
//     writeToIndexFile(timestamp - m_referenceTime);
//     writeToIndexFile(messageAttribute);
//     writeToIndexFile(messageMark);
//
//     // add 2 more reserve dep::byte
//     uint8_t reservedByte=0x00;
//     writeToIndexFile(reservedByte);
//     writeToIndexFile(reservedByte);


}

template <class DataFormat, class LockType, class FileChannel>
template <typename T>
inline void VariableRecordStorePersist<DataFormat, LockType, FileChannel>::readFromIndexFile(T& value) {
    FileRecordUtil<dep::FsyncFileStream>::readFromFile(*m_pIndexFile, value);
}

template <class DataFormat, class LockType, class FileChannel>
template <typename T>
inline void VariableRecordStorePersist<DataFormat, LockType, FileChannel>::writeToIndexFile(const T& value) {
    FileRecordUtil<dep::FsyncFileStream>::writeToFile(*m_pIndexFile, value);
}

template <class DataFormat, class LockType, class FileChannel>
void VariableRecordStorePersist<DataFormat, LockType, FileChannel>::writeToIndexFile(const char* s, uint32_t length) {
    FileRecordUtil<dep::FsyncFileStream>::writeToFile(*m_pIndexFile, s, length);
}

template <class DataFormat, class LockType, class FileChannel>
inline FileChannel* VariableRecordStorePersist<DataFormat, LockType, FileChannel>::acquireDataFile(FileNumberType fileNumber) {
    if (fileNumber == m_dataFileCount-1) {
        return m_pDataFile.get();
    }
    else {
        const auto dataFileName = getDataFileName(fileNumber);
        m_pTempDataFile->open(dataFileName, std::ios_base::in | std::ios_base::out);
        if (!m_pTempDataFile->is_open())
            throw dep::DEPException{"Fail to open data file: " + dataFileName};
        else
            return m_pTempDataFile.get();
    }
}

template <class DataFormat, class LockType, class FileChannel>
inline void VariableRecordStorePersist<DataFormat, LockType, FileChannel>::releaseDataFile(FileNumberType fileNumber) {
    if (fileNumber < m_dataFileCount - 1)
        m_pTempDataFile->close();
}

template <class DataFormat, class LockType, class FileChannel>
void VariableRecordStorePersist<DataFormat, LockType, FileChannel>::initReferenceTime(uint32_t referenceTime) {
    //m_referenceTime = std::chrono::duration_cast<std::chrono::high_resolution_clock::duration>(std::chrono::hours{referenceTime}).count();
}

template <class DataFormat, class LockType, class FileChannel>
inline uint32_t VariableRecordStorePersist<DataFormat, LockType, FileChannel>::generateReferenceTime() {
    return std::chrono::duration_cast<std::chrono::hours>(std::chrono::system_clock::now().time_since_epoch()).count();
}

template <class DataFormat, class LockType, class FileChannel>
constexpr const char* VariableRecordStorePersist<DataFormat, LockType, FileChannel>::getIndexExtension() {
    return DataFormat::INDEX_EXTENSION;
}

template <class DataFormat, class LockType, class FileChannel>
constexpr const char* VariableRecordStorePersist<DataFormat, LockType, FileChannel>::getDataExtension() {
    return DataFormat::DATA_EXTENSION;
}


template <class DataFormat, class LockType /*= dep::NoLock*/, class FileChannel /*= dep::BufferedFileStream*/>
constexpr const char* VariableRecordStorePersist<DataFormat, LockType, FileChannel>::getInfoExtension(){
	return DataFormat::INFO_EXTENSION;
}

template <class DataFormat, class LockType, class FileChannel>
uint64_t VariableRecordStorePersist<DataFormat, LockType, FileChannel>::getDataStart(uint64_t recordStart) {
    return recordStart + DataFormat::PREFIX_SIZE;
}

template <class DataFormat, class LockType, class FileChannel>
uint64_t VariableRecordStorePersist<DataFormat, LockType, FileChannel>::getDataEnd(uint64_t recordEnd) {
    return recordEnd - DataFormat::SUFFIX_SIZE;
}

template <class DataFormat, class LockType, class FileChannel>
template <class TargetFormat>
constexpr uint32_t VariableRecordStorePersist<DataFormat, LockType, FileChannel>::getPrefixSize() {
    return TargetFormat::PREFIX_SIZE;
}

template <class DataFormat, class LockType, class FileChannel>
template <class TargetFormat>
constexpr uint32_t VariableRecordStorePersist<DataFormat, LockType, FileChannel>::getSuffixSize() {
    return TargetFormat::SUFFIX_SIZE;
}

template <class DataFormat, class LockType, class FileChannel>
template <class TargetFormat>
void VariableRecordStorePersist<DataFormat, LockType, FileChannel>::putPrefix(uint32_t length, dep::ByteBuffer& buffer) {
    putPrefixImpl<TargetFormat>(length, buffer, std::integral_constant<bool,(TargetFormat::PREFIX_SIZE > 0)>{});
}

template <class DataFormat, class LockType, class FileChannel>
template <class TargetFormat>
void VariableRecordStorePersist<DataFormat, LockType, FileChannel>::putPrefixImpl(uint32_t length, dep::ByteBuffer& buffer, const std::true_type&) {
    TargetFormat::putPrefix(length, buffer);
}

template <class DataFormat, class LockType, class FileChannel>
template <class TargetFormat>
void VariableRecordStorePersist<DataFormat, LockType, FileChannel>::putPrefixImpl(uint32_t length, dep::ByteBuffer& buffer, const std::false_type&) {
}

template <class DataFormat, class LockType, class FileChannel>
template <class TargetFormat>
void VariableRecordStorePersist<DataFormat, LockType, FileChannel>::putSuffix(uint32_t length, dep::ByteBuffer& buffer) {
    putSuffixImpl<TargetFormat>(length, buffer, std::integral_constant<bool,(TargetFormat::SUFFIX_SIZE > 0)>{});
}

template <class DataFormat, class LockType, class FileChannel>
template <class TargetFormat>
void VariableRecordStorePersist<DataFormat, LockType, FileChannel>::putSuffixImpl(uint32_t length, dep::ByteBuffer& buffer, const std::true_type&) {
    TargetFormat::putSuffix(length, buffer);
}

template <class DataFormat, class LockType, class FileChannel>
template <class TargetFormat>
void VariableRecordStorePersist<DataFormat, LockType, FileChannel>::putSuffixImpl(uint32_t length, dep::ByteBuffer& buffer, const std::false_type&) {
}



template <class FileChannel>
template <typename T>
inline void FileRecordUtil<FileChannel>::readFromFile(FileChannel& fileChannel, T& value) {
    readFromFileImpl(fileChannel, value, std::is_integral<T>{});
}

template <class FileChannel>
template <typename T>
inline void FileRecordUtil<FileChannel>::writeToFile(FileChannel& fileChannel, const T& value) {
    writeToFileImpl(fileChannel, value, std::is_integral<T>{});
}

template <class FileChannel>
void FileRecordUtil<FileChannel>::writeToFile(FileChannel& fileChannel, const char* s, uint32_t length) {
    if (length > 0) {
        fileChannel.seekp(0, std::ios_base::end);
        fileChannel.write(s, length);
    }
}

template <class FileChannel>
void FileRecordUtil<FileChannel>::writeToFileAtPosition(FileChannel& fileChannel, const char* s, uint32_t length, uint64_t position) {
    if (length > 0) {
        fileChannel.seekp(position, std::ios_base::beg);
        fileChannel.write(s, length);
    }
}

template <class FileChannel>
template <typename T>
void FileRecordUtil<FileChannel>::readFromFileImpl(FileChannel& fileChannel, T& value, const std::true_type&) {
    T localCopy = 0;
    fileChannel.read(reinterpret_cast<char*>(&localCopy), sizeof(T));

    if (dep::ByteBuffer::getHostByteOrder() == dep::ByteBuffer::ByteOrder::LITTLEENDIAN) {
        dep::ByteBuffer buffer{reinterpret_cast<dep::byte*>(&localCopy), sizeof(T), false};
        buffer.setLimit(buffer.getCapacity());
        buffer >> value;
    }
    else {
        value = localCopy;
    }
}

template <class FileChannel>
template <typename T>
inline void FileRecordUtil<FileChannel>::readFromFileImpl(FileChannel& fileChannel, T& value, const std::false_type&) {
    fileChannel.read(reinterpret_cast<char*>(&value), sizeof(T));
}

template <class FileChannel>
template <typename T>
void FileRecordUtil<FileChannel>::writeToFileImpl(FileChannel& fileChannel, const T& value, const std::true_type&) {
    T localCopy = value;
    if (dep::ByteBuffer::getHostByteOrder() == dep::ByteBuffer::ByteOrder::LITTLEENDIAN) {
        dep::ByteBuffer buffer{reinterpret_cast<dep::byte*>(&localCopy), sizeof(T), false};
        buffer << value;
    }

    fileChannel.seekp(0, std::ios_base::end);
    fileChannel.write(reinterpret_cast<const char*>(&localCopy), sizeof(T));
}

template <class FileChannel>
template <typename T>
inline void FileRecordUtil<FileChannel>::writeToFileImpl(FileChannel& fileChannel, const T& value, const std::false_type&) {
    fileChannel.seekp(0, std::ios_base::end);
    fileChannel.write(reinterpret_cast<const char*>(&value), sizeof(T));
}

}} // namespace lightmq { namespace util {

#endif /* LIGHTMQVARIABLERECORDSTORE_H_ */
