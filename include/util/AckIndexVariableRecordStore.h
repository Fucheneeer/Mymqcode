////////////////////////////////////////////////////////////////////////////////
//
// AckIndexVariableRecordStore.h
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

#ifndef ACKINDEXVARIABLERECORDSTORE_H_
#define ACKINDEXVARIABLERECORDSTORE_H_

#include <string>
#include <chrono>
#include <tuple>
#include <mutex>
#include <set>
#include <unordered_map>
#include <memory>
#include <functional>


#include "VariableRecordStorePersist.h"
#include "VariableRecordStoreMemory.h"
#include "ByteBuffer.h"
#include "DEPException.h"
#include "FileUtility.h"
#include "BufferedFileStream.h"
#include "IndividualAckIndexFile.h"
#include "SequentialAckIndexFile.h"
#include "FsyncFileStream.h"
#include "ISeverityLog.h"
#include "MockMemoryStream.h"
#include "BrokerTypes.h"
#include "ISeverityLog.h"

namespace lightmq { namespace util {
class VariantMap;

template <class DataFormat, class LockType = dep::NoLock, class FileChannel = dep::BufferedFileStream>
class AckIndexVariableRecordStore {

public:
    AckIndexVariableRecordStore(dep::ISeverityLog* pLogger
                                , SessionID sessionID
                                , const std::string& storePath
                                , const std::string& storeName
                                , UnCompleteGroupMessageManagerPtr &ucgmmp 
                                , bool enableSyncMode=false
                                , const typename VariableRecordStoreBase<LockType>::StoreOptions& options = typename VariableRecordStoreBase<LockType>::StoreOptions{}
                                , const PersistencyType &type = PersistencyType::Persistent
                                , std::function<std::unique_ptr<FileChannel>()>&& createFileChannelFunc = defaultCreateFileChannelFunc<FileChannel>);
    virtual ~AckIndexVariableRecordStore() =default;

    bool addAckIndexFile(const std::string& name, bool isDurable=false, bool isSequential=false, bool eraseIfExist=true);
    void addAckRecordToSequence(const std::string& name, uint64_t toSeq);
	uint64_t add(RecordStoreAddMessageParam &addParam);
    bool ackRecord(const std::string& name, uint64_t seq);
    bool ackRecord(const std::string& name, uint8_t level, uint64_t seq);
	bool isRecordAcked(const std::string& name, uint64_t seq);
	bool isRecordAcked(const std::string& name, uint8_t level, uint64_t seq);
    bool getNonAckRecord(const std::string& name,std::map<uint64_t,bool>& result, uint64_t& largeBoxStart, uint64_t& largeBoxEnd);
    bool getExpiredAckRecord(const std::string& name,std::map<uint64_t,bool>& result, uint64_t& largeBoxStart, uint64_t& largeBoxEnd);
    bool getFirstNonAckRecord(const std::string& name, uint64_t& result);
    bool getMinNonAckRecord(uint64_t& result);
    bool markRetransmitRecord(const std::string& name, uint64_t seq);
    bool checkIndexFileExist(const std::string& name, bool isSequential);
    std::vector<std::string> getAckedBy(uint64_t seq);

    void setSequenceReady(uint64_t seq);
    void setSequenceNotReady(uint64_t seq);
    //uint32_t getManyWithPrefixSuffixReady(uint64_t start, uint64_t end, dep::ByteBuffer& buffer);
    void setAllSequenceReady();

    template <class OutputDataFormat>
    uint32_t getManyWithPrefixSuffix(uint64_t start, dep::ByteBuffer& buffer);
    template <class OutputDataFormat>
    uint32_t getManyWithPrefixSuffix(uint64_t start, uint64_t end, dep::ByteBuffer& buffer);
    uint32_t getMany(uint64_t start, uint64_t end, dep::ByteBuffer& buffer, std::pair<uint64_t, uint64_t> &startEndPosition, uint64_t& expiredCount);
    uint32_t getManyReady(uint64_t start, uint64_t end, dep::ByteBuffer& buffer, std::pair<uint64_t, uint64_t> &startEndPosition, uint64_t& expiredCount);

