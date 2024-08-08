////////////////////////////////////////////////////////////////////////////////
//
// LightmqSelectableQueueOnWritable.h
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

#ifndef LIGHTMQSELECTABLEQUEUEONWRITEABLE_H_
#define LIGHTMQSELECTABLEQUEUEONWRITEABLE_H_

#include "LightmqSelectableQueue.h"
#include <iostream>

namespace lightmq { namespace util {

/**
 * A queue which can be driven by an IO event multiplexing service.
 */
template <typename T>
class LightmqSelectableQueueOnWritable : public LightmqSelectableQueue<T> {
public:

    typedef std::function<void(dep::SelectEvent&)> OnWriteListener; /**< Callback function for write event */

    /**
     * Constructor
     *
     * @param readBatchSize Max number of items to take and forward to listener for each SelectService event callback.
     */
    explicit LightmqSelectableQueueOnWritable(uint32_t readBatchSize) : LightmqSelectableQueue<T>(readBatchSize){
        std::cout << "LightmqSelectableQueueOnWritable construct, fd=" << LightmqSelectableQueue<T>::getFileDescriptor() << std::endl;
    }
    /**
     * Destructor
     */
    virtual ~LightmqSelectableQueueOnWritable() {
        std::cout << "LightmqSelectableQueueOnWritable destruct, fd=" << LightmqSelectableQueue<T>::getFileDescriptor() << std::endl;
    }

    void setOnWriteListener(OnWriteListener&& listener);

    void registerWrite(dep::ISelector& selector);

    void unregisterWrite(dep::ISelector& selector);


    void onEvent(dep::SelectEvent& event) override;


private:
    OnWriteListener m_onWritelistener;
};


template <typename T>
void LightmqSelectableQueueOnWritable<T>::setOnWriteListener(OnWriteListener&& listener){
    m_onWritelistener = std::move(listener);
}

template <typename T>
void lightmq::util::LightmqSelectableQueueOnWritable<T>::registerWrite(dep::ISelector& selector){
    dep::SelectEvent selectEvent;
    selectEvent.setRead(true);
    selectEvent.setWrite(true);
    selector.updateEvent(*this, selectEvent);
}

template <typename T>
void lightmq::util::LightmqSelectableQueueOnWritable<T>::unregisterWrite(dep::ISelector& selector){
    dep::SelectEvent selectEvent;
    selectEvent.setRead(true);
    selectEvent.setWrite(false);
    selector.updateEvent(*this, selectEvent);
}


template<typename T>
void LightmqSelectableQueueOnWritable<T>::onEvent(dep::SelectEvent& event) {
    if (event.isRead()) {
        LightmqSelectableQueue<T>::onEvent(event);
    }

    if (event.isWrite() && m_onWritelistener) {
        m_onWritelistener(event);
    }
}


}} // namespace lightmq { namespace util {

#endif /* LIGHTMQSELECTABLEQUEUEONWRITEABLE_H_ */
