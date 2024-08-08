////////////////////////////////////////////////////////////////////////////////
//
// QMAPICircularBuffer.h
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

#ifndef QMAPICIRCULARBUFFER_H_
#define QMAPICIRCULARBUFFER_H_

#include "CircularBuffer.h"
#include <vector>
#include <atomic>
#include <mutex>
#include "platform.h"

namespace lightmq { namespace qmapi {

    template <typename T>
    class QMAPICircularBuffer : public dep::CircularBuffer<T> {
    public:
    	QMAPICircularBuffer() = delete;
        explicit QMAPICircularBuffer(uint32_t capacity, uint32_t dequeueRetryCnt = 1, uint32_t enqueueRetryCnt = 1)
        : dep::CircularBuffer<T>{ capacity }, m_dequeueRetryCnt(dequeueRetryCnt), m_enqueueRetryCnt(enqueueRetryCnt) {

        }

        inline uint32_t dequeue(std::vector<T>& vectorObjects, uint32_t maxSize) {
//			std::lock_guard<std::mutex> lock(m_mtx);

			uint32_t retry{ m_dequeueRetryCnt };
            uint32_t size{ 0 };

            while (m_lock.test_and_set(MEMORY_ORDER))  {// acquire lock
                // spin / retry
                if (--retry == 0) {
                    return 0;   // just return if cannot acquire the lock after retry
                }
            }

            while (this->empty() == false && maxSize > size) {
                //vectorObjects.push_back(this->front());
                vectorObjects[size] = std::move(this->front());
                this->popFront();
                size++;
            }

            m_lock.clear();
            if (size > 0) m_enqueueCV.notify_all();	// notify everyone who wants to put something in
            return size;
        }

        inline bool boundedEnqueue(const T& object) {
//			std::lock_guard<std::mutex> lock(m_mtx);

        	uint32_t retry{ m_enqueueRetryCnt };
            while (m_lock.test_and_set(MEMORY_ORDER)) { // acquire lock
                // spin / retry
                if (--retry == 0) {
                    return false;   // just return if cannot acquire the lock after retry
                }
            }
            if (this->full() == false) {
                this->pushBack(object);
                m_lock.clear();
                m_dequeueCV.notify_all();	// notify everyone who wants to take something out
                return true;
            }
            else {
                m_lock.clear();
                return false;
            }
        }

        inline bool boundedEnqueue(T&& object) {
//          std::lock_guard<std::mutex> lock(m_mtx);

            uint32_t retry{ m_enqueueRetryCnt };
            while (m_lock.test_and_set(MEMORY_ORDER)) { // acquire lock
                // spin / retry
                if (--retry == 0) {
                    return false;   // just return if cannot acquire the lock after retry
                }
            }
            if (this->full() == false) {
                this->pushBack(std::move(object));
                m_lock.clear();
                m_dequeueCV.notify_all();   // notify everyone who wants to take something out
                return true;
            }
            else {
                m_lock.clear();
                return false;
            }
        }
        std::condition_variable& getDequeueCV() {
        	return m_dequeueCV;
        }

        std::condition_variable& getEnqueueCV() {
        	return m_enqueueCV;
        }


    private:
        std::atomic_flag m_lock = ATOMIC_FLAG_INIT;
        std::condition_variable m_dequeueCV;
        std::condition_variable m_enqueueCV;

        uint32_t m_dequeueRetryCnt;
        uint32_t m_enqueueRetryCnt;

//        std::mutex m_mtx;
    };

}} // namespace lightmq { namespace qmapi {

#endif /* QMAPICIRCULARBUFFER_H_ */
