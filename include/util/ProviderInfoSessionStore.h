////////////////////////////////////////////////////////////////////////////////
//
// ProviderInfoSessionStore.h
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

#ifndef PROVIDERINFOSESSIONSTORE_H_
#define PROVIDERINFOSESSIONSTORE_H_


#include <unordered_map>

#include "IProviderInfoSessionStore.h"
#include "CRC.h"
#include "StringUtility.h"
#include "BrokerConstant.h"
#include "ISessionStoreInterface.h"
#include "Message.h"

namespace lightmq { namespace util {

using namespace lightmq::broker;

template <class FileChannel>
class ProviderInfoSessionStore : public IProviderInfoSessionStore {
private:
    static constexpr auto STORE_EXTENSION = "PIS"; // Provider Info SessionStore
    static constexpr auto FILE_NAME_DELIMITER = ".";
    static constexpr auto PROVIDER_INFO_BUFFER=65573; // this one is 16bit clientID + 2 dep::byte clientID length + privderMessageID + consumerMessageID + CRC

public:
    ProviderInfoSessionStore(const std::string& storePath,
                             const std::string& storeName,
                             bool enableSyncMode=false,
                             std::function<std::unique_ptr<FileChannel>()>&& createFileChannelFunc = defaultCreateFileChannelFunc<FileChannel>);
    ~ProviderInfoSessionStore();

    // no copying!
    ProviderInfoSessionStore(const ProviderInfoSessionStore&)=delete;
    ProviderInfoSessionStore& operator=(const ProviderInfoSessionStore&)=delete;

    void init() override;
    void updateRecord(const std::string& clientName, MessageID providerMessageID, MessageID brokerMessageID) override;
    ProviderMessageInfo* getProviderMessageInfoRecord(const std::string& clientName) override;
    void rebuild(std::unordered_map<SessionID, std::shared_ptr<ISessionStoreInterface>>& sessionStoreMap);
private:
    std::string getStoreFileName() const;
    void writeInfoIntoFile(const std::string& clientName,ProviderMessageInfo& info, bool isNewRecord);

    std::unordered_map<std::string, ProviderMessageInfo> m_providerMessageInfoMap;

