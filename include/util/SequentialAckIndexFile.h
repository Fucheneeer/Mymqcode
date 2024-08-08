////////////////////////////////////////////////////////////////////////////////
//
// SequentialAckIndexFile.h
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

#ifndef SEQUENTIALACKINDEXFILE_H_
#define SEQUENTIALACKINDEXFILE_H_

#include "ByteBuffer.h"
#include "DEPException.h"
#include "FileUtility.h"
#include "CRC.h"
#include "IAckIndexFile.h"
#include "IVariableRecordStore.h"

#include "VariableRecordStorePersist.h"

namespace lightmq { namespace util {

#define sss (m_storePath + ", " + m_storeName + ", " + m_indexFileName).c_str()

// 文件格式：
// 第一版
// |acksequence(8byte)|crc(4byte)|
// 第二版：扩充消息级别
// |version(2byte)|reserveHead(8byte)
// |level0 ack sequence(8byte)|crc1(4byte)
// |level1 ack sequence(8byte)|crc1(4byte)
// ...
// |level9 ack sequence(8byte)|crc9(4byte)


#define MAKE_VERSION(v1,v2) (v1<<8)|(v2)
#define GET_VERSION(v, v1,v2) do{ v1 = (v&0xff00)>>8; v2 = (v&0x00FF);} while(0)  
static uint16_t s_version=MAKE_VERSION(1,1);

template <class FileChannel>
class SequentialAckIndexFile : public IAckIndexFile {
public:
    static bool isFileExist(const std::string& storePath, const std::string& storeName, const std::string& indexFileName);
    static void validateFile(const std::string& filepath);
    static bool validateVersion1_1(std::unique_ptr<FileChannel> &pAckIndexFile, std::string &reason);
public:
    SequentialAckIndexFile(IVariableRecordStore *store, const std::string& storePath, const std::string& storeName, const std::string& indexFileName, bool deleteOnExit, bool enableSyncMode=false,
                           std::function<std::unique_ptr<FileChannel>()>&& createFileChannelFunc = defaultCreateFileChannelFunc<FileChannel>);
    ~SequentialAckIndexFile();

    void init(bool append, uint32_t baseSequence, uint64_t recordCount, bool isDurable=false);

    void addRecord(uint64_t toSeq);
    void ackRecord(uint64_t seq);
    void ackRecord(uint8_t level, uint64_t seq);
    void markRetransmit(uint64_t seq);
    void getNonAckRecord(std::map<uint64_t,bool>& result, uint64_t& largeBlockStart, uint64_t& largeBlockEnd);
    void getAllCursor(MultiSequence &seq);
    void getExpiredAckRecord(std::map<uint64_t, bool>& result, uint64_t& largeBlockStart, uint64_t& largeBlockEnd);
    void flush();
    void close();
    bool getFirstNonAckRecord(uint64_t& sequence); // return false, if no last non-ack record
    bool isRecordAcked(uint64_t seq) override;
    virtual bool isRecordAcked(uint8_t level, uint64_t seq) override;
private:
   static constexpr auto ACK_INDEX_EXTENSION = ".SAI";   /**< Index file extension **/
   static constexpr auto FILE_NAME_DELIMITER = ".";
   static constexpr auto BUFFER_SIZE = 12; // only CRC(4 bytes) and sequence(8 bytes)

    std::string getAckIndexFileName() const;
    void clearBadFile();
    uint32_t back8GetCrc(dep::ByteBuffer &buffer);
    void back8WriteCrc(dep::ByteBuffer &buffer);
    void generateFile();
    bool readOldVersion();
    bool readVersion1_1();
    void writeRecord(uint8_t level, uint64_t newAckRecordCount);

 

    static bool testRead(std::unique_ptr<FileChannel> &pAckIndexFile, std::string &reason, uint8_t printLev);

private:

