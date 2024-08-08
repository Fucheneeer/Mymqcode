#ifndef MULTILOCKGUARD_H_
#define MULTILOCKGUARD_H_
////////////////////////////////////////////////////////////////////////////////
//
// MultiLockGuard.h
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
#include <set>

namespace lightmq {	namespace util {

template <typename T, typename LOCK_OPER>
class MultiLockGuard
{
public:
	MultiLockGuard()
		: m_locks(nullptr)
		, m_locksRef(std::set<T>())
	{

	}

	MultiLockGuard(const std::set<T>& lockSet) 
		: m_locks(&lockSet)
		, m_locksRef(lockSet)
	{
		carry_lock();
	}

	void lockMulti(const std::set<T>& lockSet) {
		m_locks = &lockSet;
		carry_lock();
	}

	~MultiLockGuard() {
		carry_unlock();
	}

private:
	void carry_lock() {
		if (m_locks) {
			for (auto& lock : *m_locks) {
				LOCK_OPER::lock(lock);
			}
		}
	}

	void carry_unlock() {
		if (m_locks) {
			for (auto iter = m_locks->rbegin(); iter != m_locks->rend(); ++iter) {
				LOCK_OPER::unlock(*iter);
			}
		}
	}

	const std::set<T> *m_locks;
	const std::set<T> &m_locksRef;
};

}} // namespace lightmq { namespace util {

#endif // !MULTILOCKGUARD_H_
