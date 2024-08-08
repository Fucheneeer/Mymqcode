////////////////////////////////////////////////////////////////////////////////
//
// EncryptProcessor.h
//
// Copyright (c) 2020 CFETSIT.
// Building 3, 491 YinDong Road, Shanghai, China.
// All rights reserved.
//
// LightMQ is the copyrighted,
// proprietary property of CFETSIT which retain all right, title and interest therein.
//
// Description: Header of class EncryptProcessor
//
////////////////////////////////////////////////////////////////////////////////

#ifndef ENCRYPTPROCESSOR_H_
#define ENCRYPTPROCESSOR_H_

#include "BaseEncryptionProcessor.h"

namespace lightmq { namespace qmapi {

class EncryptionAttribute;

/**
 * Encrypt Processor is responsible to encrypt the input message according to the configured encryption criteria.
 */
class EncryptProcessor : public BaseEncryptionProcessor {

public:
    /**
     * Constructor
     *
     * @param attribute     Encryption Attribute
     * @param factory       ICryptoFactory
     * @param logger        SeverityLogger
     */
    EncryptProcessor(const EncryptionAttribute& attribute,
                     const ICryptoFactory& factory,
                     dep::ISeverityLog& logger);

    /**
     * Destructor
     */
    virtual ~EncryptProcessor() = default;

    /**
     * Internal process - encrypt the message
     *
     * @param processorInfo     Info of processor
     * @param message           Message buffer
     * @return true if the input is changed or unchanged, thus forward to next processor
     *         false if the input is dropped, thus do NOT forward
     */
    bool internalProcess(ProcessorInfo& processorInfo, InternalLightmqMessage &imessage) override;
};

}} // namespace lightmq { namespace qmapi {

#endif /* ENCRYPTPROCESSOR_H_ */
