////////////////////////////////////////////////////////////////////////////////
//
// IAckIndexFile.h
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

#ifndef IACKINDEXFILE_H_
#define IACKINDEXFILE_H_

#include <map>
#include "MultiLevelBlock.h"

namespace lightmq { namespace util {


class IAckIndexFile {
public:
    enum ACK_TYPE{
        NORMAL_ACK = 0,     // just acked
        EXPIRED_ACK = 1    // expired message(ttl version)
    };
    IAckIndexFile()=default;
    virtual ~IAckIndexFile()=default;

    virtual void init(bool append, uint32_t baseSequence, uint64_t recordCount, bool isDurable)=0;
    virtual void addRecord(uint64_t toSeq)=0;
    virtual void ackRecord(uint64_t seq)=0;
    virtual void ackRecord(uint8_t level, uint64_t seq)=0;
    virtual void markRetransmit(uint64_t seq)=0;
    virtual void getNonAckRecord(std::map<uint64_t, bool>& result, uint64_t& largeBlockStart, uint64_t& largeBlockEnd)=0;
    virtual void getAllCursor(MultiSequence &seq) = 0;
    virtual void getExpiredAckRecord(std::map<uint64_t, bool>& result, uint64_t& largeBlockStart, uint64_t& largeBlockEnd)=0;
    virtual void flush()=0;
    virtual void close()=0;
    virtual bool getFirstNonAckRecord(uint64_t& sequence)=0; // return false, if no last non-ack record

    virtual bool isRecordAcked(uint64_t seq) = 0;
    // 主备同步的时候，ack文件的同步速度可能store文件的同步速度，
    // 这时候，备机不能知道record的级别
    // indivalAck能够判断某个记录是否ack，sequential类型的ack文件必须依赖外部传入的级别，才能判断某个sequence是否已经ack。
    virtual bool isRecordAcked(uint8_t level, uint64_t seq) = 0;

};


}} // namespace lightmq { namespace util {

#endif /* IACKINDEXFILE_H_ */
