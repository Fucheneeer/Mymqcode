////////////////////////////////////////////////////////////////////////////////
//
// DecryptProcessor.h
//
// Copyright (c) 2020 CFETSIT.
// Building 3, 491 YinDong Road, Shanghai, China.
// All rights reserved.
//
// LightMQ is the copyrighted,
// proprietary property of CFETSIT which retain all right, title and interest therein.
//
// Description: Header of class DecryptProcessor
//
////////////////////////////////////////////////////////////////////////////////

#ifndef DECRYPTPROCESSOR_H_
#define DECRYPTPROCESSOR_H_

#include "BaseEncryptionProcessor.h"

namespace lightmq { namespace qmapi {

class DecryptionAttribute;

/**
 * Decrypt Processor is responsible to decrypt the input message according to the configured decryption criteria.
 */
class DecryptProcessor : public BaseEncryptionProcessor {

public:
    /**
     * Constructor
     *
     * @param attribute     Decryption Attribute
     * @param factory       ICryptoFactory
     * @param logger        SeverityLogger
     */
    DecryptProcessor(const DecryptionAttribute& attribute,
                     const ICryptoFactory& factory,
                     dep::ISeverityLog& logger);

    /**
     * Destructor
     */
    virtual ~DecryptProcessor() = default;

    /**
     * Internal process - decrypt the message
     *
     * @param processorInfo     Info of processor
     * @param message           Message buffer
     * @return true if the input is changed or unchanged, thus forward to next processor
     *         false if the input is dropped, thus do NOT forward
     */
    bool internalProcess(ProcessorInfo& processorInfo, InternalLightmqMessage &imessage) override;
};

}} // namespace lightmq { namespace qmapi {

#endif /* DECRYPTPROCESSOR_H_ */
