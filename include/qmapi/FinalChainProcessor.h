////////////////////////////////////////////////////////////////////////////////
//
// FinalChainProcessor.h
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
#ifndef FINALCHAINPROCESSOR_H_
#define FINALCHAINPROCESSOR_H_

#include "ByteBuffer.h"
#include "BaseChainProcessor.h"
#include <iostream>

namespace lightmq { namespace qmapi {

//class FinalMessageListener {
//public:
//    void onMessage(dep::ByteBuffer& message) {
//        m_buffer = &message;
//    }
//
//    //void deepCopy(dep::ByteBuffer &buf){};
//    //dep::ByteBuffer shallowCopy(){};
//
//    dep::ByteBuffer* m_buffer = nullptr;
//};


class FinalChainProcessor : public BaseChainProcessor {
public:

    bool internalProcess(ProcessorInfo& processorInfo, InternalLightmqMessage &imessage) {
        //m_listener.onMessage(message);
        return true;
    }

private:
    //FinalMessageListener& m_listener;
};

}} // namespace lightmq { namespace qmapi {

#endif /* FINALCHAINPROCESSOR_H_ */
