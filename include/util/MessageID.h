////////////////////////////////////////////////////////////////////////////////
//
// MessageID.h
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

#ifndef LIGHTMQ_UTIL_MESSAGEID_H_
#define LIGHTMQ_UTIL_MESSAGEID_H_

#include <string>

namespace lightmq {
// unique identify a message in a queue

typedef uint64_t SessionID;

struct MessageID {
    MessageID();
    MessageID(SessionID sessID, uint64_t seq);
    MessageID(const MessageID& other);

    void set(SessionID sessID, uint64_t seq);
    // operator bool() const;

    std::string toString() const;

    std::string toString(int count) const;

    bool operator!=(const MessageID& other) const;

    MessageID& operator=(const MessageID& other);

    bool operator==(const MessageID& other) const;

    bool operator< (const MessageID& other) const;

    bool operator> (const MessageID& other) const;

    bool operator <= (const MessageID& other) const;

    bool operator >= (const MessageID& other) const;

    static MessageID parse(const std::string& strMsgID, const std::string& delimiter=",");

    SessionID sessionID;
    uint64_t sequence;
};
} //namespace lightmq {
#endif
