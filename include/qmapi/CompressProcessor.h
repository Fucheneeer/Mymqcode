////////////////////////////////////////////////////////////////////////////////
//
// CompressProcessor.h
//
// Copyright (c) 2020 CFETSIT.
// Building 3, 491 YinDong Road, Shanghai, China.
// All rights reserved.
//
// LightMQ is the copyrighted,
// proprietary property of CFETSIT which retain all right, title and interest therein.
//
// Description: Header of class CompressProcessor
//
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "BaseCompressProcessor.h"

namespace lightmq { namespace qmapi {


class CompressProcessor : public BaseCompressProcessor {

public:

    CompressProcessor(const ImplFactoryAttribute& attribute,
                     const ICompressionFactory& factory,
                     dep::ISeverityLog& logger);

    virtual ~CompressProcessor() = default;

    bool internalProcess(ProcessorInfo& processorInfo, InternalLightmqMessage &imessage) override;
};

}} // namespace lightmq { namespace qmapi {

