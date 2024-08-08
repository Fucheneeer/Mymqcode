////////////////////////////////////////////////////////////////////////////////
//
// BaseProcessorAttribute.h
//
// Copyright (c) 2020 CFETSIT.
// Building 3, 491 YinDong Road, Shanghai, China.
// All rights reserved.
//
// LightMQ is the copyrighted,
// proprietary property of CFETSIT which retain all right, title and interest therein.
//
// Description: Header file of BaseProcessorAttribute
//
////////////////////////////////////////////////////////////////////////////////

#ifndef BASEPROCESSORATTRIBUTE_H_
#define BASEPROCESSORATTRIBUTE_H_

#include <string>

namespace lightmq { namespace qmapi {

/**
 * Technical attribute.
 */
class BaseProcessorAttribute {
public:

    enum class AttributeType {
        SOURCE_TIME_PERIOD,         /**< SOURCE_TIME_PERIOD */
        SEQUENCE_NUMBER,            /**< SEQUENCE_NUMBER */
        PRIORITY,                   /**< PRIORITY */
        MESSAGE_FILTER,             /**< MESSAGE_FILTER */
        COMPRESSION,                /**< COMPRESSION */
        DECOMPRESSION,              /**< DECOMPRESSION */
        ENCRYPTION,                 /**< ENCRYPTION */
        DECRYPTION,                 /**< DECRYPTION */
        THROTTLING,                 /**< Message throttling */
        SNAPSHOT                    /**< Message Snapshot */
    };

    /**
     * Constructor.
     */
    BaseProcessorAttribute(AttributeType attributeType)
        : m_attributeType{attributeType} {
    }

    /**
     * Destructor.
     */
    virtual ~BaseProcessorAttribute() = default;

    /**
     * Get the attribute type
     *
     * @return the attribute type
     */
    AttributeType getAttributeType() {
        return m_attributeType;
    }

protected:
    AttributeType m_attributeType;
};

}} // namespace lightmq { namespace qmapi {

#endif /* BASEPROCESSORATTRIBUTE_H_ */
