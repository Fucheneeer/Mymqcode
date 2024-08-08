////////////////////////////////////////////////////////////////////////////////
//
// AddressTranslationUtil.h
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

#ifndef ADDRESSTRANSLATIONUTIL_H_
#define ADDRESSTRANSLATIONUTIL_H_

#include <unordered_map>
#include <mutex>

namespace lightmq { namespace util {

class AddressTranslationUtil {
public:
    AddressTranslationUtil()=delete;
    static const std::string& getEscapedDestination(const std::string& destination);
    static const std::string& getNonEscapedDestination(const std::string& destination);
private:
    static std::mutex cachelock;
    static std::unordered_map<std::string, std::string> m_escapeDestinationCache;
    static std::unordered_map<std::string, std::string> m_nonEscapedAddressMap;
};

}} // namespace lightmq { namespace util {

#endif /* ADDRESSTRANSLATIONUTIL_H_ */
