////////////////////////////////////////////////////////////////////////////////
//
// RecursiveLock.h
//
// Copyright (c) 2020 CFETSIT.
// Building 3, 491 YinDong Road, Shanghai, China.
// All rights reserved.
//
// LightMQ is the copyrighted,
// proprietary property of CFETSIT which retain all right, title and interest therein.
//
// Description: Implementation of NoLock using CAS operations.
////////////////////////////////////////////////////////////////////////////////

#ifndef RECURESIVE_IMPL_H_
#define RECURESIVE_IMPL_H_

#include <mutex>
#include "ILock.h"

namespace lightmq { namespace util {

class RecursiveLockImpl : public ILock {
public:
    RecursiveLockImpl() = default;

    virtual ~RecursiveLockImpl() = default;

    /**
        *  Tries to acquire the lock
        *  If lock already owned by thread calling try_lock, it will increment lock count and return true.
        *
        * @return true on successful lock acquisition, false otherwise.
        */
    virtual bool try_lock() override{
        return m_lock.try_lock();
    }

    /**
        * Acquire the lock
        */
    virtual void lock() override {
        m_lock.lock();
    }

    /**
        * Releases the lock
        */
    virtual void unlock() override {
        m_lock.unlock();
    }

private:
    std::recursive_mutex m_lock;
};

}} // namespace lightmq { namespace util {

#endif