    uint64_t getRecordCount();
    void readSingleMessageBuffer(uint64_t seq, dep::ByteBuffer& buffer);
    void markDeleted(uint64_t start, uint64_t end);
    bool isDeletedOrRemoteMessage(uint64_t seq) const;
    uint8_t getTransState(uint64_t seq);
    std::vector<uint64_t> getDeletedSequences(uint64_t start, uint64_t end) const;
    bool markExpired(uint64_t start, uint64_t end);
    std::vector<uint64_t> getExpiredSequences(uint64_t start, uint64_t end) const;
    std::vector<uint64_t> getRemoteSequences(uint64_t start, uint64_t end) const;
    std::vector<uint64_t> getRemoteOrDeletedSequences(uint64_t start, uint64_t end) const;
    const VariantMap* getMessageProperties(uint64_t sequence) const;
    std::vector<uint64_t> getNotDeletedSequences() const;
    uint64_t getLastHasProperitesMessageSequence() const;
    uint64_t getOldestDeletedSequence() const;
    bool isDeleted(uint64_t seq) const;
    std::tuple<uint64_t, uint64_t> getLastLocalMessageSequence() const;
	uint64_t getBeginSequence() const;
	void setNewBeginSequence(uint64_t newBeginSequence);
	void lock();
	void unlock();

    std::vector<uint64_t> queryMessageIDs(const std::string& providerID, std::shared_ptr<ISQLExpression> spSelector, const std::string& msgKeyWord, uint64_t startTime, uint64_t endTime, uint64_t timeoutMs, bool& searchCompleted);
	std::shared_ptr<IAckIndexFile> getConsumerAckMana(const std::string &name);
    GroupMessage* getGroupMessagePtr(const MessageID &messageID) const ;
    bool checkGroupMessage(const MessageID &begin, uint64_t seq, IVariableRecordStore::ChkGroupResult *first) const ;
    void getAllCursor(const std::string &indexFileName, MultiSequence &cursor) const;
    bool checkDefaultLevel(uint64_t startSeq, uint64_t endSeq, IVariableRecordStore::ChkLevelResult *result) const ;
    uint8_t getRecordLevel(uint64_t sequence) const ;
    bool getLevelBrief(uint8_t lev, uint64_t *count, uint64_t *firstSeq, uint64_t *lastSeq) const ;
    uint64_t getFirstNoAckByLevel(uint8_t lev, uint64_t cursor) const ;

private:
    dep::ISeverityLog* m_pLogger;
    std::unordered_map<std::string, std::shared_ptr<IAckIndexFile>> m_ackIndexFileMap;
    std::string m_storePath;
    std::string m_storeName;
    bool m_enableSyncMode;
    std::shared_ptr<IVariableRecordStoreLocked<LockType> > m_pLightmqSessionStore;
    PersistencyType m_persistMode;
    uint32_t m_baseSequence;       /**< index of the base record */
    typename VariableRecordStoreBase<LockType>::StoreOptions m_options;

};



template <class DataFormat, class LockType /*= dep::NoLock*/, class FileChannel /*= dep::BufferedFileStream*/>
std::vector<uint64_t> lightmq::util::AckIndexVariableRecordStore<DataFormat, LockType, FileChannel>::queryMessageIDs(const std::string& providerID, std::shared_ptr<ISQLExpression> spSelector, const std::string& msgKeyWord, uint64_t startTime, uint64_t endTime, uint64_t timeoutMs, bool& searchCompleted)
{
    return m_pLightmqSessionStore->queryMessageIDs(providerID, spSelector, msgKeyWord, startTime, endTime, timeoutMs, searchCompleted);
}

template <class DataFormat, class LockType /*= dep::NoLock*/, class FileChannel /*= dep::BufferedFileStream*/>
void AckIndexVariableRecordStore<DataFormat, LockType, FileChannel>::lock()
{
	m_pLightmqSessionStore->getLock().lock();
}

template <class DataFormat, class LockType /*= dep::NoLock*/, class FileChannel /*= dep::BufferedFileStream*/>
void AckIndexVariableRecordStore<DataFormat, LockType, FileChannel>::unlock()
{
	m_pLightmqSessionStore->getLock().unlock();

}

template <class DataFormat, class LockType, class FileChannel>
AckIndexVariableRecordStore<DataFormat, LockType, FileChannel>::AckIndexVariableRecordStore(
        dep::ISeverityLog* pLogger
            , SessionID sessionID
            , const std::string& storePath
            , const std::string& storeName
            , UnCompleteGroupMessageManagerPtr &ucgmmp 
            , bool enableSyncMode
            , const typename VariableRecordStoreBase<LockType>::StoreOptions& options
            , const PersistencyType &type
            , std::function<std::unique_ptr<FileChannel>()>&& createFileChannelFunc)

