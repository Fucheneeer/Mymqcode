////////////////////////////////////////////////////////////////////////////////
//
// MessageUtil.h
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
#pragma once

#include <functional>
#include "ByteBuffer.h"
#include "BrokerTypes.h"

namespace dep {
    class ISeverityLog;
}

namespace lightmq {
struct TransactionID;
namespace protocol{

class IMessage;

} }

namespace lightmq { namespace util {


class IMessageParser
{
public: 
    virtual const char* desc(void *param1, void* param2) = 0; 
    virtual bool parse(uint32_t mt, dep::ByteBuffer &buffer, void *param1, void* param2) = 0;
};



class LightmqMessageHead;
typedef std::function<bool(uint32_t messageType, dep::ByteBuffer &buffer)> ParserFunc;

struct ExtendBmpRemoteBrokerInfo
{
	bool m_hasRemoteBrokerInfo = {false};
	std::string m_remoteBrokerID;
	MessageID m_remoteBrokerMessageID;
};

struct ExtendBmpTransactionInfo
{
    TransactionID m_transID;
	uint32_t  m_transMessageSeq { 0 };
};

struct MessageBufferPosition {
    //  Position info
	uint32_t  m_bufPosHead { 0 };
	uint32_t  m_bufPosValue { 0 };
	uint32_t  m_bufPosProviderMessageID { 0 };
    uint32_t m_bufPosLmqHead{0};

    void relinkMessagePos(int posChange)
    {
        m_bufPosHead += posChange;
        m_bufPosValue += posChange;
        m_bufPosProviderMessageID += posChange;
        m_bufPosLmqHead += posChange;
    }
};



class MessageUtil {
public:
	static const int EXTEND_BMP_REMOTE_BROKER_INFO = 0x01;
    static const int EXTEND_BMP_TRANSACTION_INFO   = 0x02;

public:
    typedef std::function<uint32_t(dep::ByteBuffer&, uint64_t currentSequence, bool& hasNext)> WriteStoreDataFunc;

    static bool parseLmqMessage(dep::ISeverityLog* log, dep::ByteBuffer &buffer, uint32_t maxBuffLength, IMessageParser *parser, void *param1 = nullptr, void *param2 = nullptr);
    // static bool parseLmqMessage(dep::ISeverityLog* log, dep::ByteBuffer &buffer, ParserFunc &parser, const char* desc);
    // 这两个函数启用，造成了很多困扰
    // 之后所有的消息都在writeTo的时候，自己写入消息长度和类型
    // static void writeLmqMessage(protocol::IMessage* msg, dep::ByteBuffer &buffer);
    // static void writeMessage(protocol::IMessage* msg, dep::ByteBuffer &buffer);


    static uint32_t writeMessageHead(protocol::IMessage* msg, dep::ByteBuffer &buffer);
    static void writeMessageHead(protocol::IMessage* msg, uint32_t messageLength, dep::ByteBuffer &buffer);
    static void updateMessageLength(dep::ByteBuffer &buffer, uint32_t startPos);

    static bool parseStoreData(dep::ByteBuffer &buffer, ExtendBmpRemoteBrokerInfo *rbInfo, std::string *providerID, MessageID *providerMessageID, uint32_t *messageLength, bool check, MessageBufferPosition* pHeaderPostions = nullptr, ExtendBmpTransactionInfo *transInfo = nullptr);
    static void writeStoreData(dep::ByteBuffer &buffer, const std::string &clientID, const SessionID &sessionID, const uint64_t &sequence, ExtendBmpRemoteBrokerInfo *rbInfo , ExtendBmpTransactionInfo *transInfo = nullptr);
    static uint64_t writeStoreData(dep::ByteBuffer &buffer, const std::string &clientID, const SessionID &sessionID, const uint64_t &sequence, ExtendBmpRemoteBrokerInfo *rbInfo, WriteStoreDataFunc &func, bool &hasNext, ExtendBmpTransactionInfo *transInfo = nullptr);

    enum {
        ToBufferEnd = 0,
        ToBufferStart, 
        ToClientDataBegin,
        ToBufferHead,
    };

    enum
    {
        SkipHead = 0, 
        ReadHeadAll,
        ReadHeadProperties,
    };
    static bool parseStoreAllData(dep::ByteBuffer &buffer, ExtendBmpRemoteBrokerInfo *rbInfo, std::string *providerID, MessageID *providerMessageID, uint32_t *messageLength, LightmqMessageHead *head, int readHeadType, std::string *body, int posEndType);
    static std::string readBodyFromBuffer(dep::ByteBuffer &buffer, int endPos);

    static void testWriteData(dep::ByteBuffer &buffer, const std::string &clientID, const SessionID &sessionID, const uint64_t &sequence, ExtendBmpRemoteBrokerInfo *rbInfo, LightmqMessageHead* head, const std::string &body);

    static bool writeVariableType(dep::ByteBuffer& buffer, const std::string& value);
};

}} // namespace lightmq { namespace protocol {

