////////////////////////////////////////////////////////////////////////////////
//
// LightmqSelectableQueue.h
//
// Copyright (c) 2020 CFETSIT.
// Building 3, 491 YinDong Road, Shanghai, China.
// All rights reserved.
//
// LightMQ is the copyrighted,
// proprietary property of CFETSIT which retain all right, title and interest therein.
//
// Description: Header file of SelectableQueue
//
////////////////////////////////////////////////////////////////////////////////

#ifndef LIGHTMQSELECTABLEQUEUE_H_
#define LIGHTMQSELECTABLEQUEUE_H_

#include <cstddef>
#include <cstring>
#include <functional>
#include <memory>
#include <mutex>
#include <queue>

#include "ByteBuffer.h"
#include "DEPException.h"
#include "ISelectable.h"
#include "ISelector.h"
#include "Pipe.h"
#include "SpinLock.h"

namespace lightmq { namespace util {

/**
 * A queue which can be driven by an IO event multiplexing service.
 */
template <typename T>
class LightmqSelectableQueue : public dep::ISelectable {
public:
    typedef std::function<void(T)> QueueListener; /**< Callback function on new item in queue */
    typedef dep::SpinLock LockImpl; /**< Lock implementation class */

    /**
     * Constructor
     *
     * @param readBatchSize Max number of items to take and forward to listener for each SelectService event callback.
     */
    explicit LightmqSelectableQueue(uint32_t readBatchSize);

    /**
     * Destructor
     */
    virtual ~LightmqSelectableQueue() = default;

    /**
     * Initialized this object
     *
     * @param selector
     */
    void init(dep::ISelector& selector);

    /**
     * Close this queue
     *
     * @param selector
     */
    void close(dep::ISelector& selector);

    /**
     * Publish a new item to this queue.
     *
     * @param item
     * @return true if success
     */
    bool push(const T& item);

    /**
     * Publish a new item to this queue.
     *
     * @param item
     * @return true if success
     */
    bool push(T&& item);

    /**
     * Put a new item to this queue without notifying queue listener.
     *
     * @param item
     */
    void pushSilently(T&& item);

    /**
     * Put a new item to this queue without notifying queue listener.
     *
     * @param item
     */
    void pushSilently(T& item);

    /**
     * Notify queue listener all items previously pushed silently.
     */
    void notifyEvent();

    /**
     * Set callback function for this queue
     * @param listener
     */
    void setListener(QueueListener listener);

    const int32_t getFileDescriptor() const override;

    const int32_t getWriteFileDescriptor() const;

