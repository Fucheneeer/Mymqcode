////////////////////////////////////////////////////////////////////////////////
//
// ICryptoFactory.h
//
// Copyright (c) 2020 CFETSIT.
// Building 3, 491 YinDong Road, Shanghai, China.
// All rights reserved.
//
// LightMQ is the copyrighted,
// proprietary property of CFETSIT which retain all right, title and interest therein.
//
// Description: Interface of class CryptoFactory
//
////////////////////////////////////////////////////////////////////////////////

#ifndef ICRYPTOFACTORY_H_
#define ICRYPTOFACTORY_H_

#include <memory>
#include <string>
#include "LightmqCryptoExport.h"

namespace dep {
    class Properties;
}

namespace lightmq { namespace qmapi {

class ISymmetricEncryptionEngine;
class IAsymmetricEncryptionEngine;

/**
 * Interface of crypto factory
 */
class LIGHTMQ_CRYPTO_API ICryptoFactory {
public:
    /**
     * Destructor
     */
    virtual ~ICryptoFactory() = default;

    /**
     * Factory method to create a new engine according to the implementation string
     *
     * @param impl          Implementation string
     * @param key           The master key to be decrypted
     * @param properties    dep::Properties of the encryption criteria
     * @return nullptr if null
     */
    virtual std::unique_ptr<ISymmetricEncryptionEngine> createSymmetricEncryptionEngine(const std::string& impl, const std::string& key, const dep::Properties& properties) const = 0;

    /**
     * Factory method to create a new engine according to the implementation string
     *
     * @param impl          Implementation string
     * @param properties    dep::Properties of the encryption criteria
     * @return nullptr if null
     */
    virtual std::unique_ptr<ISymmetricEncryptionEngine> createSymmetricEncryptionEngine(const std::string& impl, const dep::Properties& properties) const = 0;
};

}} // namespace lightmq { namespace qmapi {

#endif /* ICRYPTOFACTORY_H_ */
