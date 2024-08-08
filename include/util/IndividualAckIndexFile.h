////////////////////////////////////////////////////////////////////////////////
//
// IndividualAckIndexFile.h
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

#ifndef INDIVIDUALACKINDEXFILE_H_
#define INDIVIDUALACKINDEXFILE_H_

#include "ByteBuffer.h"
#include "DEPException.h"
#include "FileUtility.h"
#include "IAckIndexFile.h"
#include "ISeverityLog.h"

namespace lightmq { namespace util {

template <class FileChannel>
class IndividualAckIndexFile : public IAckIndexFile {
public:
    static bool isFileExist(const std::string& storePath, const std::string& storeName, const std::string& indexFileName);

private:
    static void fillByte(FileChannel* fileChannel, uint64_t count, uint8_t byte);

public:
    IndividualAckIndexFile(dep::ISeverityLog* pLogger, const std::string& storePath, const std::string& storeName, const std::string& indexFileName, bool deleteOnExit, bool enableSyncMode=false, std::function<std::unique_ptr<FileChannel>()>&& createFileChannelFunc = defaultCreateFileChannelFunc<FileChannel>);
    ~IndividualAckIndexFile();

    void init(bool append, uint32_t baseSequence, uint64_t recordCount, bool isDurable=false);

    void addRecord(uint64_t toSeq);
    void ackRecord(uint64_t seq);
    void ackRecord(uint8_t level, uint64_t seq);
    void markRetransmit(uint64_t seq);
    void getNonAckRecord(std::map<uint64_t, bool>& result, uint64_t& largeBlockStart, uint64_t& largeBlockEnd);
    void getAllCursor(MultiSequence &seq);
    void getExpiredAckRecord(std::map<uint64_t, bool>& result, uint64_t& largeBlockStart, uint64_t& largeBlockEnd);
    void flush();
    void close();
    bool getFirstNonAckRecord(uint64_t& sequence); // return false, if no last non-ack record
    virtual bool isRecordAcked(uint64_t seq) override;
    virtual bool isRecordAcked(uint8_t level, uint64_t seq) override;

private:
   static constexpr auto ACK_INDEX_EXTENSION = ".VRAI";   /**< Index file extension **/
   static constexpr auto FILE_NAME_DELIMITER = ".";
   static constexpr auto BUFFER_SIZE = 100000;
   static constexpr auto NON_ACK_FRESH = 0x00;
   static constexpr auto NON_ACK_RETRANSMIT = 0x01;
   static constexpr auto ACKED = 0x02;
   static constexpr auto EXPIRED_ACKED = 0x06;


