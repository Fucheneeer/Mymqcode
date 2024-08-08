////////////////////////////////////////////////////////////////////////////////
//
// ISymmetricEncryptionEngine.h
//
// Copyright (c) 2020 CFETSIT.
// Building 3, 491 YinDong Road, Shanghai, China.
// All rights reserved.
//
// LightMQ is the copyrighted,
// proprietary property of CFETSIT which retain all right, title and interest therein.
//
// Description: Interface of SymmetricEncryptionEngine class
//
////////////////////////////////////////////////////////////////////////////////

#ifndef ISYMMETRICENCRYPTIONENGINE_H_
#define ISYMMETRICENCRYPTIONENGINE_H_

#include <string>
#include "LightmqCryptoExport.h"

namespace dep {
    class ISeverityLog;
    class ByteBuffer;
}

namespace lightmq { namespace qmapi {


/**
 * Interface of symmetric encryption engine
 */
class LIGHTMQ_CRYPTO_API ISymmetricEncryptionEngine {
public:
    /**
     * The type of result code of encryption
     */
    enum class ResultCode : uint16_t {
        SUCCESS,            /**< Success*/
        BUFFER_NOT_ENOUGH,  /**< Cannot extend buffer to hold result */
        ERROR               /**< Error*/
    };

public:
    /**
     * Destructor
     */
    virtual ~ISymmetricEncryptionEngine() = default;

    /**
     * Set the logger
     *
     * @param logger
     */
    virtual void setLogger(dep::ISeverityLog* logger) = 0;

    /**
     * Set the encryption key
     *
     * @param key the emcryption key
     */
    virtual void setKey(const std::string& key) = 0;

    /**
     * Encrypt the input buffer and write the encrypted data to the output buffer.
     * In this method, the input buffer position will not be adjusted. However, for the output
     * buffer, the position will be set to the last slot of data if it return SUCCESS
     *
     * It should be noted that the output dep::ByteBuffer may expend automatically when it do not have
     * enough space to write the encrypted data. The space required depends on the underlying
     * implementation.
     *
     * @param input     Input buffer
     * @param output    Output buffer
     * @return  result code
     */
    virtual ResultCode encrypt(const dep::ByteBuffer& input, dep::ByteBuffer& output) = 0;

    /**
     * Decrypt the input buffer and write the decrypted data to the output buffer.
     * In this method, the input buffer position will not be adjusted. However, for the output
     * buffer, the position will be set to the last slot of data if it return SUCCESS
     *
     * It should be noted that the output dep::ByteBuffer may expend automatically when it do not have
     * enough space to write the decrypted data. The space required depends on the underlying
     * implementation.
     *
     * @param input     Input buffer
     * @param output    Output buffer
     * @return  result code
     */
    virtual ResultCode decrypt(const dep::ByteBuffer& input, dep::ByteBuffer& output) = 0;

};

}} // namespace lightmq { namespace qmapi {

#endif /* ISYMMETRICENCRYPTIONENGINE_H_ */