    std::unique_ptr<FileChannel> m_pProviderInfoFile;
    std::string m_storePath;
    std::string m_storeName;
    dep::ByteBuffer m_bufferForRecord;
    dep::CRC m_crc;
    bool m_enableSyncMode;
    std::recursive_mutex m_pisLock;
};

template <class FileChannel>
ProviderInfoSessionStore<FileChannel>::ProviderInfoSessionStore(const std::string& storePath,
                                                                const std::string& storeName,
                                                                bool enableSyncMode,
                                                                std::function<std::unique_ptr<FileChannel>()>&& createFileChannelFunc) :
                                                                m_bufferForRecord(PROVIDER_INFO_BUFFER), m_enableSyncMode(enableSyncMode) {

    m_storePath = storePath;
    m_storeName = storeName;


    m_pProviderInfoFile = createFileChannelFunc();
}

template <class FileChannel>
ProviderInfoSessionStore<FileChannel>::~ProviderInfoSessionStore() {
    if (m_pProviderInfoFile->is_open())
        m_pProviderInfoFile->close();

}

template <class FileChannel>
void ProviderInfoSessionStore<FileChannel>::init() {
    std::lock_guard<std::recursive_mutex> lock(m_pisLock);
    const auto storeFileName = getStoreFileName();
    auto isFileExists = dep::FileUtility::isFileExist(storeFileName);

    auto openMode = std::ios_base::in | std::ios_base::out
                    | (isFileExists? std::ios_base::ate : std::ios_base::trunc);

    //printf("ProviderInfoSessionStore<FileChannel>::init() opening file[%s]\n", storeFileName.data());

    m_pProviderInfoFile->open(storeFileName, openMode);
    if (!m_pProviderInfoFile->is_open())
        throw dep::DEPException{"Fail to open time index file: " + storeFileName};
    uint64_t currentLength = m_pProviderInfoFile->tellp();

    if (currentLength ==0) {
        return; // no need to initial from old data file
    }

    // load from data file and reconstruct the data structure
    m_pProviderInfoFile->seekg(0, std::ios_base::beg);
    m_bufferForRecord.clear();
    uint32_t lastValidPos=0;
    uint32_t recordPos =0;
    bool hasHalfRecord = false;
    try {
        while (!m_pProviderInfoFile->eof()) {
            //printf("checking start pos[%u] limit[%u]\n", m_bufferForRecord.getPosition(), m_bufferForRecord.getLimit());
            m_pProviderInfoFile->read(m_bufferForRecord, m_bufferForRecord.remaining());
            m_bufferForRecord.flip();
            //printf("after read start pos[%u] limit[%u]\n", m_bufferForRecord.getPosition(), m_bufferForRecord.getLimit());

        //    printf("read whole file \n%s\n", dep::StringUtility::hexdump((const char*)(m_bufferForRecord.getArray() ), m_bufferForRecord.remaining()).data());
            try {
                while (m_bufferForRecord.remaining()) {
                    hasHalfRecord = true;
                  //  printf("start cal pos %u\n", (unsigned)m_bufferForRecord.getPosition());
                    ProviderMessageInfo info;

                    info.m_recordPosition = recordPos;
                    lastValidPos = m_bufferForRecord.getPosition();
                  //  printf("recordPos=%u\n",(unsigned)recordPos);

                    uint32_t crc= 0;
                    uint16_t clientNameLength=0;


                    uint32_t beginPos = m_bufferForRecord.getPosition();
                    m_bufferForRecord >> clientNameLength;
                    std::string clientName;
                    m_bufferForRecord.get(clientName, clientNameLength);
                    m_bufferForRecord >>info.providerMessageID.sessionID;
                    m_bufferForRecord >>info.providerMessageID.sequence;
                    m_bufferForRecord >>info.brokerMessageID.sessionID;
                    m_bufferForRecord >>info.brokerMessageID.sequence;
                    uint32_t endPos = m_bufferForRecord.getPosition();
                    m_bufferForRecord >> crc;


                    m_crc.reset();
                    m_crc.update(m_bufferForRecord.getArray() + beginPos , endPos - beginPos);
                    uint32_t trueCRC = m_crc.getValue();

                    //printf("checking based on \n%s\n", dep::StringUtility::hexdump((const char*)(m_bufferForRecord.getArray() + beginPos), endPos - beginPos).data());

                    if (trueCRC!=crc)
                        throw dep::DEPException("CRC does not match");

                    m_providerMessageInfoMap[clientName] = std::move(info);
                    recordPos = m_bufferForRecord.getPosition() +recordPos;
      //              printf("after cal pos %u remaining %u\n", (unsigned)m_bufferForRecord.getPosition(), (unsigned)m_bufferForRecord.remaining());
                    hasHalfRecord = false;
                }
                m_bufferForRecord.compact();
            }
            catch (dep::OverflowException& e) {
        //        printf("catch overflow exception\n");
                m_bufferForRecord.setPosition(lastValidPos);
                m_bufferForRecord.compact();
            }
            catch (dep::RangeException& e) {
        //        printf("RangeException exception\n");
                m_bufferForRecord.setPosition(lastValidPos);
                m_bufferForRecord.compact();
            }
        }
        if (hasHalfRecord) {
            throw dep::DEPException("exception throw because there are invalid data left");
        }


    }
    catch (std::exception& e) {
        // catch any exception, we need to reset the state and use other session store for reconstruction
      //  printf("any exception catch\n");
        m_pProviderInfoFile->close();
        m_pProviderInfoFile->open(storeFileName, std::ios_base::in | std::ios_base::out
                                  | std::ios_base::trunc); // we open it again and truncate it
        m_providerMessageInfoMap.clear();
        m_pProviderInfoFile->seekg(0, std::ios_base::beg);
        m_pProviderInfoFile->seekp(0, std::ios_base::beg);
        throw;
    }

    m_pProviderInfoFile->seekg(0, std::ios_base::beg);
    m_pProviderInfoFile->seekp(0, std::ios_base::beg);
}



template <class FileChannel>
void ProviderInfoSessionStore<FileChannel>::updateRecord(const std::string& clientName, MessageID providerMessageID, MessageID brokerMessageID) {

    std::lock_guard<std::recursive_mutex> lock(m_pisLock);
    auto iter = m_providerMessageInfoMap.find(clientName);
   // printf("updateRecord client[%s] providerMessageID[%llu:%llu] brokerID[%llu:%llu]\n",clientName.data(), providerMessageID.sessionID, providerMessageID.sequence, brokerMessageID.sessionID, brokerMessageID.sequence);
    if (iter==m_providerMessageInfoMap.end()) {
        // we create a new record in both file and map
        ProviderMessageInfo info;
        info.providerMessageID= providerMessageID;
        info.brokerMessageID = brokerMessageID;
        writeInfoIntoFile(clientName, info, true);
        m_providerMessageInfoMap[clientName] = std::move(info);
    }
    else {
        // we update the record in both file and map
        ProviderMessageInfo& info=iter->second;
        if (brokerMessageID > info.brokerMessageID) {
            info.providerMessageID= providerMessageID;
            info.brokerMessageID = brokerMessageID;
            writeInfoIntoFile(clientName, info, false);
        }
    }
}



template<class FileChannel>
ProviderMessageInfo* ProviderInfoSessionStore<FileChannel>::getProviderMessageInfoRecord(const std::string& clientName) {
    std::lock_guard<std::recursive_mutex> lock(m_pisLock);
    auto iter = m_providerMessageInfoMap.find(clientName);

    // NONG-SHA?
    /*
    for(auto& kv : m_providerMessageInfoMap){
        auto x = kv.first + "x";
    }
    */

    if (iter==m_providerMessageInfoMap.end()) {
        return nullptr;
    }
    else {
        return &(iter->second);
    }
}

template <class FileChannel>
std::string ProviderInfoSessionStore<FileChannel>::getStoreFileName() const {
    return m_storePath + m_storeName + FILE_NAME_DELIMITER +  STORE_EXTENSION;
}

template <class FileChannel>
void ProviderInfoSessionStore<FileChannel>::writeInfoIntoFile(const std::string& clientName, ProviderMessageInfo& info, bool isNewRecord) {
    if (isNewRecord) {
        // new record
        m_pProviderInfoFile->seekp(0, std::ios_base::end);
        info.m_recordPosition = m_pProviderInfoFile->tellp();
    //    printf("start writing new record\n");
    }
    else {
        m_pProviderInfoFile->seekp(info.m_recordPosition, std::ios_base::beg);
    //    printf("start update record pos %u \n", (unsigned) info.m_recordPosition);
    }

    // printf("clientName size %u\n",(unsigned)clientName.size());
    uint16_t clientNameLength = clientName.size();
    uint32_t crcValue = 0;

    m_bufferForRecord.clear();

    uint32_t beginPos = m_bufferForRecord.getPosition();
    m_bufferForRecord << clientNameLength;
    m_bufferForRecord << clientName;
    m_bufferForRecord << info.providerMessageID.sessionID;
    m_bufferForRecord << info.providerMessageID.sequence;
    m_bufferForRecord << info.brokerMessageID.sessionID;
    m_bufferForRecord << info.brokerMessageID.sequence;
    uint32_t endPos =  m_bufferForRecord.getPosition();

    m_crc.reset();
    m_crc.update(m_bufferForRecord.getArray() + beginPos , endPos - beginPos);
    //printf("updating based on \n%s\n", dep::StringUtility::hexdump((const char*)(m_bufferForRecord.getArray() + beginPos), endPos - beginPos).data());


    crcValue = m_crc.getValue();
    m_bufferForRecord << crcValue;
    /*
    printf("writeInfoIntoFile calculated crc:%u\n", (unsigned)crcValue);
    if (clientName.length() < 100)
        printf("writeInfoIntoFile calcuated based on \n%s\n", dep::StringUtility::hexdump((const char*)(m_bufferForRecord.getArray() + afterCRCPos), endPos - afterCRCPos).data());
    else
        printf("writeInfoIntoFile calcuated based on \n%s\n", dep::StringUtility::hexdump((const char*)(m_bufferForRecord.getArray() + afterCRCPos), 100).data());
*/


    m_bufferForRecord.flip();


    m_pProviderInfoFile->write(m_bufferForRecord);
    m_pProviderInfoFile->flush();
    if (m_enableSyncMode) {
        m_pProviderInfoFile->sync();
    }
}

template <class FileChannel>
void ProviderInfoSessionStore<FileChannel>::rebuild(std::unordered_map<SessionID, std::shared_ptr<ISessionStoreInterface>>& sessionStoreMap) {
    std::lock_guard<std::recursive_mutex> lock(m_pisLock);
    std::unique_ptr<dep::ByteBuffer> pBuffer;

    m_providerMessageInfoMap.clear();

    if (m_pProviderInfoFile->is_open())
        m_pProviderInfoFile->close();

    auto openMode = std::ios_base::in | std::ios_base::out
                    | (std::ios_base::trunc);

    const auto storeFileName = getStoreFileName();
    m_pProviderInfoFile->open(storeFileName, openMode);
    if (!m_pProviderInfoFile->is_open())
        throw dep::DEPException{"Fail to open time index file: " + storeFileName};

    m_pProviderInfoFile->seekg(0, std::ios_base::beg);
    m_pProviderInfoFile->seekp(0, std::ios_base::beg);

    if (sessionStoreMap.empty())
        return;

    pBuffer.reset(new dep::ByteBuffer(BrokerConstant::DEFAULT_TCP_MSG_BUFFER_SIZE+ BrokerConstant::DEFAULT_TCP_EXTRA_HEADER_SIZE));

    for (auto iter= sessionStoreMap.begin();iter!=sessionStoreMap.end();++iter) {
        SessionID sessionID = iter->first;
        auto pStore = iter->second;
        uint64_t count = pStore->getRecordCount();
        for (uint32_t i=1;i<= count ;++i) {
            pBuffer->clear();
            pStore->readSingleMessageBuffer(i, *(pBuffer.get()));
            pBuffer->flip();
			// dep::ByteBuffer& buffer = *pBuffer;
			// printf( "message \n%s\n",
			//                        dep::StringUtility::hexdump((const char*)buffer.getArray()+buffer.getPosition(),buffer.remaining()).data());

			protocol::MessageSerializeAgent msgAgent;
			msgAgent.parseMessage(*pBuffer, protocol::MessageSerializeAgent::FIELD_MASK_PROVIDER_INFO);

			MessageID brokerMessageID(sessionID, i);
			updateRecord(msgAgent.m_providerID, msgAgent.m_providerMessageID, brokerMessageID);
        }
    }

}

}} // namespace lightmq { namespace util {

#endif /* PROVIDERINFOSESSIONSTORE_H_ */
