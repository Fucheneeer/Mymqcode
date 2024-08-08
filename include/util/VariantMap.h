////////////////////////////////////////////////////////////////////////////////
//
// VariantMap.h
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

#include <map>
#include <memory>
#include <unordered_map>
#include <vector>
//#include "MessagePropertyVariant.h"

namespace lightmq { namespace util {

class MessagePropertyVariant;


class VariantMap
{
// class InCompleteType;
public:
    VariantMap();
    VariantMap(const VariantMap &m);
	VariantMap(const std::shared_ptr<std::unordered_map<std::string, MessagePropertyVariant>>& props);
    VariantMap(std::shared_ptr<std::unordered_map<std::string, MessagePropertyVariant>>&& props);
    ~VariantMap() = default;
    VariantMap& operator=(const VariantMap &vmap);

    bool empty() const ;
    void swap(VariantMap &src);
    MessagePropertyVariant* find(const std::string &key) const;


    const std::unordered_map<std::string, MessagePropertyVariant>& getMap() const;
    std::unordered_map<std::string, MessagePropertyVariant>& ensureGetMap();

    std::shared_ptr<std::unordered_map<std::string, MessagePropertyVariant>> getMapPtr() const;

    size_t size() const;
    // property
    bool addBooleanProperty(const std::string &key, const bool &v);
    bool addByteProperty(const std::string &key, const int8_t &v);
    bool addShortProperty(const std::string& key, const int16_t& v);
    bool addUniCodeCharProperty(const std::string& key, const uint16_t& v);
    bool addIntegerProperty(const std::string &key, const int32_t &v);
    bool addLongProperty(const std::string &key, const int64_t &v);
    bool addFloatProperty(const std::string &key, const float &v);
    bool addDoubleProperty(const std::string &key, const double &v);
    bool addStringProperty(const std::string& key, const std::string& v);
    bool addBytesProperty(const std::string &key, const int8_t* pBytes, int length);
    bool addVariantProperty(const std::string &key, const MessagePropertyVariant &v);

    bool getBooleanProperty(const std::string &key, bool &v) const;
    bool getByteProperty(const std::string &key, int8_t &v) const;
    bool getShortProperty(const std::string &key, int16_t &v) const;
    bool getIntegerProperty(const std::string &key, int32_t &v) const;
    bool getLongProperty(const std::string &key, int64_t &v) const;
    bool getFloatProperty(const std::string &key, float &v) const;
    bool getDoubleProperty(const std::string &key, double &v) const;
    bool getStringProperty(const std::string &key, std::string &v) const;
    const MessagePropertyVariant* getVariantProperty(const std::string &key) const;
    void clearProperties();
    void keys(std::vector<std::string> &keys) const;
    void removeProperty(const std::string &key);

    std::string toString() const;
private:

    std::shared_ptr<std::unordered_map<std::string, MessagePropertyVariant>> m_props;
    // std::shared_ptr<InCompleteType> m_props;
    // std::unique_ptr<InCompleteType> m_props;
};



}}

