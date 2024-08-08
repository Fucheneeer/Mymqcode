////////////////////////////////////////////////////////////////////////////////
//
// ILock.h
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

#ifndef ILOCK_H_
#define ILOCK_H_

#include <vector>

namespace lightmq { namespace util {

class ILock {
public:
    ILock() = default;

    virtual ~ILock() = default;

    /**
        *  Tries to acquire the lock
        *  If lock already owned by thread calling try_lock, it will increment lock count and return true.
        *
        * @return true on successful lock acquisition, false otherwise.
        */
    virtual bool try_lock() = 0;

    /**
        * Acquire the lock
        */
    virtual void lock() = 0;

    /**
        * Releases the lock
        */
    virtual void unlock() = 0;
};

// class MQLock : public std::recursive_mutex{
// public:
//     MQLock()
//     {
// 
//     }
// 
// 
//     MQLock(uint32_t serial) {
//         m_serial = serial;
//     }
//     virtual ~MQLock() = default;
// 
//     bool try_lock()
//     {
//         return std::recursive_mutex::try_lock();
// //        return m_mtx.try_lock();
//     }
// 
//     void lock()
//     {
//         if (!m_vec_lock.empty() && m_vec_lock.back() > m_serial)
//         {
//             throw std::exception();
//         }
// //         m_mtx.lock();
//         std::recursive_mutex::lock();
//     }
// 
//     void unlock()
//     {
//         m_vec_lock.pop_back();
// //        m_mtx.unlock();
//         std::recursive_mutex::unlock();
//     }
// 
//     static thread_local std::vector<uint32_t> m_vec_lock;
//     uint32_t m_serial;
// //    std::recursive_mutex m_mtx;
// };

extern thread_local std::vector<uint32_t> s_vec_lock;

#define MQLOCK(serial) \
do  \
{ \
    if (!s_vec_lock.empty() && s_vec_lock.back() > serial) \
    { \
        /*throw std::exception();*/ \
    } \
    s_vec_lock.push_back(serial); \
} while (0)

#define MQUNLOCK() s_vec_lock.pop_back()

class MQLockGuard
{
public:
    MQLockGuard(uint32_t s) {
        MQLOCK(s);
    }
    ~MQLockGuard() 
    {
        MQUNLOCK();
    }

};

}} // namespace lightmq { namespace util {


#endif
