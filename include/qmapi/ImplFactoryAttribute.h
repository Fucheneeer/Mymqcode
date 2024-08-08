////////////////////////////////////////////////////////////////////////////////
//
// ImplFactoryAttribute.h
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

#ifndef IMPLFACTORYATTRIBUTE_H_
#define IMPLFACTORYATTRIBUTE_H_

#include "BaseProcessorAttribute.h"

#include <string>
#include <vector>

#include "Properties.h"

namespace lightmq { namespace qmapi {

class ImplFactoryAttribute : public BaseProcessorAttribute {
public:
    /**
     * The struct of configuration implementation and properties
     */
    struct Configuration {
        std::string implementation;     /**< Configuration implementation*/
        dep::Properties properties;          /**< Configuration properties*/
    };

    static const std::string CONFIG_DELIMIER;
    static const std::string IMPL_DELIMIER;
    static const std::string FIELD_DELIMIER;
    static const std::string KEY_VALUE_PAIR_DELIMIER;

public:
    /**
     * Constructor
     * @param implInfo Implementation String
     */
    explicit ImplFactoryAttribute(AttributeType attributeType, const std::string& implInfo);

    /**
     * Destructor
     */
    virtual ~ImplFactoryAttribute() = default;

    /**
     * Get the configuration vector
     * @return configuration vector
     */
    const std::vector<Configuration>& getConfiguration() const {
        return m_configuration;
    }

    const std::string& getImplInfo() const;

private:

    virtual void parseConfiguration(const std::string& configValue);

private:
    const std::string m_implInfo;
    std::vector<Configuration> m_configuration;
};

inline const std::string& ImplFactoryAttribute::getImplInfo() const {
    return m_implInfo;
}

}} // namespace lightmq { namespace qmapi {

#endif /* IMPLFACTORYATTRIBUTE_H_ */
