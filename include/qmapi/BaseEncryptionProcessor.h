////////////////////////////////////////////////////////////////////////////////
//
// BaseEncryptionProcessor.h
//
// Copyright (c) 2020 CFETSIT.
// Building 3, 491 YinDong Road, Shanghai, China.
// All rights reserved.
//
// LightMQ is the copyrighted,
// proprietary property of CFETSIT which retain all right, title and interest therein.
//
// Description: Header of class BaseEncryptionProcessor
//
////////////////////////////////////////////////////////////////////////////////

#ifndef BASEENCRYPTIONPROCESSOR_H_
#define BASEENCRYPTIONPROCESSOR_H_

#include <memory>

#include "ISymmetricEncryptionEngine.h"
#include "BaseChainProcessor.h"
#include "ModuleManager.h"
#include "ByteBuffer.h"

namespace lightmq { namespace qmapi {

class ImplFactoryAttribute;
class ICryptoFactory;
// class dep::ISeverityLog;

/**
 * Base Encryption Processor is responsible to process the input message according to the configured encryption criteria.
 */
class BaseEncryptionProcessor : public BaseChainProcessor {
private:
    static constexpr uint32_t INITIAL_BUFFER_SIZE { 1024 * 10 };

public:
    /**
     * Constructor
     *
     * @param attribute     Encryption Attribute
     * @param factory       IEncryptionFactory
     * @param logger        SeverityLogger
     */
    BaseEncryptionProcessor(const ImplFactoryAttribute& attribute,
                            const ICryptoFactory& factory,
                            dep::ISeverityLog& logger);

    /**
     * Destructor
     */
    virtual ~BaseEncryptionProcessor() = default;

    /**
     * Internal process - encrypt or decrypt the message
     *
     * @param processorInfo     Info of processor
     * @param message           Message buffer
     * @return true if the input is changed or unchanged, thus forward to next processor
     *         false if the input is dropped, thus do NOT forward
     */
    virtual bool internalProcess(ProcessorInfo& processorInfo, InternalLightmqMessage &imessage) = 0;

protected:
    /**
     * Load encryption engine
     *
     * @param attribute     Encryption Attribute
     * @param factory       Crypto Factory Class
     */
    void loadEncryptionEngine(const ImplFactoryAttribute& attribute, const ICryptoFactory& factory);

protected:
    dep::ByteBuffer* getFreeBuffer();

protected:
    static thread_local dep::ByteBuffer m_internalBuffer;

    dep::ISeverityLog& m_logger;
    std::unique_ptr<ISymmetricEncryptionEngine> m_pEncryptionEngine{nullptr};
};

}} // namespace lightmq { namespace qmapi {

#endif /* BASEENCRYPTIONPROCESSOR_H_ */
