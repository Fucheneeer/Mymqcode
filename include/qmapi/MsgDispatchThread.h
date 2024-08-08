////////////////////////////////////////////////////////////////////////////////
//
// MsgDispatchThread.h
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

#ifndef MSGDISPATCHTHREAD_H_
#define MSGDISPATCHTHREAD_H_

#include "Thread.h"
#include "BrokerMessage.h"
#include "QMAPICircularBuffer.h"

namespace lightmq { namespace qmapi {

class MsgDispatchThread {
public:
	using RXMSGQUEUE = QMAPICircularBuffer<BrokerMessage>;

	virtual ~MsgDispatchThread();

	static std::unique_ptr<MsgDispatchThread> getInstance(uint32_t id = 0, RXMSGQUEUE* pQueue = nullptr);
	void start();
	void stop();

	RXMSGQUEUE* getMsgQueue() { return m_pQueue; };

private:
	MsgDispatchThread(uint32_t id, RXMSGQUEUE* pQueue = nullptr);
	void run();

private:
	std::unique_ptr<RXMSGQUEUE> m_pOwnedQueue = nullptr;
	RXMSGQUEUE* m_pQueue = nullptr;
	std::unique_ptr<dep::Thread> m_pThread = nullptr;
	bool m_stop = false;
	uint32_t m_id;
};


}} // namespace lightmq { namespace qmapi {


#endif /* MSGDISPATCHTHREAD_H_ */
