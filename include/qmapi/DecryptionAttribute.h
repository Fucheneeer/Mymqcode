////////////////////////////////////////////////////////////////////////////////
//
// DecryptionAttribute.h
//
// Copyright (c) 2020 CFETSIT.
// Building 3, 491 YinDong Road, Shanghai, China.
// All rights reserved.
//
// LightMQ is the copyrighted,
// proprietary property of CFETSIT which retain all right, title and interest therein.
//
// Description: Header of class DecryptionAttribute
//
////////////////////////////////////////////////////////////////////////////////

#ifndef DECRYPTIONATTRIBUTE_H_
#define DECRYPTIONATTRIBUTE_H_

#include "ImplFactoryAttribute.h"

namespace lightmq { namespace qmapi {

/**
 * Class to handle decryption
 */
class DecryptionAttribute : public ImplFactoryAttribute {

public:

    /**
     * Constructor
     *
     * @param implInfo       modInfo used in decryption
     */
    DecryptionAttribute(const std::string& implInfo);

    /**
     * Destructor
     */
    virtual ~DecryptionAttribute() = default;
};

}} // namespace lightmq { namespace qmapi {

#endif /* DECRYPTIONATTRIBUTE_H_ */
