////////////////////////////////////////////////////////////////////////////////
//
// BaseChainProcessor.h
//
// Copyright (c) 2020 CFETSIT.
// Building 3, 491 YinDong Road, Shanghai, China.
// All rights reserved.
//
// LightMQ is the copyrighted,
// proprietary property of CFETSIT which retain all right, title and interest therein.
//
// Description: Header of BaseChainProcessor
//
////////////////////////////////////////////////////////////////////////////////

#ifndef BASECHAINPROCESSOR_H_
#define BASECHAINPROCESSOR_H_

#include "IChainProcessor.h"

namespace lightmq { namespace qmapi {

class InternalLightmqMessage;
/**
 * Base class for chain processor
 */
class BaseChainProcessor : public IChainProcessor {
public:
    /**
     * Constructor
     */
    BaseChainProcessor() = default;

    /**
     * Destructor
     */
    virtual ~BaseChainProcessor() = default;

public:
    IChainProcessor* setNext(IChainProcessor* pNextProcessor) override;

    virtual void process(ProcessorInfo& processorInfo, InternalLightmqMessage &imessage) override;

    /**
     * Internal process the message
     * Derived processor has to implement this method for its own internal processing logic
     *
     * @param processorInfo     Info of processor
     * @param message           Message buffer
     * @return true if forward to next processor, otherwise false
     */
    virtual bool internalProcess(ProcessorInfo& processorInfo, InternalLightmqMessage &imessage) = 0;

protected:
    IChainProcessor* m_pNextProcessor{nullptr};
};

}} // namespace lightmq { namespace qmapi {

#endif /* BASECHAINPROCESSOR_H_ */
