////////////////////////////////////////////////////////////////////////////////
//
// UnConsumedQ.h
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
#include <mutex>
#include <condition_variable>
#include <queue>
#include "BrokerMessage.h"
#include "QMAPIConstant.h"

namespace lightmq { namespace qmapi {

class UnconsumedQueue
{
public:
    struct SingleMessage
    {
        std::shared_ptr<BrokerMessage> m_pBrokerMessage = {nullptr};
        uint32_t m_idx = {0};
    };

    UnconsumedQueue() = default;
    ~UnconsumedQueue();

    void enqueue(BrokerMessage &&obj);

    // timeout: -1 wait until message comes.
    //          0: not wait
    //          other: wait timeout.
    QMAPIConstant::QMAPI_RETCODE dequeue(int timeout, SingleMessage &msg, bool &hasMessage);

    void clear();

    bool condition() const;
    bool empty() const;
    void stop();

    void onError(QMAPIConstant::QMAPI_RETCODE err);

    bool isStoped() const;

private:
    mutable std::mutex m_mtx;
    std::condition_variable m_enqueCV;

    std::queue<SingleMessage> m_messageQ; 
    bool m_stopped{false};

    QMAPIConstant::QMAPI_RETCODE m_errCode;
};

}}

