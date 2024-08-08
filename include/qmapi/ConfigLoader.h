////////////////////////////////////////////////////////////////////////////////
//
// ConfigLoader.h
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

#pragma once
#include <string>
#include <vector>
#include "Properties.h"
#include "BaseProcessorAttribute.h"

namespace lightmq { namespace qmapi {

class QMAPIConfig;
class IDestination;
class ImplFactoryAttribute;

class ConfigLoader
{
public:
    ConfigLoader(const QMAPIConfig& config);
    void setDestination(IDestination *dest);
    bool readConfig(const std::string &prop, std::string &out_string);

    bool loadEncryptionConfig(dep::Properties &pluginProp, std::vector<ImplFactoryAttribute> &attributes);
    bool loadDecryptionConfig(dep::Properties &pluginProp, std::vector<ImplFactoryAttribute> &attributes);

    bool loadCompressConfig(dep::Properties &pluginProp, std::vector<ImplFactoryAttribute> &attributes);
    bool loadExtractConfig(dep::Properties &pluginProp, std::vector<ImplFactoryAttribute> &attributes);


private:
    bool loadCryptionConfig(dep::Properties &pluginProp, std::vector<ImplFactoryAttribute> &attributes, BaseProcessorAttribute::AttributeType at);
    bool loadCompressConfig(dep::Properties &pluginProp, std::vector<ImplFactoryAttribute> &attributes, BaseProcessorAttribute::AttributeType at);

private:
	const QMAPIConfig& m_config;
    IDestination *m_pDestination;
};

}} // end of namespace lightmq { namespace qmapi {
