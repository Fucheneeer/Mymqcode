////////////////////////////////////////////////////////////////////////////////
//
// BaseCompressProcessor.h
//
// Copyright (c) 2020 CFETSIT.
// Building 3, 491 YinDong Road, Shanghai, China.
// All rights reserved.
//
// LightMQ is the copyrighted,
// proprietary property of CFETSIT which retain all right, title and interest therein.
//
// Description: Header of class BaseCompressProcessor
//
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <memory>

#include "ICompressionEngine.h"
#include "BaseChainProcessor.h"
#include "ModuleManager.h"
#include "ByteBuffer.h"

namespace dep {
    class ISeverityLog;
    class ByteBuffer;
}


namespace lightmq { namespace qmapi {

class ImplFactoryAttribute;
class ICompressionFactory;

class BaseCompressProcessor : public BaseChainProcessor 
{
    static constexpr uint32_t INITIAL_BUFFER_SIZE { 1024 * 10 };
public:
    BaseCompressProcessor(const ImplFactoryAttribute& attribute,
                     const ICompressionFactory& factory,
                     dep::ISeverityLog& logger);
    virtual ~BaseCompressProcessor() = default;

protected:
    void LoadEngine(const ImplFactoryAttribute& attribute,
                     const ICompressionFactory& factory);
    dep::ByteBuffer* getFreeBuffer();

protected:
    static thread_local dep::ByteBuffer m_internalBuffer;

    dep::ISeverityLog& m_logger;
    std::unique_ptr<ICompressionEngine> m_pCompressEngine;
};
}}

