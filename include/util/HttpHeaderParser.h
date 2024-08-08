////////////////////////////////////////////////////////////////////////////////
//
// HttpHeaderParser.h
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

#ifndef HTTPHEADER_PARSER_H_
#define HTTPHEADER_PARSER_H_
#include <vector>
#include <string>
#include "MessagePropertyVariant.h"
#include "DEPException.h"

namespace lightmq { namespace util{

struct HttpHeaderPropertyKeyInfo {
    std::string keyName;
    int32_t msgIndex{ -1 };
};

class HttpHeaderParseException : dep::DEPException {
    explicit HttpHeaderParseException(const std::string& message) noexcept : dep::DEPException(message){
    }
};

class HttpHeaderParser {
public:
    /**
     *
     * parseInt32List   "1,2,3" -> vector<int32_t>{1, 2, 3}
     *
     * @param input 
     *  	string to parse
     * @param v
     * 		result vector
     * @return 
     *      return true is parse successfully
     *  
     */
    static bool parseInt32List(const std::string& input, std::vector<int32_t>& v);

    /**
     *
     * parseInt64List   "1,2,3" -> vector<int64_t>{1, 2, 3}
     *
     * @param input
     *  	string to parse
     * @param v
     * 		result vector
     * @return
     *      return true is parse successfully
     *
     */
    static bool parseInt64List(const std::string& input, std::vector<int64_t>& v);


    /**
     *
     * parseInt64   "1" -> 1
     *
     * @param input
     *  	string to parse
     * @param v
     * 		result value
     * @return
     *      return true is parse successfully
     *
     */
    static bool parseInt64(const std::string& input, int64_t& v);

    /**
     *
     * parseDoubleList   "1.0,2.0,3.0" -> vector<int64_t>{1.0, 2.0, 3.0}
     *
     * @param input
     *  	string to parse
     * @param v
     * 		result vector
     * @return
     *      return true is parse successfully
     *
     */
    static bool parseDoubleList(const std::string& input, std::vector<double>& v);

    /**
     *
     * parsePropertyKey   
     * pattern "lmq-msg{index}-property-{key}"
     * if no index exists, parsed {index} value is -1, and the {key} obey the C language variable's rule
     * "lmq-msg-property-A" -> HttpHeaderPropertyKeyInfo{"A", -1}
     * "lmq-msg100-property-A" -> HttpHeaderPropertyKeyInfo{"A", 100}
     *
     * @param input
     *  	string to parse
     * @param propInfo
     * 		result HttpHeaderPropertyKeyInfo
     * @return
     *      return true is parse successfully
     *
     */
    static bool parsePropertyKey(const std::string& input, HttpHeaderPropertyKeyInfo& propInfo);


    /**
     *
     * parsePropertyKey
     * the expected value type will be one of bool long double string
     * @param input
     *  	string to parse
     * @return
     *      parsed result
     *
     */
    static MessagePropertyVariant parsePropertyValue(const std::string& input);
};

}}


#endif //HTTPHEADER_PARSER_H_
