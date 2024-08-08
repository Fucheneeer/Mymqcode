////////////////////////////////////////////////////////////////////////////////
//
// ExtractProcessor.h
//
// Copyright (c) 2020 CFETSIT.
// Building 3, 491 YinDong Road, Shanghai, China.
// All rights reserved.
//
// LightMQ is the copyrighted,
// proprietary property of CFETSIT which retain all right, title and interest therein.
//
// Description: Header of class ExtractProcessor
//
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "BaseCompressProcessor.h"



namespace lightmq { namespace qmapi {

class ExtractProcessor : public BaseCompressProcessor 
{

public:

    ExtractProcessor(const ImplFactoryAttribute& attribute,
                     const ICompressionFactory& factory,
                     dep::ISeverityLog& logger);

    virtual ~ExtractProcessor() = default;

    bool internalProcess(ProcessorInfo& processorInfo, InternalLightmqMessage &imessage) override;
};

}} // namespace lightmq { namespace qmapi {


