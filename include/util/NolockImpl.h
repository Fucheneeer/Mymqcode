////////////////////////////////////////////////////////////////////////////////
//
// NolockImpl.h
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

#ifndef NOLOCK_IMPL_H_
#define NOLOCK_IMPL_H_
#include "ILock.h"

namespace lightmq { namespace util {

    class NolockImpl : public ILock{
    public:
        NolockImpl() = default;

        virtual ~NolockImpl() = default;

        /**
            *  Tries to acquire the lock
            *  If lock already owned by thread calling try_lock, it will increment lock count and return true.
            *
            * @return true on successful lock acquisition, false otherwise.
            */
        virtual bool try_lock() override { return true; }

        /**
            * Acquire the lock
            */
        virtual void lock() override {}

        /**
            * Releases the lock
            */
        virtual void unlock() override {}
    };

}} // namespace lightmq { namespace util {

#endif
