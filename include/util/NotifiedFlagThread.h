////////////////////////////////////////////////////////////////////////////////
//
// NotifiedFlagThread.h
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
#ifndef UTIL_NOTIFIEDFLAGTHREAD_H_
#define UTIL_NOTIFIEDFLAGTHREAD_H_

#include <memory>
#include <mutex>
#include <condition_variable>
#include "Thread.h"

namespace lightmq { namespace util {

class NotifiedFlagThread : public dep::Thread
{
public:
	static const int NOTIFY_FLAG_STOP   = 1;
	static const int NOTIFY_FLAG_WAKEUP = 2;

	/**
	* Allocates a new dep::Thread object. This constructor is for the dep::Thread class that initializes the runnable object pointer.
	*
	* @param  pRunnable  The pointer of dep::Runnable object
	*/
	explicit NotifiedFlagThread(dep::Runnable* pRunnable = nullptr)
		: dep::Thread(pRunnable)
	{}

	/**
	 * The other overload of constructor taking a smart pointer to an runnable object.
	 *
	 * @param  pRunnable  The pointer of dep::Runnable object
	 */
	explicit NotifiedFlagThread(std::unique_ptr<dep::Runnable>&& pRunnable = nullptr)
		: dep::Thread(std::move(pRunnable))
	{}


	void start()
	{
		m_notifyFlag = 0;
        dep::Thread::start();
	}


	template<typename Rep, typename Period>
	uint64_t waitNotifyFor(const std::chrono::duration<Rep, Period>& timeout, uint64_t removeFlag = 0ull) 
    {
		std::unique_lock<std::mutex> lock(m_lock);
		m_notifyCV.wait_for(lock, timeout);

		uint64_t retFlag = m_notifyFlag;
		m_notifyFlag &= (~removeFlag);
		return retFlag;
	}

	void notifyFlag(uint64_t flag)
    {
		std::lock_guard<std::mutex> lock(m_lock);
		m_notifyFlag |= flag;
		m_notifyCV.notify_all();
	}

	void notifyStop()
    {
        notifyFlag(NOTIFY_FLAG_STOP);
    }


    uint64_t getThreadFlag() const
    {
		std::lock_guard<std::mutex> lock(m_lock);
		return m_notifyFlag;
    }

    bool isNotifiedStop()
    {
        return getThreadFlag() & NOTIFY_FLAG_STOP;
    }


private:
	uint64_t m_notifyFlag;
	mutable std::mutex m_lock;
	std::condition_variable m_notifyCV;
};

}} // namespace lightmq { namespace util {

#endif  //  !UTIL_NOTIFIEDFLAGTHREAD_H_