////////////////////////////////////////////////////////////////////////////////
//
// LockedPropertiesUtility.h
//
// Copyright (c) 2020 CFETSIT.
// Building 3, 491 YinDong Road, Shanghai, China.
// All rights reserved.
//
// Distributed Service Platform (DSP) is the copyrighted,
// proprietary property of CFETSIT which retain all right, title and interest therein.
//
// Description: Header file of LockedPropertiesUtility
//
////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <mutex>
#include "PropertiesUtility.h"
#include "LockedProperties.h"

namespace lightmq { 
    namespace util{

class LockedPropertiesUtility {
public:
    /**
     * Disable constructor.
     */
    LockedPropertiesUtility() = delete;

public:

    template<typename T>
    static std::pair<bool, T> get(const LockedProperties& prop, const std::string& key);

    template<typename T>
    static T getWithDefault(const LockedProperties& prop, const std::string& key, T defaultValue);

    template<typename T>
    static T getWithDefault(const LockedProperties& prop, const std::string& key, T defaultValue, T minValue, T maxValue);

    template<typename T>
    static bool get(const LockedProperties& prop, const std::string& key, T& value);
private:
    template<typename T>
    static T parseIntegral(const std::string& input);

};

template<typename T>
inline bool LockedPropertiesUtility::get(const LockedProperties& prop, const std::string& key, T& value) {
    std::lock_guard<LockedProperties> lock(const_cast<LockedProperties&>(prop));
    return dep::PropertiesUtility::get<T>(prop.getProperties(), key, value);
}

template<typename T>
inline std::pair<bool, T> LockedPropertiesUtility::get(const LockedProperties& prop, const std::string& key) {
    std::lock_guard<LockedProperties> lock(const_cast<LockedProperties&>(prop));
    return dep::PropertiesUtility::get<T>(prop.getProperties(), key);
}

template<typename T>
inline T LockedPropertiesUtility::getWithDefault(const LockedProperties& prop, const std::string& key, T defaultValue) {
    std::lock_guard<LockedProperties> lock(const_cast<LockedProperties&>(prop));
    return dep::PropertiesUtility::getWithDefault<T>(prop.getProperties(), key, defaultValue);
}


template<typename T>
inline T LockedPropertiesUtility::getWithDefault(const LockedProperties& prop, const std::string& key, T defaultValue, T minValue, T maxValue) {
    std::lock_guard<LockedProperties> lock(const_cast<LockedProperties&>(prop));
    return dep::PropertiesUtility::getWithDefault<T>(prop.getProperties(), key, defaultValue, minValue, maxValue);
}


template<typename T>
inline T  LockedPropertiesUtility::parseIntegral(const std::string& input) {
    return input; // default implementation for string
}

template<>
inline uint16_t LockedPropertiesUtility::parseIntegral(const std::string& input) {
    return std::stoul(input);
}

template<>
inline int32_t LockedPropertiesUtility::parseIntegral(const std::string& input) {
    return std::stol(input);
}

template<>
inline int64_t LockedPropertiesUtility::parseIntegral(const std::string& input) {
    return std::stoll(input);
}

}}  // namespace dep