            : m_pLogger(pLogger)
            , m_storePath(storePath)
            , m_storeName(storeName)
            , m_enableSyncMode(enableSyncMode)
            , m_persistMode(type)
            , m_baseSequence(m_options.baseSequence)
            ,  m_options(options)
{
    //m_persistMode = PersistencyType::InMemory;
    if(m_persistMode == PersistencyType::Persistent) {
        m_pLightmqSessionStore = std::shared_ptr<VariableRecordStorePersist<DataFormat, LockType, FileChannel>>(new VariableRecordStorePersist<DataFormat, LockType, FileChannel>(sessionID, storePath, storeName, ucgmmp, options, std::move(createFileChannelFunc)));
    } else if(m_persistMode == PersistencyType::InMemory){
        m_pLightmqSessionStore = std::shared_ptr<VariableRecordStoreMemory<LockType>>(new VariableRecordStoreMemory<LockType>(sessionID, storeName, ucgmmp
                              , options
                              , std::bind(&AckIndexVariableRecordStore::getMinNonAckRecord, this, std::placeholders::_1)
                                                              , pLogger));
    } else{
        throw dep::DEPException{"Session store[" + m_storeName + "] persist mode[" + AddressTypeUtil::getPersistencyTypeString(m_persistMode) + "]is invalid."};
    }
}
template <class DataFormat, class LockType, class FileChannel>
bool AckIndexVariableRecordStore<DataFormat, LockType, FileChannel>::addAckIndexFile(const std::string& name, bool isDurable, bool isSequential, bool eraseIfExist) {
    std::lock_guard<LockType> lock(m_pLightmqSessionStore->getLock());
    auto iter = m_ackIndexFileMap.find(name);
    if (iter!=m_ackIndexFileMap.end()) {
        if (eraseIfExist)
           m_ackIndexFileMap.erase(iter);
        else
            return false;
    }
    if (isSequential){
        if(m_persistMode == PersistencyType::Persistent){
            m_ackIndexFileMap[name].reset(new SequentialAckIndexFile<dep::FsyncFileStream>(m_pLightmqSessionStore.get(), m_storePath, m_storeName, name, m_options.deleteOnExit, m_enableSyncMode));
        } else if(m_persistMode == PersistencyType::InMemory){
            m_ackIndexFileMap[name].reset(new SequentialAckIndexFile<MockMemoryStream>(m_pLightmqSessionStore.get(), m_storePath, m_storeName, name, m_options.deleteOnExit, m_enableSyncMode));
        } else {
            throw dep::DEPException{"Session store[" + m_storeName + "] persist mode[" + AddressTypeUtil::getPersistencyTypeString(m_persistMode) + "]is invalid."};
        }
    }
    else{
        if(m_persistMode == PersistencyType::Persistent){
            m_ackIndexFileMap[name].reset(new IndividualAckIndexFile<dep::FsyncFileStream>(m_pLogger, m_storePath, m_storeName, name, m_options.deleteOnExit, m_enableSyncMode));
        } else if(m_persistMode == PersistencyType::InMemory){
            m_ackIndexFileMap[name].reset(new IndividualAckIndexFile<MockMemoryStream>(m_pLogger, m_storePath, m_storeName, name, m_options.deleteOnExit, m_enableSyncMode));
        } else {
            throw dep::DEPException{"Session store[" + m_storeName + "] persist mode[" + AddressTypeUtil::getPersistencyTypeString(m_persistMode) + "]is invalid."};
        }
    }

    m_ackIndexFileMap[name]->init(m_options.append, m_options.baseSequence, m_pLightmqSessionStore->getRecordCount(), isDurable);
    return true;
}

template <class DataFormat, class LockType, class FileChannel>
void AckIndexVariableRecordStore<DataFormat, LockType, FileChannel>::addAckRecordToSequence(const std::string& name, uint64_t toSeq) {
    std::lock_guard<LockType> lock(m_pLightmqSessionStore->getLock());
    auto iter = m_ackIndexFileMap.find(name);
    if (iter==m_ackIndexFileMap.end())
            return;
    iter->second->addRecord(toSeq);
}

template <class DataFormat, class LockType, class FileChannel>
uint64_t AckIndexVariableRecordStore<DataFormat, LockType, FileChannel>::add(RecordStoreAddMessageParam &addParam)
{
    std::lock_guard<LockType> lock(m_pLightmqSessionStore->getLock());
    uint64_t res = m_pLightmqSessionStore->add(addParam);
    if (m_enableSyncMode) {
        m_pLightmqSessionStore->sync();
    }
	return res;
}

template <class DataFormat, class LockType, class FileChannel>
std::shared_ptr<IAckIndexFile> AckIndexVariableRecordStore<DataFormat, LockType, FileChannel>::getConsumerAckMana(const std::string &name) {
	std::lock_guard<LockType> lock(m_pLightmqSessionStore->getLock());
	auto iter = m_ackIndexFileMap.find(name);
	if (iter == m_ackIndexFileMap.end())
		return std::shared_ptr<IAckIndexFile>();

	return iter->second;
}

template <class DataFormat, class LockType, class FileChannel>
bool AckIndexVariableRecordStore<DataFormat, LockType, FileChannel>::ackRecord(const std::string& name,uint64_t seq) {
    std::lock_guard<LockType> lock(m_pLightmqSessionStore->getLock());
    auto iter = m_ackIndexFileMap.find(name);
    if (iter==m_ackIndexFileMap.end())
        return false;
    iter->second->ackRecord(seq);
    return true;
}

template <class DataFormat, class LockType, class FileChannel>
bool AckIndexVariableRecordStore<DataFormat, LockType, FileChannel>::ackRecord(const std::string& name, uint8_t level,uint64_t seq) {
    std::lock_guard<LockType> lock(m_pLightmqSessionStore->getLock());
    auto iter = m_ackIndexFileMap.find(name);
    if (iter==m_ackIndexFileMap.end())
        return false;
    iter->second->ackRecord(level, seq);
    return true;
}

template <class DataFormat, class LockType, class FileChannel>
bool AckIndexVariableRecordStore<DataFormat, LockType, FileChannel>::isRecordAcked(const std::string& name, uint8_t level, uint64_t seq) {
	std::lock_guard<LockType> lock(m_pLightmqSessionStore->getLock());
	auto iter = m_ackIndexFileMap.find(name);
	if (iter == m_ackIndexFileMap.end())
		return false;

	return iter->second->isRecordAcked(level, seq);
}

template <class DataFormat, class LockType, class FileChannel>
bool AckIndexVariableRecordStore<DataFormat, LockType, FileChannel>::isRecordAcked(const std::string& name, uint64_t seq) {
	std::lock_guard<LockType> lock(m_pLightmqSessionStore->getLock());
	auto iter = m_ackIndexFileMap.find(name);
	if (iter == m_ackIndexFileMap.end())
		return false;

	return iter->second->isRecordAcked(m_pLightmqSessionStore->getRecordLevel(seq), seq);
}

template <class DataFormat, class LockType, class FileChannel>
bool AckIndexVariableRecordStore<DataFormat, LockType, FileChannel>::getNonAckRecord(const std::string& name,std::map<uint64_t,bool>& result, uint64_t& largeBoxStart, uint64_t& largeBoxEnd) {
    std::lock_guard<LockType> lock(m_pLightmqSessionStore->getLock());
    auto iter = m_ackIndexFileMap.find(name);
    if (iter==m_ackIndexFileMap.end())
        return false;
    iter->second->getNonAckRecord(result, largeBoxStart, largeBoxEnd);
    return true;
}

template <class DataFormat, class LockType, class FileChannel>
bool AckIndexVariableRecordStore<DataFormat, LockType, FileChannel>::getExpiredAckRecord(const std::string& name,std::map<uint64_t,bool>& result, uint64_t& largeBoxStart, uint64_t& largeBoxEnd) {
    std::lock_guard<LockType> lock(m_pLightmqSessionStore->getLock());
    auto iter = m_ackIndexFileMap.find(name);
    if (iter==m_ackIndexFileMap.end())
        return false;
    iter->second->getExpiredAckRecord(result, largeBoxStart, largeBoxEnd);
    return true;
}

template <class DataFormat, class LockType, class FileChannel>
bool AckIndexVariableRecordStore<DataFormat, LockType, FileChannel>::getFirstNonAckRecord(const std::string& name, uint64_t& result) {
    std::lock_guard<LockType> lock(m_pLightmqSessionStore->getLock());
    auto iter = m_ackIndexFileMap.find(name);
        if (iter==m_ackIndexFileMap.end())
            return false;
    return iter->second->getFirstNonAckRecord(result);
}

template <class DataFormat, class LockType, class FileChannel>
bool AckIndexVariableRecordStore<DataFormat, LockType, FileChannel>::getMinNonAckRecord(uint64_t& result){
    std::lock_guard<LockType> lock(m_pLightmqSessionStore->getLock());
    std::once_flag onceFlag;
    uint64_t noAckSeq = m_baseSequence + getRecordCount();
    for (auto &e : m_ackIndexFileMap) {
        std::call_once(onceFlag, [&](){
            result = m_baseSequence + getRecordCount();
        });
        e.second->getFirstNonAckRecord(noAckSeq);
        result = std::min(result, noAckSeq);
    }
    return true;
}


template <class DataFormat, class LockType, class FileChannel>
bool AckIndexVariableRecordStore<DataFormat, LockType, FileChannel>::markRetransmitRecord(const std::string& name, uint64_t seq) {
    std::lock_guard<LockType> lock(m_pLightmqSessionStore->getLock());
    auto iter = m_ackIndexFileMap.find(name);
        if (iter==m_ackIndexFileMap.end())
            return false;
    iter->second->markRetransmit(seq);
    return true;

}

template <class DataFormat, class LockType, class FileChannel>
bool AckIndexVariableRecordStore<DataFormat, LockType, FileChannel>::checkIndexFileExist(const std::string& name, bool isSequential) {
    if (isSequential)
        return SequentialAckIndexFile<dep::FsyncFileStream>::isFileExist(m_storePath, m_storeName, name);
    else
        return IndividualAckIndexFile<dep::FsyncFileStream>::isFileExist(m_storePath, m_storeName, name);

}

template <class DataFormat, class LockType /*= dep::NoLock*/, class FileChannel /*= dep::BufferedFileStream*/>
std::vector<std::string> lightmq::util::AckIndexVariableRecordStore<DataFormat, LockType, FileChannel>::getAckedBy(uint64_t seq){
    std::lock_guard<LockType> lock(m_pLightmqSessionStore->getLock());
    std::vector<std::string> result;
    for (auto& kv : m_ackIndexFileMap) {
        if (kv.second->isRecordAcked(seq)) {
            result.push_back(kv.first);
        }
    }
    return result;
}

template <class DataFormat, class LockType, class FileChannel>
void AckIndexVariableRecordStore<DataFormat, LockType, FileChannel>::setSequenceReady(uint64_t seq){
    m_pLightmqSessionStore->setSequenceReady(seq);
}

template <class DataFormat, class LockType, class FileChannel>
void AckIndexVariableRecordStore<DataFormat, LockType, FileChannel>::setSequenceNotReady(uint64_t seq){
    m_pLightmqSessionStore->setSequenceNotReady(seq);
}

template <class DataFormat, class LockType, class FileChannel>
void AckIndexVariableRecordStore<DataFormat, LockType, FileChannel>::setAllSequenceReady(){
    m_pLightmqSessionStore->setAllSequenceReady();
}

//template <class DataFormat, class LockType, class FileChannel>
//uint32_t AckIndexVariableRecordStore<DataFormat, LockType, FileChannel>::getManyWithPrefixSuffixReady(uint64_t start, uint64_t end, dep::ByteBuffer& buffer){
//    return m_pLightmqSessionStore->getManyWithPrefixSuffixReady(start, end, buffer);
//}
template <class DataFormat, class LockType, class FileChannel>
template <class OutputDataFormat>
uint32_t AckIndexVariableRecordStore<DataFormat, LockType, FileChannel>::getManyWithPrefixSuffix(uint64_t start, dep::ByteBuffer& buffer){
    return m_pLightmqSessionStore->getManyWithPrefixSuffixProxy(start, buffer);
}
template <class DataFormat, class LockType, class FileChannel>
template <class OutputDataFormat>
uint32_t AckIndexVariableRecordStore<DataFormat, LockType, FileChannel>::getManyWithPrefixSuffix(uint64_t start, uint64_t end, dep::ByteBuffer& buffer){
    return m_pLightmqSessionStore->getManyWithPrefixSuffixProxy(start, end, buffer);
}

template <class DataFormat, class LockType, class FileChannel>
uint32_t AckIndexVariableRecordStore<DataFormat, LockType, FileChannel>::getMany(uint64_t start, uint64_t end, dep::ByteBuffer& buffer, std::pair<uint64_t, uint64_t> &startEndPosition, uint64_t& expiredCount){
    return m_pLightmqSessionStore->getMany(start, end, buffer, startEndPosition, expiredCount);

}
template <class DataFormat, class LockType, class FileChannel>
uint32_t AckIndexVariableRecordStore<DataFormat, LockType, FileChannel>::getManyReady(uint64_t start, uint64_t end, dep::ByteBuffer& buffer, std::pair<uint64_t, uint64_t> &startEndPosition, uint64_t& expiredCount){
    return m_pLightmqSessionStore->getManyReady(start, end, buffer, startEndPosition, expiredCount);

}
template <class DataFormat, class LockType, class FileChannel>
uint64_t AckIndexVariableRecordStore<DataFormat, LockType, FileChannel>::getRecordCount(){
    return m_pLightmqSessionStore->getRecordCount();
}
template <class DataFormat, class LockType, class FileChannel>
void AckIndexVariableRecordStore<DataFormat, LockType, FileChannel>::readSingleMessageBuffer(uint64_t seq, dep::ByteBuffer& buffer){
    m_pLightmqSessionStore->readSingleMessageBuffer(seq, buffer);
}

template <class DataFormat, class LockType, class FileChannel>
void AckIndexVariableRecordStore<DataFormat, LockType, FileChannel>::markDeleted(uint64_t start, uint64_t end){
    m_pLightmqSessionStore->markDeleted(start, end);
}
template <class DataFormat, class LockType, class FileChannel>
bool AckIndexVariableRecordStore<DataFormat, LockType, FileChannel>::isDeletedOrRemoteMessage(uint64_t seq) const{
    return m_pLightmqSessionStore->isDeletedOrRemoteMessage(seq);
}

template <class DataFormat, class LockType, class FileChannel>
uint8_t AckIndexVariableRecordStore<DataFormat, LockType, FileChannel>::getTransState(uint64_t seq) {
    return m_pLightmqSessionStore->getTransState(seq);
}

template <class DataFormat, class LockType, class FileChannel>
std::vector<uint64_t> AckIndexVariableRecordStore<DataFormat, LockType, FileChannel>::getDeletedSequences(uint64_t start, uint64_t end) const{
    return m_pLightmqSessionStore->getDeletedSequences(start, end);
}

template <class DataFormat, class LockType, class FileChannel>
bool AckIndexVariableRecordStore<DataFormat, LockType, FileChannel>::markExpired(uint64_t start, uint64_t end){
    return m_pLightmqSessionStore->markExpired(start, end);
}

template <class DataFormat, class LockType, class FileChannel>
std::vector<uint64_t> AckIndexVariableRecordStore<DataFormat, LockType, FileChannel>::getExpiredSequences(uint64_t start, uint64_t end) const{
    return m_pLightmqSessionStore->getExpiredSequences(start, end);
}

template <class DataFormat, class LockType, class FileChannel>
std::vector<uint64_t> AckIndexVariableRecordStore<DataFormat, LockType, FileChannel>::getRemoteSequences(uint64_t start, uint64_t end) const{
    return m_pLightmqSessionStore->getRemoteSequences(start, end);
}
template <class DataFormat, class LockType, class FileChannel>
std::vector<uint64_t> AckIndexVariableRecordStore<DataFormat, LockType, FileChannel>::getRemoteOrDeletedSequences(uint64_t start, uint64_t end) const{
    return m_pLightmqSessionStore->getRemoteOrDeletedSequences(start, end);
}
template <class DataFormat, class LockType, class FileChannel>
const VariantMap* AckIndexVariableRecordStore<DataFormat, LockType, FileChannel>::getMessageProperties(uint64_t sequence) const{
    return m_pLightmqSessionStore->getMessageProperties(sequence);
}

template <class DataFormat, class LockType, class FileChannel>
std::vector<uint64_t> AckIndexVariableRecordStore<DataFormat, LockType, FileChannel>::getNotDeletedSequences() const{
    return m_pLightmqSessionStore->getNotDeletedSequences();
}
template <class DataFormat, class LockType, class FileChannel>
uint64_t AckIndexVariableRecordStore<DataFormat, LockType, FileChannel>::getLastHasProperitesMessageSequence() const{
    return m_pLightmqSessionStore->getLastHasProperitesMessageSequence();
}
template <class DataFormat, class LockType, class FileChannel>
uint64_t AckIndexVariableRecordStore<DataFormat, LockType, FileChannel>::getOldestDeletedSequence() const{
    return m_pLightmqSessionStore->getOldestDeletedSequence();
}

template <class DataFormat, class LockType, class FileChannel>
bool AckIndexVariableRecordStore<DataFormat, LockType, FileChannel>::isDeleted(uint64_t seq) const{
    return m_pLightmqSessionStore->isDeleted(seq);
}

template <class DataFormat, class LockType, class FileChannel>
std::tuple<uint64_t, uint64_t> AckIndexVariableRecordStore<DataFormat, LockType, FileChannel>::getLastLocalMessageSequence() const{
    return m_pLightmqSessionStore->getLastLocalMessageSequence();
}

template<class DataFormat, class LockType, class FileChannel>
uint64_t AckIndexVariableRecordStore<DataFormat, LockType, FileChannel>::getBeginSequence() const
{
	return m_pLightmqSessionStore->getBeginSequence();
}

template<class DataFormat, class LockType, class FileChannel>
void AckIndexVariableRecordStore<DataFormat, LockType, FileChannel>::setNewBeginSequence(uint64_t newBeginSequence)
{
	m_pLightmqSessionStore->setNewBeginSequence(newBeginSequence);
}


template<class DataFormat, class LockType, class FileChannel>
GroupMessage* AckIndexVariableRecordStore<DataFormat, LockType, FileChannel>::getGroupMessagePtr(const MessageID &messageID) const
{
	return m_pLightmqSessionStore->getGroupMessagePtr(messageID);
}

template<class DataFormat, class LockType, class FileChannel>
bool AckIndexVariableRecordStore<DataFormat, LockType, FileChannel>::checkGroupMessage(const MessageID &begin, uint64_t seq, IVariableRecordStore::ChkGroupResult *first) const
{
    return m_pLightmqSessionStore->checkGroupMessage(begin, seq, first);
}

template<class DataFormat, class LockType, class FileChannel>
void AckIndexVariableRecordStore<DataFormat, LockType, FileChannel>::getAllCursor(const std::string &indexFileName, MultiSequence &cursor) const
{
    std::lock_guard<LockType> lock(m_pLightmqSessionStore->getLock());
    auto iter = m_ackIndexFileMap.find(indexFileName);
    if (iter==m_ackIndexFileMap.end())
        return ;
    
    iter->second->getAllCursor(cursor);
    return ;   
}

template<class DataFormat, class LockType, class FileChannel>
bool AckIndexVariableRecordStore<DataFormat, LockType, FileChannel>::checkDefaultLevel(uint64_t startSeq, uint64_t endSeq, IVariableRecordStore::ChkLevelResult *result) const 
{
    return m_pLightmqSessionStore->checkDefaultLevel(startSeq, endSeq, result);
}
template<class DataFormat, class LockType, class FileChannel>
uint8_t AckIndexVariableRecordStore<DataFormat, LockType, FileChannel>::getRecordLevel(uint64_t sequence) const
{
    return m_pLightmqSessionStore->getRecordLevel(sequence);
}
template<class DataFormat, class LockType, class FileChannel>
bool AckIndexVariableRecordStore<DataFormat, LockType, FileChannel>::getLevelBrief(uint8_t lev, uint64_t *count, uint64_t *firstSeq, uint64_t *lastSeq) const
{
    return m_pLightmqSessionStore->getLevelBrief(lev, count, firstSeq, lastSeq);
}
template<class DataFormat, class LockType, class FileChannel>
uint64_t AckIndexVariableRecordStore<DataFormat, LockType, FileChannel>::getFirstNoAckByLevel(uint8_t lev, uint64_t cursor) const 
{
    return m_pLightmqSessionStore->getFirstNoAckByLevel(lev, cursor);
}

}} // namespace lightmq { namespace util {

#endif /* ACKINDEXVARIABLERECORDSTORE_H_ */