    std::string getAckIndexFileName() const;
private:
    dep::ISeverityLog* m_pLogger;
    std::unique_ptr<FileChannel> m_pAckIndexFile;
    std::string m_storePath;
    std::string m_storeName;
    std::string m_indexFileName;
    bool m_deleteOnExit;
    uint64_t m_recordCount{0};
    uint64_t m_firstNonAckSeq{0}; // if non, it will equals to current sequence +1
    // 理论上本文件的最后存储的是一大块都没有回复ack的记录
    // 而这一大块记录前边是零散的没有回复ack的记录
    // 零散的为回复ack记录存储在m_NonAckSet中
    // 后边的一大块记录存放在[m_largeBlockStart, m_largeBlockEnd]
    uint64_t m_largeBlockStart{0}; // if non, it will equals to 0
    uint64_t m_largeBlockEnd{0}; // if non, it will equals to 0
    uint64_t m_currentFileLength{0};
    std::map<uint64_t, bool> m_NonAckSet; // seq, isRetransmit;
    std::map<uint64_t, bool> m_ExpiredAckMap;
    uint32_t m_baseSequence{0};
    bool m_isDurable{false};
    bool m_enableSyncMode{false};
    static constexpr auto HEADER_SIZE = sizeof(m_firstNonAckSeq);
};

template <class FileChannel>
bool lightmq::util::IndividualAckIndexFile<FileChannel>::isRecordAcked(uint64_t seq){
    return isRecordAcked(0, seq);
}

template <class FileChannel>
bool lightmq::util::IndividualAckIndexFile<FileChannel>::isRecordAcked(uint8_t level, uint64_t seq){
    (void)level;
	if (seq - m_baseSequence + 1 > m_recordCount) {
		return false;
	}
    return m_NonAckSet.find(seq) == m_NonAckSet.end() 
        && (m_largeBlockStart == 0 || seq < m_largeBlockStart);
}



template <class FileChannel>
void IndividualAckIndexFile<FileChannel>::fillByte(FileChannel* fileChannel, uint64_t count, uint8_t byte) {
    static thread_local dep::ByteBuffer buffer(BUFFER_SIZE);
    fileChannel->seekp(0, std::ios_base::end);
    while (count> 0) {
        buffer.clear();
        while (buffer.remaining()>0 && count > 0) {
            buffer << byte;
            count-=1;
        }
        buffer.flip();
        fileChannel->write(buffer);
    }
    fileChannel->flush();
}

template <class FileChannel>
bool IndividualAckIndexFile<FileChannel>::isFileExist(const std::string& storePath, const std::string& storeName, const std::string& indexFileName) {

    std::string fileName = storePath;
    if (fileName.back() != '/')
        fileName += "/";
    fileName.append(storeName + FILE_NAME_DELIMITER + indexFileName + ACK_INDEX_EXTENSION);
    return dep::FileUtility::isFileExist(fileName);
}

template <class FileChannel>
IndividualAckIndexFile<FileChannel>::IndividualAckIndexFile(dep::ISeverityLog* pLogger, const std::string& storePath, const std::string& storeName, const std::string& indexFileName, bool deleteOnExit, bool enableSyncMode, std::function<std::unique_ptr<FileChannel>()>&& createFileChannelFunc)
                                                    : m_pLogger(pLogger), m_deleteOnExit(deleteOnExit)
                                                    , m_enableSyncMode(enableSyncMode) {


    m_storePath = storePath;
    if (m_storePath.back() != '/')
        m_storePath += "/";

    m_storeName = storeName;
    m_indexFileName = indexFileName;
    m_pAckIndexFile = createFileChannelFunc();
}

template <class FileChannel>
IndividualAckIndexFile<FileChannel>::~IndividualAckIndexFile() {
    if (m_pAckIndexFile->is_open())
        m_pAckIndexFile->close();

    if (m_deleteOnExit) {
        std::remove(getAckIndexFileName().c_str());
    }
}
template <class FileChannel>
void IndividualAckIndexFile<FileChannel>::init(bool append, uint32_t baseSequence, uint64_t recordCount, bool isDurable) {
    DEP_LOG_DEBUG2_CHECK(m_pLogger, "individual index storeName[", m_storeName, "] init, recordCount[", recordCount, "]");

    m_isDurable = isDurable;
    m_recordCount = recordCount;
    m_baseSequence = baseSequence;
    m_firstNonAckSeq = m_baseSequence; // it is equals to baseSeq+recordCount, but recordCount is zero at the beginning
    const auto ackIndexFileName = getAckIndexFileName();
    auto ackIndexFileExists = dep::FileUtility::isFileExist(ackIndexFileName);

    bool isOldFile = (append && ackIndexFileExists && isDurable);
    auto openMode = std::ios_base::in | std::ios_base::out
                    | (isOldFile? std::ios_base::ate : std::ios_base::trunc);


    if (m_isDurable) {
        m_pAckIndexFile->open(ackIndexFileName, openMode);
        if (!m_pAckIndexFile->is_open())
            throw dep::DEPException{"Fail to open time index file: " + ackIndexFileName};
        m_currentFileLength = m_pAckIndexFile->tellp();
    }

  //  printf("open storeName[%s] storePath[%s] recordCount[%llu] currentLength[%llu]\n\n", m_storeName.data(), m_storePath.data(), (unsigned long long)recordCount, (unsigned long long) currentLength);

    if (isOldFile && m_currentFileLength >= HEADER_SIZE && recordCount>0) {
       // printf("Reading old file [%s] length[%llu] m_indexFileName[%s]\n", ackIndexFileName.data(), (unsigned long long)m_currentFileLength, m_indexFileName.data());
       // if (m_currentFileLength < HEADER_SIZE)
       //     throw dep::DEPException{"Corrupted ack index file(non-exist header): " + ackIndexFileName};

        m_pAckIndexFile->seekg(0, std::ios_base::beg);
        m_pAckIndexFile->read(reinterpret_cast<char*>(&m_firstNonAckSeq), sizeof(m_firstNonAckSeq));
        //printf("Reading old file m_recordCount[%llu]\n", (unsigned long long)m_recordCount);
        //printf("Reading old file m_firstNonAckSeq[%llu]\n", (unsigned long long)m_firstNonAckSeq);
        //printf("Reading old file currentLength[%llu]\n", (unsigned long long)m_currentFileLength);

        if (m_firstNonAckSeq - m_baseSequence < m_currentFileLength - HEADER_SIZE)
        {

            // start to read the non-ack sequence from following dep::byte
            // by using the the first nonAckSeq, we can start at the dep::byte it located
            uint64_t byteLocation = (m_firstNonAckSeq - m_baseSequence); // we start from firstNonAckSeq
            dep::ByteBuffer buffer(BUFFER_SIZE);
            m_pAckIndexFile->seekg(HEADER_SIZE + byteLocation, std::ios_base::beg);

            bool exit=false;
            while (!m_pAckIndexFile->eof() && !exit) {
                // seek to the position
                // printf("seek to %llu sequence[%llu]\n", (unsigned long long)HEADER_SIZE +byteLocation, (unsigned long long)byteLocation+m_baseSequence);


                buffer.clear();
                if (buffer.remaining() + HEADER_SIZE +byteLocation > m_currentFileLength) {
                    buffer.setLimit(m_currentFileLength - HEADER_SIZE - byteLocation);
              //      printf("set limit %llu \n", (unsigned long long)currentLength - HEADER_SIZE - byteLocation);

                }

                m_pAckIndexFile->read(buffer, buffer.remaining());

                buffer.flip();
              //  printf("buffer remaining =%u\n", (unsigned)buffer.remaining());

                while (buffer.remaining() > 0 ) {
                    uint8_t byte;
                    buffer >> byte;
                    uint64_t index = byteLocation + m_baseSequence;
                    if (index >= m_recordCount + m_baseSequence)
                        break;
                 //   printf("index = %llu m_recordCount=%llu\n", index, m_recordCount);


                //    printf("read old file sequence[%llu] byte[%u]\n", (unsigned long long)byteLocation + m_baseSequence, (unsigned)byte);
                    if (byte == NON_ACK_FRESH) {
                        if (m_largeBlockStart == 0) {
                            m_largeBlockStart = index;
                            m_largeBlockEnd   = index;
                        }
                        else {
                            m_largeBlockEnd = index;
                        }

                    }
                    else {
                        if (m_largeBlockStart != 0) {
                            for (uint64_t i=m_largeBlockStart; i<=m_largeBlockEnd; ++i) {
                                m_NonAckSet[i] = false;
                            }
                            m_largeBlockEnd = 0;
                            m_largeBlockStart = 0;
                        }
                        if (byte == NON_ACK_RETRANSMIT) {
                            m_NonAckSet[index] = true; // seq start from m_baseSequence, so need add m_baseSequence
                        }
                        // ttl expired message
                        if (byte == EXPIRED_ACKED){
                            m_ExpiredAckMap[index] = true;
                        }
                    }

                    ++byteLocation;
                    if (byteLocation >= m_currentFileLength - HEADER_SIZE ) {
                        exit = true;
                        break;
                   }
                }



            }

            if (m_firstNonAckSeq > m_recordCount + m_baseSequence) {
         //       printf("modify first nonAckSeq[%llu] currentRecordCount[%llu] modifiedFirstNonAckSeq[%llu]", (unsigned long long)m_firstNonAckSeq, (unsigned long long)m_recordCount, (unsigned long long)m_baseSequence + m_recordCount);
                m_firstNonAckSeq = m_baseSequence + m_recordCount;
            }


        }

        // we need to fill nonAck set if there are missing inside file
       // printf("recordCount %llu, currentLgnth %llu , HEADERISZE %llu\n", m_recordCount, currentLength, HEADER_SIZE);
        if (m_recordCount >  m_currentFileLength - HEADER_SIZE) {
            //printf("m_largeBlockStart %llu\n", m_largeBlockStart);
            if (m_largeBlockStart==0) {
                m_largeBlockStart = m_currentFileLength - HEADER_SIZE + m_baseSequence;
                m_largeBlockEnd =  m_recordCount -1 + m_baseSequence;
            }
            else {
                if (m_largeBlockEnd + 1 == m_currentFileLength - HEADER_SIZE + m_baseSequence) {
                    m_largeBlockEnd = m_recordCount - 1 + m_baseSequence;
                }
                else {
                    for (uint64_t i=m_largeBlockStart; i<=m_largeBlockEnd; ++i )
                        m_NonAckSet[i] = false; // push block into map
                    m_largeBlockStart = m_currentFileLength - HEADER_SIZE + m_baseSequence;
                    m_largeBlockEnd =  m_recordCount - 1 + m_baseSequence;
                }
            }
        }

        m_pAckIndexFile->seekg(0, std::ios_base::beg); // reset the point to beginning
    }
    else {
     //   printf("Reading new file [%s] length[%llu] m_indexFileName[%s]\n", ackIndexFileName.data(), (unsigned long long)currentLength, m_indexFileName.data());
        // write header first

        if (isDurable) {
            DEP_LOG_DEBUG2_CHECK(m_pLogger, "individual index file[", ackIndexFileName, "] is either not exist or corrupted");

            if (m_currentFileLength < HEADER_SIZE && m_currentFileLength!=0) {
                DEP_LOG_ERROR_CHECK(m_pLogger, "individual index file[", ackIndexFileName, "] is fixed for its length smaller than header, previousFileSize[", m_currentFileLength, "]");
            }
            m_pAckIndexFile->seekp(0, std::ios_base::beg);
            m_pAckIndexFile->write(reinterpret_cast<const char*>(&m_firstNonAckSeq) , sizeof(m_firstNonAckSeq));
            m_currentFileLength +=sizeof(m_firstNonAckSeq);

            m_firstNonAckSeq = m_baseSequence;
            if (m_recordCount== 0 ) {
                m_largeBlockEnd=0;
                m_largeBlockStart=0;
            }
            else {
                m_largeBlockStart = m_baseSequence;
                m_largeBlockEnd = m_baseSequence + m_recordCount -1;
            }
            m_pAckIndexFile->flush();
            m_pAckIndexFile->seekg(0, std::ios_base::beg); // reset the point to beginning
            // printf("initialize durable individual index m_recordCount[%llu] currentLength[%llu]\n\n", (unsigned long long)m_recordCount, (unsigned long long)m_pAckIndexFile->tellp());

        }
        else { // for non-durable, no need to write file
            m_firstNonAckSeq = m_baseSequence + m_recordCount;
        }
    }
}

template <class FileChannel>
void IndividualAckIndexFile<FileChannel>::addRecord(uint64_t toSeq) {
    DEP_LOG_DEBUG2_CHECK(m_pLogger, "individual index storeName[", m_storeName, "] addRecord, seq[", toSeq, "]");
    if (!m_isDurable)
        return;


    uint64_t countForSeq = toSeq - m_baseSequence + 1;
    int64_t totalCount = countForSeq - m_recordCount;
    uint64_t seq = (m_recordCount+1) + m_baseSequence-1; // the first nonAckSeq

   // printf("add record toSeq[%llu] totalCount[%lld] seq[%llu] countForSeq[%llu] recordCount[%llu]\n\n\n "
   //                 , toSeq, totalCount, seq, countForSeq, m_recordCount);
    if (totalCount<=0) {
        // if added sequnece smaller than our already known m_recordCount size
        // we need to verify our length of file and fill zero if needed
        DEP_LOG_DEBUG2_CHECK(m_pLogger, "individual index storeName[", m_storeName, "] addRecord, totalCount[", totalCount, "] m_currentFileLength[", m_currentFileLength, "] currentLength[", m_currentFileLength, "] countForSeq[", countForSeq, "]");

        if (m_currentFileLength - HEADER_SIZE < countForSeq) {
            uint64_t total = countForSeq - (m_currentFileLength - HEADER_SIZE);
            // printf("going to fill total[%llu] currentLength[%llu] \n", total, currentLength);
            DEP_LOG_DEBUG2_CHECK(m_pLogger, "individual index storeName[", m_storeName, "] addRecord, seq[", toSeq, "] going to fill total[", total, "] currentLength[", m_currentFileLength, "] this[", (uint64_t)this, "]");
            fillByte(m_pAckIndexFile.get(), total, NON_ACK_FRESH);
            m_currentFileLength +=total;
        } else {
			return;
		}
        /*
        else {
            printf("not fill currentLength[%llu] \n",  currentLength);
        }*/

    }
    else {

        if (m_NonAckSet.empty() && m_largeBlockStart==0) {// if non-ack set is empty, we have to update the first non-ack seq
            m_firstNonAckSeq = seq;
            m_pAckIndexFile->seekp(0, std::ios_base::beg);
            m_pAckIndexFile->write(reinterpret_cast<const char*>(&m_firstNonAckSeq) , sizeof(m_firstNonAckSeq));

        //    printf("addRecord update first non-ack seq[%llu]\n", (unsigned long long)m_firstNonAckSeq);
        }


        //  printf("addRecord currentLength[%llu] headerSize[%llu] numberOfByteRequired[%llu] seq[%llu]\n",
        //         (unsigned long long)m_currentLength, (unsigned long long)HEADER_SIZE,
        //        (unsigned long long)numberOfRequiredDataByte, (unsigned long long)seq);
        DEP_LOG_DEBUG2_CHECK(m_pLogger, "individual index storeName[", m_storeName, "] addRecord, seq[", toSeq, "] going to fill2 total[", totalCount, "] currentLength[", m_currentFileLength, "] m_recordCount[", m_recordCount, "] countForSeq[",countForSeq, "] this[", (uint64_t)this , "]" );

        fillByte(m_pAckIndexFile.get(), totalCount, NON_ACK_FRESH);
        m_currentFileLength +=totalCount;
        m_recordCount = countForSeq;

        if (m_largeBlockStart==0) {
            m_largeBlockStart=seq;
            m_largeBlockEnd=toSeq;
       //     printf("addRecord1 BlockStart %llu BlockEnd %llu\n", (unsigned long long)m_largeBlockStart, (unsigned long long)m_largeBlockEnd);
        }
        else {
            if (m_largeBlockEnd ==seq+1) {
                // it is continuous, this should be most likely
                m_largeBlockEnd = toSeq;
        //        printf("addRecord2 BlockStart %llu BlockEnd %llu\n", (unsigned long long)m_largeBlockStart, (unsigned long long)m_largeBlockEnd);

            }
            else {
                // flush the large box into map, and replace it with the new sequence
                for (uint64_t i=m_largeBlockStart;i<=m_largeBlockEnd;++i) {
                    m_NonAckSet[i]=false;
                }
                m_largeBlockStart=seq;
                m_largeBlockEnd=toSeq;

           //     printf("addRecord3 BlockStart %llu BlockEnd %llu\n", (unsigned long long)m_largeBlockStart, (unsigned long long)m_largeBlockEnd);

            }
        }

    }

    m_pAckIndexFile->flush();
    if (m_enableSyncMode) {
        m_pAckIndexFile->sync();
    }
}

template <class FileChannel>
void IndividualAckIndexFile<FileChannel>::ackRecord(uint64_t seq) {
    DEP_LOG_DEBUG2_CHECK(m_pLogger, "individual index storeName[", m_storeName, "] ackRecord, seq[", seq, "]");

    if (!m_isDurable)
        return;

    if (seq - m_baseSequence +1 > m_recordCount)
        addRecord(seq); // add to seq

    uint64_t sequence= seq-m_baseSequence; // because our sequence start from baseSequence
    static dep::byte record = ACKED;

  //  printf("ackRecord storeName[%s] storePath[%s] recordCount[%llu] seq[%llu] sizeof[%u]\n\n", m_storeName.data(), m_storePath.data(), (unsigned long long)m_recordCount, (unsigned long long) seq, sizeof(record));
  //  printf("before write, seek to %llu\n",(unsigned long long) HEADER_SIZE+sequence);

    m_pAckIndexFile->seekp(HEADER_SIZE + sequence, std::ios_base::beg);
    m_pAckIndexFile->write(reinterpret_cast<char*>(&record), sizeof(record));

    if (m_largeBlockStart!=0 && seq <=m_largeBlockEnd && seq >= m_largeBlockStart) {
        // if the acked seq inside large box, we put the first half to map, and resize the box
        for (uint64_t i =m_largeBlockStart; i<seq;++i) {
            m_NonAckSet[i]=false;
        }
        m_largeBlockStart = seq + 1;

        if (m_largeBlockStart > m_largeBlockEnd) { // handle if seq = largeBlockEnd, our box should be empty
            m_largeBlockStart=0;
            m_largeBlockEnd = 0;
        }

    }
    else {
        m_NonAckSet.erase(seq);


        // update first non-ack seq and write it into file
        if ( m_NonAckSet.empty() || m_firstNonAckSeq != m_NonAckSet.begin()->first) {
            if (m_NonAckSet.empty()) {
                if (m_largeBlockStart==0)
                    m_firstNonAckSeq = m_recordCount + m_baseSequence; // point to non-existing sequence
                else
                    m_firstNonAckSeq = m_largeBlockStart;
            }
            else {
                m_firstNonAckSeq = m_NonAckSet.begin()->first;
            }
            m_pAckIndexFile->seekp(0, std::ios_base::beg);
            m_pAckIndexFile->write(reinterpret_cast<char*>(&m_firstNonAckSeq), sizeof(m_firstNonAckSeq));
        //    printf("ackRecord update first non-ack seq[%llu]\n", (unsigned long long)m_firstNonAckSeq);
        }

    }


    m_pAckIndexFile->flush();
    if (m_enableSyncMode) {
        m_pAckIndexFile->sync();
    }
}

template <class FileChannel>
void IndividualAckIndexFile<FileChannel>::ackRecord(uint8_t level, uint64_t seq) {
    ackRecord(seq);
}

template <class FileChannel>
void IndividualAckIndexFile<FileChannel>::markRetransmit(uint64_t seq) {
    DEP_LOG_DEBUG2_CHECK(m_pLogger, "individual index storeName[", m_storeName, "] markRetransmit, seq[", seq, "]");
    if (!m_isDurable)
        return;

    if (seq - m_baseSequence +1 > m_recordCount)
        addRecord(seq);

    uint64_t sequence= seq-m_baseSequence; // because our sequence start from baseSequence
    static dep::byte record = NON_ACK_RETRANSMIT;

  //  printf("ackRecord storeName[%s] storePath[%s] recordCount[%llu] seq[%llu] sizeof[%u]\n\n", m_storeName.data(), m_storePath.data(), (unsigned long long)m_recordCount, (unsigned long long) seq, sizeof(record));
  //  printf("before write, seek to %llu\n",(unsigned long long) HEADER_SIZE+sequence);


    auto result = m_NonAckSet.find(seq);
    if (result!=m_NonAckSet.end()) {
        result->second=true;
    }
    else if (m_largeBlockStart!=0 && seq <=m_largeBlockEnd && seq >= m_largeBlockStart) {
        // if the acked seq inside large box, we put the first half to map, and resize the box
        for (uint64_t i =m_largeBlockStart; i<seq;++i) {
            m_NonAckSet[i]=false;
        }
        m_NonAckSet[seq]=true; // put the seq into the map as retransmited as well
        m_largeBlockStart = seq +1;

        if (m_largeBlockStart > m_largeBlockEnd) { // handle if seq = largeBlockEnd, our box should be empty
            m_largeBlockStart=0;
            m_largeBlockEnd = 0;
        }
    }
    else {
        return; // if non in map and non in box, then it may be already acked, ignore the request
    }
    m_pAckIndexFile->seekp(HEADER_SIZE + sequence, std::ios_base::beg);
    m_pAckIndexFile->write(reinterpret_cast<char*>(&record), sizeof(record));
    m_pAckIndexFile->flush();
    if (m_enableSyncMode) {
        m_pAckIndexFile->sync();
    }
}

template <class FileChannel>
void IndividualAckIndexFile<FileChannel>::getNonAckRecord(std::map<uint64_t, bool>& result, uint64_t& largeBlockStart, uint64_t& largeBlockEnd) {
    if (!m_isDurable)
        return;
    result = m_NonAckSet; // direct copy the set
    largeBlockStart = m_largeBlockStart ;
    largeBlockEnd = m_largeBlockEnd;
}

template <class FileChannel>
void IndividualAckIndexFile<FileChannel>::getAllCursor(MultiSequence &seq)
{
    // 暂未实现,未使用
    return ;
}

template <class FileChannel>
void IndividualAckIndexFile<FileChannel>::getExpiredAckRecord(std::map<uint64_t, bool>& result, uint64_t& largeBlockStart, uint64_t& largeBlockEnd) {
    if (!m_isDurable)
        return;
    result = m_ExpiredAckMap; // direct copy the set
    largeBlockStart = m_largeBlockStart ;
    largeBlockEnd = m_largeBlockEnd;
}

template <class FileChannel>
std::string IndividualAckIndexFile<FileChannel>::getAckIndexFileName() const {
    return m_storePath + m_storeName + FILE_NAME_DELIMITER + m_indexFileName + ACK_INDEX_EXTENSION;
}

template <class FileChannel>
void IndividualAckIndexFile<FileChannel>::flush() {
    if (m_isDurable && m_pAckIndexFile)
        m_pAckIndexFile->flush();
}
template <class FileChannel>
void IndividualAckIndexFile<FileChannel>::close() {
    if (m_isDurable && m_pAckIndexFile && m_pAckIndexFile->is_open())
        m_pAckIndexFile->close();
}
template <class FileChannel>
bool IndividualAckIndexFile<FileChannel>::getFirstNonAckRecord(uint64_t& sequence) {
    sequence =  m_firstNonAckSeq;
    return true;
    //throw dep::DEPException("IndividualAckIndexFile<FileChannel>::getFirstNonAckRecord function not support!");
}


}} // namespace lightmq { namespace util {

#endif /* INDIVIDUALACKINDEXFILE_H_ */