    void onEvent(dep::SelectEvent& event) override;

private:
    uint32_t getPipeCapacity() const;

private:
    uint32_t m_readBatchSize;
    dep::Pipe m_pipe {false}; // nonBlocking
    std::unique_ptr<dep::ByteBuffer> m_pWriteBuffer;
    dep::ByteBuffer m_readBuffer;
    std::queue<T> m_queue;
    QueueListener m_listener;
    LockImpl m_queueLock;
    LockImpl m_writeBufferLock;
    LockImpl m_silentItemCountLock;
    dep::SelectEvent m_selectEvent;
    std::vector<T> m_popStore; /**< Temporary container to store item received from queue before call back to queue listener */
    uint32_t m_silentItemCount {0};
};

template<typename T>
LightmqSelectableQueue<T>::LightmqSelectableQueue(uint32_t readBatchSize)
    : m_readBatchSize(readBatchSize)
    , m_readBuffer(1) {
    const uint32_t pipeBufferSize {getPipeCapacity()};
    m_pWriteBuffer.reset(new dep::ByteBuffer {pipeBufferSize});
    std::memset(m_pWriteBuffer->getArray(), 0, pipeBufferSize);
    m_selectEvent.setRead(true);
}

template<typename T>
void LightmqSelectableQueue<T>::init(dep::ISelector& selector) {
    selector.registerEvent(*this, m_selectEvent);
}

template<typename T>
bool LightmqSelectableQueue<T>::push(const T& item) {
    {
        std::lock_guard<LockImpl> queueLock {m_queueLock};
        m_queue.push(item);
    }
    std::lock_guard<LockImpl> writeBufferLock {m_writeBufferLock};
    // Do not care about the content being written
    // Just write 1 dep::byte to pipe
    m_pWriteBuffer->setLimit(1).setPosition(0);
    return m_pipe.write(*m_pWriteBuffer) > 0;

}

template<typename T>
bool LightmqSelectableQueue<T>::push(T&& item) {
    {
        std::lock_guard<LockImpl> queueLock {m_queueLock};
        m_queue.push(std::move(item));
    }
    std::lock_guard<LockImpl> writeBufferLock {m_writeBufferLock};
    // Do not care about the content being written
    // Just write 1 dep::byte to pipe
    m_pWriteBuffer->setLimit(1).setPosition(0);
    return m_pipe.write(*m_pWriteBuffer) > 0;
}

template<typename T>
inline void LightmqSelectableQueue<T>::pushSilently(T& item) {
    {
        std::lock_guard<LockImpl> queueLock {m_queueLock};
        m_queue.push(item);
    }
    std::lock_guard<LockImpl> silentItemCountLock {m_silentItemCountLock};
    ++m_silentItemCount;
}

template<typename T>
inline void LightmqSelectableQueue<T>::pushSilently(T&& item) {
    {
        std::lock_guard<LockImpl> queueLock {m_queueLock};
        m_queue.push(std::move(item));
    }
    std::lock_guard<LockImpl> silentItemCountLock {m_silentItemCountLock};
    ++m_silentItemCount;
}

template<typename T>
void LightmqSelectableQueue<T>::notifyEvent() {
    std::lock_guard<LockImpl> silentItemCountLock {m_silentItemCountLock};
    uint32_t byteToWrite {m_silentItemCount};
    const uint32_t bufCapacity {m_pWriteBuffer->getCapacity()};
    while (byteToWrite > 0) {
        const uint32_t attempt {std::min(byteToWrite, bufCapacity)};
        m_pWriteBuffer->setLimit(attempt).setPosition(0);
        if (m_pipe.write(*m_pWriteBuffer) != attempt) {
            throw dep::DEPException {"SelectableQueue<T>::notifyEvent() Failed to write to pipe"};
        }
        byteToWrite -= attempt;
    }
    m_silentItemCount = 0;
}

template<typename T>
inline void LightmqSelectableQueue<T>::setListener(QueueListener listener) {
    m_listener = listener;
}

template<typename T>
inline void LightmqSelectableQueue<T>::close(dep::ISelector& selector) {
    selector.unregisterEvent(*this);
}

template<typename T>
inline const int32_t LightmqSelectableQueue<T>::getFileDescriptor() const {
    return m_pipe.getReadFileDescriptor();
}

template<typename T>
inline const int32_t LightmqSelectableQueue<T>::getWriteFileDescriptor() const {
    return m_pipe.getWriteFileDescriptor();
}

template<typename T>
void LightmqSelectableQueue<T>::onEvent(dep::SelectEvent& event) {
    if (!m_listener) {
        return;
    }
    m_readBuffer.clear();
    int32_t count = m_pipe.read(m_readBuffer);

    if (count != -1) {
        m_readBuffer.flip();

        {
            // Acquire lock with minimal duration
            std::lock_guard<LockImpl> queueLock {m_queueLock};
            for (uint32_t i = 0; i < m_readBatchSize; ++i) {
                if (!m_queue.empty()) {
                    m_popStore.push_back(std::move(m_queue.front()));
                    m_queue.pop();
                }
                else {
                    break;
                }
            }
        }
        // Do not need lock when performing callback
        for (T& item : m_popStore) {
            m_listener(std::move(item));
        }
        m_popStore.clear();
    }

}

template<typename T>
inline uint32_t LightmqSelectableQueue<T>::getPipeCapacity() const {
    return m_pipe.getCapacity();
}

}} // namespace lightmq { namespace util {

#endif /* LIGHTMQSELECTABLEQUEUE_H_ */
