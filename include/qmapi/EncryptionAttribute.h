////////////////////////////////////////////////////////////////////////////////
//
// EncryptionAttribute.h
//
// Copyright (c) 2020 CFETSIT.
// Building 3, 491 YinDong Road, Shanghai, China.
// All rights reserved.
//
// LightMQ is the copyrighted,
// proprietary property of CFETSIT which retain all right, title and interest therein.
//
// Description: Header of class EncryptionAttribute
//
////////////////////////////////////////////////////////////////////////////////

#ifndef ENCRYPTIONATTRIBUTE_H_
#define ENCRYPTIONATTRIBUTE_H_

#include "ImplFactoryAttribute.h"

namespace lightmq { namespace qmapi {

/**
 * Class to handle encryption
 */
class EncryptionAttribute : public ImplFactoryAttribute {

public:

    /**
     * Constructor
     *
     * @param implInfo       modInfo used in encryption
     */
    EncryptionAttribute(const std::string& implInfo);

    /**
     * Destructor
     */
    virtual ~EncryptionAttribute() = default;
};

}} // namespace lightmq { namespace qmapi {


#endif /* ENCRYPTIONATTRIBUTE_H_ */