    std::unique_ptr<FileChannel> m_pAckIndexFile;
    IVariableRecordStore *m_store{nullptr};
    std::string m_storePath;
    std::string m_storeName;
    std::string m_indexFileName;
    bool m_deleteOnExit;
    uint32_t m_baseSequence{0};
    bool m_isDurable{false};
    uint64_t m_currentRecordCount{0};
    // uint64_t m_currentAckRecordCount{0};
    MultiSequence m_multiAckRecord;

    dep::ByteBuffer m_tmpBuffer{RECORD_SIZE};
    dep::CRC m_crc;
    bool m_enableSyncMode{false};

    static constexpr int HEAD_SIZE{10};
    static constexpr int RECORD_SIZE{12}; // (sequence + crc)
    static constexpr int VERSION11_FILELENGTH{HEAD_SIZE+RECORD_SIZE*10};
};

////////
template <class FileChannel>
void lightmq::util::SequentialAckIndexFile<FileChannel>::clearBadFile(){
    auto ackIndexFileName = getAckIndexFileName();
    m_pAckIndexFile->close();
    m_pAckIndexFile->open(ackIndexFileName, std::ios_base::in | std::ios_base::out |  std::ios_base::trunc);
    m_multiAckRecord.reset();
}

// 
template <class FileChannel>
uint32_t lightmq::util::SequentialAckIndexFile<FileChannel>::back8GetCrc(dep::ByteBuffer &buffer){
    m_crc.reset();
    m_crc.update((const char*)buffer.getArray() + buffer.getPosition() - 8, 8);
    return m_crc.getValue();
}

// 退后八字节，计算crc，然后把crc写入当前位置
template <class FileChannel>
void lightmq::util::SequentialAckIndexFile<FileChannel>::back8WriteCrc(dep::ByteBuffer &buffer){
    m_crc.reset();
    m_crc.update((const char*)buffer.getArray() + buffer.getPosition() - 8, 8);
    uint32_t crc = m_crc.getValue();
    buffer << crc;
    DEBUG_PRINT("SequentialAckIndexFile back8write crc<%u> filename<%s>", crc, sss);
}

template <class FileChannel>
void lightmq::util::SequentialAckIndexFile<FileChannel>::generateFile(){
    auto ackIndexFileName = getAckIndexFileName();
    m_pAckIndexFile->close();
    m_pAckIndexFile->open(ackIndexFileName, std::ios_base::in | std::ios_base::out |  std::ios_base::trunc);
    
    dep::ByteBuffer buffer(VERSION11_FILELENGTH);
    uint64_t reserve{0};
    buffer << s_version ;
    buffer << reserve;
    FOREACH_LEVEL(lev)
    {
        buffer << m_multiAckRecord.get(lev);
        DEBUG_PRINT("SequentialAckIndexFile generate level<%zd>, seq<%lu> filename<%s>", lev, m_multiAckRecord.get(lev), sss);
        back8WriteCrc(buffer);
    }

    buffer.flip();

    auto hex = util::hexdumpLimit((const char*)buffer.getArray(), buffer.remaining(), 128);
    DEBUG_PRINT("generate buffer  filename<%s>\n%s\n", hex.c_str(), sss);
    
    m_pAckIndexFile->write(buffer);
    m_pAckIndexFile->flush();
    if (m_enableSyncMode) {
        m_pAckIndexFile->sync();
    }

    // std::string reason;
    // if (validateVersion1_1(m_pAckIndexFile, reason))
    // {
    //     throw dep::DEPException{std::string ("error reason:  ") + reason };
    // }
}

// read old version 12bytes.
// return: true if read ok, else return false.
template <class FileChannel>
bool lightmq::util::SequentialAckIndexFile<FileChannel>::readOldVersion(){
    dep::ByteBuffer buffer(BUFFER_SIZE);
    m_pAckIndexFile->seekg(0, std::ios_base::beg); // reset the point to beginning
    m_pAckIndexFile->read(buffer, buffer.remaining());
    buffer.flip();

    uint32_t crc=0;
    uint64_t ackCount;

    buffer >> ackCount;
    uint32_t actualCrc = back8GetCrc(buffer);
    buffer>> crc;
    if (actualCrc != crc) return false;

    if (ackCount > m_currentRecordCount)
        ackCount = m_currentRecordCount; // ack record cannot more than existing record
    m_multiAckRecord.setAll(ackCount);
    return true;
}

template <class FileChannel>
bool lightmq::util::SequentialAckIndexFile<FileChannel>::testRead(std::unique_ptr<FileChannel> &pAckIndexFile, std::string &reason, uint8_t printLev){
    // 
    dep::ByteBuffer buffer(VERSION11_FILELENGTH);
    pAckIndexFile->seekg(0, std::ios_base::beg); // reset the point to beginning
    pAckIndexFile->read(buffer, buffer.remaining());
    buffer.flip();
    if (buffer.remaining() != VERSION11_FILELENGTH) 
    {
        reason = "file length error";
        return false;
    }

    uint16_t version, v1, v2; 
    buffer >> version;
    GET_VERSION(version, v1, v2);
    if (v1 != 1 || v2 != 1)
    {
        reason = "read version error";
        return false;
    }

    dep::CRC tempCrc;
    const int reserveBytes = 8;
    buffer.setPosition(buffer.getPosition() + reserveBytes);
    FOREACH_LEVEL(lev)
    {
        uint64_t seq;
        uint32_t crc;
        buffer >> seq;
        tempCrc.reset();
        tempCrc.update((const char*)buffer.getArray() + buffer.getPosition() - 8, 8);
        auto actualCrc = tempCrc.getValue();

        buffer >> crc;
        if (lev == printLev)
        {
            DEBUG_PRINT("SequentialAckIndexFile print validate read level<%d>, seq<%lu> crc<%u,%u> ", lev, seq, crc, actualCrc);
        }
        if (crc != actualCrc)
        {
            reason = std::string("crc error with lev<") + std::to_string(lev) + ">";
            return false;
        }
    }

    return true;
}



template <class FileChannel>
bool lightmq::util::SequentialAckIndexFile<FileChannel>::validateVersion1_1(std::unique_ptr<FileChannel> &pAckIndexFile, std::string &reason){
    // 
    dep::ByteBuffer buffer(VERSION11_FILELENGTH);
    pAckIndexFile->seekg(0, std::ios_base::beg); // reset the point to beginning
    pAckIndexFile->read(buffer, buffer.remaining());
    buffer.flip();
    if (buffer.remaining() != VERSION11_FILELENGTH) 
    {
        reason = "file length error";
        return false;
    }

    auto hex = util::hexdumpLimit((const char*)buffer.getArray(), buffer.remaining(), 128);
    DEBUG_PRINT("validate buffer  \n%s\n", hex.c_str());

    uint16_t version, v1, v2; 
    buffer >> version;
    GET_VERSION(version, v1, v2);
    if (v1 != 1 || v2 != 1)
    {
        reason = "read version error";
        return false;
    }

    dep::CRC tempCrc;
    const int reserveBytes = 8;
    buffer.setPosition(buffer.getPosition() + reserveBytes);
    FOREACH_LEVEL(lev)
    {
        uint64_t seq;
        uint32_t crc;
        buffer >> seq;
        tempCrc.reset();
        tempCrc.update((const char*)buffer.getArray() + buffer.getPosition() - 8, 8);
        auto actualCrc = tempCrc.getValue();

        buffer >> crc;
        DEBUG_PRINT("SequentialAckIndexFile validate read level<%zd>, seq<%lu> crc<%u,%u> ", lev, seq, crc, actualCrc);
        if (crc != actualCrc)
        {
            reason = std::string("crc error with lev<") + std::to_string(lev) + ">";
            return false;
        }
    }

    return true;
}


template <class FileChannel>
bool lightmq::util::SequentialAckIndexFile<FileChannel>::readVersion1_1(){
    // 
    dep::ByteBuffer buffer(VERSION11_FILELENGTH);
    m_pAckIndexFile->seekg(0, std::ios_base::beg); // reset the point to beginning
    m_pAckIndexFile->read(buffer, buffer.remaining());
    buffer.flip();
    if (buffer.remaining() != VERSION11_FILELENGTH) return false;

    uint16_t version, v1, v2; 
    buffer >> version;
    GET_VERSION(version, v1, v2);
    if (v1 != 1 && v2 != 1) return false; 

    const int reserveBytes = 8;
    buffer.setPosition(buffer.getPosition() + reserveBytes);
    FOREACH_LEVEL(lev)
    {
        uint64_t seq;
        uint32_t crc;
        buffer >> seq;
        auto actualCrc = back8GetCrc(buffer);
        buffer>> crc;
        if (crc != actualCrc) return false;

        m_multiAckRecord.set(lev, seq);
    }

    return true;
}

// 更新文件
// 读的有好多版本，写的只需要保留一个当前版本
template <class FileChannel>
void lightmq::util::SequentialAckIndexFile<FileChannel>::writeRecord(uint8_t level, uint64_t newAckRecordCount){
    m_tmpBuffer.clear();
    m_tmpBuffer << newAckRecordCount;
    DEBUG_PRINT("SequentialAckIndexFile back8write level<%u>, seq<%lu> filename<%s>", level, newAckRecordCount, sss);
    back8WriteCrc(m_tmpBuffer);
    m_tmpBuffer.flip();

    m_pAckIndexFile->seekp(HEAD_SIZE + (level)*RECORD_SIZE, std::ios_base::beg); // reset the point to beginning
    m_pAckIndexFile->write(m_tmpBuffer);
    m_pAckIndexFile->flush();
    if (m_enableSyncMode) {
        m_pAckIndexFile->sync();
    }

    // std::string tt;
    // testRead(m_pAckIndexFile, tt, level);
}

template <class FileChannel>
bool lightmq::util::SequentialAckIndexFile<FileChannel>::isRecordAcked(uint64_t seq){
    return isRecordAcked(m_store->getRecordLevel(seq), seq);
}

template <class FileChannel>
bool lightmq::util::SequentialAckIndexFile<FileChannel>::isRecordAcked(uint8_t level, uint64_t seq){
    return seq <= m_multiAckRecord.get(level);
}

template <class FileChannel>
bool SequentialAckIndexFile<FileChannel>::isFileExist(const std::string& storePath, const std::string& storeName, const std::string& indexFileName) {

    std::string fileName = storePath;
    if (fileName.back() != '/')
        fileName += "/";
    fileName.append(storeName + FILE_NAME_DELIMITER + indexFileName + ACK_INDEX_EXTENSION);
    return dep::FileUtility::isFileExist(fileName);
}

template <class FileChannel>
void SequentialAckIndexFile<FileChannel>::validateFile(const std::string& filepath) {

    const auto& ackIndexFileName = filepath;

    auto openMode = std::ios_base::in | std::ios_base::out |std::ios_base::ate;

    uint64_t currentLength=0;
    std::unique_ptr<FileChannel> pAckIndexFile= dep::make_unique<FileChannel>();;
    pAckIndexFile->open(ackIndexFileName, openMode);

    if (!pAckIndexFile->is_open())
        throw dep::DEPException{"Fail to open time index file: " + ackIndexFileName};

    currentLength = pAckIndexFile->tellp();

    if (currentLength==0) {
        return;
    }
    else if (currentLength!=12) {
        std::string reason;
        bool ret = validateVersion1_1(pAckIndexFile, reason);
        if (!ret)
        {
            throw dep::DEPException("Corrupted Sequential Ack index file: "+ ackIndexFileName + " with error " + reason);
        }
    }
    else {
        dep::ByteBuffer buffer(BUFFER_SIZE);
        pAckIndexFile->seekg(0, std::ios_base::beg); // reset the point to beginning
        pAckIndexFile->read(buffer, buffer.remaining());
        buffer.flip();

        uint32_t crc=0;
        dep::CRC realcrc;
        uint32_t beginPos=buffer.getPosition();
        uint64_t currentAckRecordCount=0;
        buffer >> currentAckRecordCount;
        uint32_t endPos = buffer.getPosition();
        buffer>> crc;

        realcrc.reset();
        realcrc.update((const char*)buffer.getArray() + beginPos, endPos - beginPos);
        uint32_t actualCrc = realcrc.getValue();
        if (actualCrc!=crc) {
            throw dep::DEPException("Corrupted Sequential Ack index file, CRC not match: "+ ackIndexFileName);
        }

}
}

template <class FileChannel>
SequentialAckIndexFile<FileChannel>::SequentialAckIndexFile(IVariableRecordStore *pStore, const std::string& storePath, const std::string& storeName, const std::string& indexFileName, bool deleteOnExit, bool enableSyncMode, std::function<std::unique_ptr<FileChannel>()>&& createFileChannelFunc)
    : m_store(pStore)
    , m_deleteOnExit(deleteOnExit)
    , m_enableSyncMode(enableSyncMode) {


    m_storePath = storePath;
    if (m_storePath.back() != '/')
        m_storePath += "/";

    m_storeName = storeName;
    m_indexFileName = indexFileName;
    m_pAckIndexFile = createFileChannelFunc();
}

template <class FileChannel>
SequentialAckIndexFile<FileChannel>::~SequentialAckIndexFile() {
    if (m_pAckIndexFile->is_open())
        m_pAckIndexFile->close();

    if (m_deleteOnExit) {
        std::remove(getAckIndexFileName().c_str());
    }
}


template <class FileChannel>
void SequentialAckIndexFile<FileChannel>::init(bool append, uint32_t baseSequence, uint64_t recordCount, bool isDurable) {
    m_baseSequence=baseSequence;
    m_currentRecordCount = recordCount;
    m_isDurable = isDurable;


    if (m_isDurable) {

        const auto ackIndexFileName = getAckIndexFileName();
        auto ackIndexFileExists = dep::FileUtility::isFileExist(ackIndexFileName);

        bool isOldFile = (append && ackIndexFileExists && recordCount>0 );
        auto openMode = std::ios_base::in | std::ios_base::out
                        | (isOldFile? std::ios_base::ate : std::ios_base::trunc);

        uint64_t currentLength=0;
        // m_currentAckRecordCount=0;
        m_pAckIndexFile->open(ackIndexFileName, openMode);
        if (!m_pAckIndexFile->is_open())
            throw dep::DEPException{"Fail to open time index file: " + ackIndexFileName};

        currentLength = m_pAckIndexFile->tellp();

        if (currentLength> 0 && currentLength < 12) {
            // clearBadFile();
            generateFile();
        }
        else if (currentLength==12){
            if (!readOldVersion()) {
                clearBadFile();
            }
            m_pAckIndexFile->seekg(0, std::ios_base::beg); // reset the point to beginning
            generateFile(); // 
        }
        else if (currentLength == VERSION11_FILELENGTH)
        {
            if (!readVersion1_1()) 
            {
                clearBadFile();
            }
        }
        else
        {
            // clearBadFile();
            generateFile();
        }
    }
    else {
        //m_currentAckRecordCount = recordCount; // for non-durable case, we initialize it to this value
        m_multiAckRecord.setAll(recordCount); // for non-durable case, we initialize it to this value
    }


}

template <class FileChannel>
void SequentialAckIndexFile<FileChannel>::addRecord(uint64_t toSeq) {
    m_currentRecordCount=toSeq ;
}  

template <class FileChannel>
void SequentialAckIndexFile<FileChannel>::ackRecord(uint64_t seq) {
    // 主机中sessionStore是肯定存在的
    uint8_t level = m_store->getRecordLevel(seq);
    ackRecord(level, seq);
}

// 主备同步的时候，store中可能还不存在对应的数据，因此消息级别应该从外部传入
template <class FileChannel>
void SequentialAckIndexFile<FileChannel>::ackRecord(uint8_t level, uint64_t seq) 
{
    if (level > CommonConstant::MAX_MESSAGE_LEVEL) return ;

    uint64_t newAckRecordCount = seq - m_baseSequence +1;
   // if (newAckRecordCount<=m_currentAckRecordCount)
   //     throw dep::DEPException("Ack sequence[" + std::to_string(seq)+"] is smaller than previous Ack sequence");
   // if (newAckRecordCount > m_currentRecordCount)
   //     throw dep::DEPException("Ack sequence[" + std::to_string(seq)+"] is larger than existing records");

    if (newAckRecordCount <= m_multiAckRecord.get(level))
        return;

    m_multiAckRecord.set(level, newAckRecordCount);

    if (m_isDurable) {
        writeRecord(level, newAckRecordCount);
    }
}

template <class FileChannel>
void SequentialAckIndexFile<FileChannel>::markRetransmit(uint64_t seq) {
    throw dep::DEPException("SequentialAckIndexFile does not support markRetransmit function");
}

template <class FileChannel>
void SequentialAckIndexFile<FileChannel>::getNonAckRecord(std::map<uint64_t, bool>& result, uint64_t& largeBlockStart, uint64_t& largeBlockEnd) {
    throw dep::DEPException("SequentialAckIndexFile does not support getNonAckRecord function");
}

template <class FileChannel>
void SequentialAckIndexFile<FileChannel>::getAllCursor(MultiSequence &seq)
{
    seq = m_multiAckRecord;
}

template <class FileChannel>
void SequentialAckIndexFile<FileChannel>::getExpiredAckRecord(std::map<uint64_t, bool>& result, uint64_t& largeBlockStart, uint64_t& largeBlockEnd) {
    throw dep::DEPException("SequentialAckIndexFile does not support getExpiredAckRecord function");
}

template <class FileChannel>
void SequentialAckIndexFile<FileChannel>::flush() {
    if (m_isDurable)
        m_pAckIndexFile->flush();
}

template <class FileChannel>
void SequentialAckIndexFile<FileChannel>::close() {
    if (m_isDurable && m_pAckIndexFile && m_pAckIndexFile->is_open())
            m_pAckIndexFile->close();
}

template <class FileChannel>
bool SequentialAckIndexFile<FileChannel>::getFirstNonAckRecord(uint64_t& sequence) {
    sequence = 0;
    FOREACH_LEVEL(level)
    {
        auto t = m_store->getFirstNoAckByLevel(level, m_multiAckRecord.get(level));
        if (t > 0 && t < sequence) sequence = t;
    }

    return 0 == sequence;
}


// template <class FileChannel>
// bool SequentialAckIndexFile<FileChannel>::getFirstNonAckRecord(uint64_t& sequence) {
//         throw dep::DEPException{"getFirstNonAckRecord should not called on sequential file ,file name: " + getAckIndexFileName()};
//         return false;
// //    if (m_currentRecordCount > m_currentAckRecordCount) { // This check is removed due to synchronization slow in message than index files
//         // remov by ghli 22/10/27-15:19:32
//         // sequence = m_baseSequence + m_currentAckRecordCount;
//         // return true;
// //    }
// //    return false;
// }

template <class FileChannel>
std::string SequentialAckIndexFile<FileChannel>::getAckIndexFileName() const {
    return m_storePath + m_storeName + FILE_NAME_DELIMITER + m_indexFileName + ACK_INDEX_EXTENSION;
}

}} // namespace lightmq { namespace util {

#endif /* SEQUENTIALACKINDEXFILE_H_ */
