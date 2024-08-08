////////////////////////////////////////////////////////////////////////////////
//
// ExpireService.h
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
#ifndef EXPIRE_SERVICE_H_
#define EXPIRE_SERVICE_H_
#include <set>
#include "NotifiedFlagThread.h"



namespace lightmq { namespace util {

class ExpirableObject {
public:
	typedef std::function<void()> ExpireListener;

protected:
	ExpirableObject(ExpireListener &&lis);

public:
	typedef std::chrono::system_clock::time_point TimePoint;

	virtual ~ExpirableObject() = default;

	void expireAt(TimePoint pt);
	void expireAfter(std::chrono::milliseconds ms);
	void delayExpire(std::chrono::milliseconds ms);

	TimePoint getExpireTime() const
	{
		return m_expireTime;
	}

	inline bool isExpired() const {
		auto now = std::chrono::system_clock::now();
		return now >= m_expireTime;
	}

protected:
	friend class ExpireService;
	TimePoint m_expireTime;
	ExpireListener m_expireListener;
	//std::chrono::milliseconds m_expireTime;
};

/*
 *  There is another timer service in dep util: BaseSelectService, who implement dep::ITimer.
 *  It has some problem, so here implement a new one.
 *  1. It works together with a IO select service, we need not it.
 *  2. It has timer count limit: 256
 *  3. It's listener is function with timerID param. we want to operate(reg, unreg, listen) on a object, not timerID.
 */
class ExpireService: public dep::Runnable {
public:
	ExpireService();
	//void expireCallWhen(TimePoint tp, ExpireListener &&lis);
	//void expireCallAfter(std::chrono::milliseconds ms, ExpireListener &&lis);

	void startExpireService();
	void stopExpireService();

	void registerExpireObject(const std::shared_ptr<ExpirableObject> &obj);
	void unregisterExpireObject(const std::shared_ptr<ExpirableObject> &obj);

protected:
	int scheduleExpireObjects(std::chrono::milliseconds &suggestWaitMs, int maxCount = -1);
	void doScheduleWork();

private:
	struct TimeComparator {
		bool operator() (const std::shared_ptr<ExpirableObject> &lv, const std::shared_ptr<ExpirableObject> &rv) {
			if (lv->getExpireTime() == rv->getExpireTime()) {
				return lv < rv;
			} else {
				return lv->getExpireTime() < rv->getExpireTime();
			}
		}
	};

	std::mutex m_mtxExpireQueue;
	std::set<std::shared_ptr<ExpirableObject>, TimeComparator> m_expireObjects;

	util::NotifiedFlagThread m_scheduleThread;
};

//class TransactionProcesso


}} // namespace lightmq { namespace broker {

#endif  //  !EXPIRE_SERVICE_H_