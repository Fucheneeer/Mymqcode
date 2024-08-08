////////////////////////////////////////////////////////////////////////////////
//
// IAsymmetricEncryptionEngine.h
//
// Copyright (c) 2020 CFETSIT.
// Building 3, 491 YinDong Road, Shanghai, China.
// All rights reserved.
//
// LightMQ is the copyrighted,
// proprietary property of CFETSIT which retain all right, title and interest therein.
//
// Description:
//
////////////////////////////////////////////////////////////////////////////////

#ifndef IASYMMETRICENCRYPTIONENGINE_H_
#define IASYMMETRICENCRYPTIONENGINE_H_

#include <cstdint>

#include "ByteBuffer.h"
#include "LightmqCryptoExport.h"

namespace lightmq { namespace qmapi {

class LIGHTMQ_CRYPTO_API IAsymmetricEncryptionEngine {

public:

    virtual ~IAsymmetricEncryptionEngine()=default;


    virtual std::string encrypt(const std::string& plainData) = 0;
    virtual std::string decrypt(const std::string& encryptedData) = 0;

};

}} // namespace lightmq { namespace qmapi {

#endif /* IASYMMETRICENCRYPTIONENGINE_H_ */
