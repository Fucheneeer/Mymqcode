////////////////////////////////////////////////////////////////////////////////
//
// QMAPIProcessorMap.h
//
// Copyright (c) 2020 CFETSIT.
// Building 3, 491 YinDong Road, Shanghai, China.
// All rights reserved.
//
// LightMQ is the copyrighted,
// proprietary property of CFETSIT which retain all right, title and interest therein.
//
// Description: Header file of ProcessorChain
//
////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <memory>
#include <vector>
#include <map>
#include <memory>

#include "IChainProcessor.h"
#include "Uncopyable.h"


namespace dep {
    class ByteBuffer;
    class ISeverityLog;
}

namespace lightmq { namespace qmapi {


class BaseProcessorAttribute;
class QMAPIProcessorChainConfiguration;
class InternalLightmqMessage;


class QMAPIProcessorMap : private dep::Uncopyable{
public:


    explicit QMAPIProcessorMap(const std::vector<std::reference_wrapper<BaseProcessorAttribute>>& attributes,
                            QMAPIProcessorChainConfiguration& configuration);

    void add(uint32_t processorType, std::unique_ptr<IChainProcessor> &&pProcessor);
    bool process(uint32_t processorType, IChainProcessor::ProcessorInfo& processorInfo, InternalLightmqMessage &imessage);

private:
    std::map<uint32_t, std::unique_ptr<IChainProcessor> > m_processorMap;
};

}} // namespace lightmq { namespace qmapi {

