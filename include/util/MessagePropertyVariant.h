////////////////////////////////////////////////////////////////////////////////
//
// MessagePropertyVariant.h
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
#ifndef LIGHTMQ_UTIL_PROPERTYVARIANT_H_
#define LIGHTMQ_UTIL_PROPERTYVARIANT_H_
#include <string>
#include <boost/variant.hpp>
#include "TypeTraits.h"
namespace lightmq { namespace util{

class MessagePropertyVariant : public boost::static_visitor<void>{
public:
    enum VariantType : int32_t
    {
        VT_NULL = 0,
        VT_BOOL = 1,
        VT_BYTE = 2,
        VT_SHORT = 3,
        VT_INT = 4,
        VT_LONG = 5,
        VT_FLOAT = 6,
        VT_DOUBLE = 7,
        VT_STRING = 8,
        VT_BYTES = 9,
        VT_UNICODE_CHAR = 10,
    };
    MessagePropertyVariant() = default;

    explicit MessagePropertyVariant(const char* pString) {
        m_variant = std::move(std::string(pString));
        boost::apply_visitor(*this, m_variant);
    }

    MessagePropertyVariant(const int8_t* pByte, int length) {
        m_variant = std::move(std::string((const char*)pByte, length));
        m_type = VT_BYTES;
    }

    MessagePropertyVariant(const MessagePropertyVariant& val) {
        m_variant = val.m_variant;
        m_type = val.m_type;
    }

    MessagePropertyVariant(MessagePropertyVariant&& val) {
        m_variant = std::move(val.m_variant);
        m_type = val.m_type;
    }
     
    template<class T, class F = typename std::enable_if<is_varvalue_type<T>::value>::type>
    explicit MessagePropertyVariant(const T& rhs) : m_variant(rhs) {
        boost::apply_visitor(*this, m_variant);
    }

    template<class T, class F = typename std::enable_if<is_varvalue_type<T>::value>::type>
    explicit MessagePropertyVariant(T&& rhs) :m_variant(std::forward<T>(rhs)){
        boost::apply_visitor(*this, m_variant);
    }

    MessagePropertyVariant& operator = (const MessagePropertyVariant& val) {
        m_variant = val.m_variant;
        m_type = val.m_type;
        return *this;
    }

    MessagePropertyVariant& operator = (MessagePropertyVariant&& val) {
        m_variant = std::move(val.m_variant);
        m_type = val.m_type;
        return *this;
    }

    template<class T, class F = typename std::enable_if<is_varvalue_type<T>::value>::type>
    MessagePropertyVariant& operator = (const T& rhs) {
        m_variant = rhs;
        boost::apply_visitor(*this, m_variant);
        return *this;
    }
    template<class T, class F = typename std::enable_if<is_varvalue_type<T>::value>::type>
    MessagePropertyVariant& operator = (T&& rhs) {
        m_variant = std::forward<T>(rhs);
        boost::apply_visitor(*this, m_variant);
        return *this;
    }

    
    MessagePropertyVariant& operator = (const char* pString) {
        m_variant = std::move(std::string(pString));
        boost::apply_visitor(*this, m_variant);
        return *this;
    }


    void setBytes(const int8_t* pByte, int length) {
        m_variant = std::move(std::string((const char*)pByte, length));
        m_type = VT_BYTES;
    }

    void setBytes(std::string&& bytesString) {
        m_variant = std::move(bytesString);
        m_type = VT_BYTES;
    }

    // template<class T>
    // MessagePropertyVariant& operator = (const T& rhs) {
    //     m_variant = rhs;
    //     return *this;
    // }

    
    int32_t getType() const {
        return m_type;
    }

    void clear() {
        m_type = VT_NULL;
        m_variant = decltype(m_variant)();
    }
    
    void operator ()(bool b) {
        m_type = MessagePropertyVariant::VT_BOOL;
    }

    void operator ()(int8_t b) {
        m_type = MessagePropertyVariant::VT_BYTE;
    }

    void operator ()(int16_t s) {
        m_type = MessagePropertyVariant::VT_SHORT;
    }

    void operator ()(int32_t i) {
        m_type = MessagePropertyVariant::VT_INT;
    }

    void operator ()(int64_t i) {
        m_type = MessagePropertyVariant::VT_LONG;
    }

    void operator ()(float f) {
        m_type = MessagePropertyVariant::VT_FLOAT;
    }

    void operator ()(double f) {
        m_type = MessagePropertyVariant::VT_DOUBLE;
    }

    void operator ()(const std::string& s) {
        m_type = MessagePropertyVariant::VT_STRING;
    }

    void operator ()(uint16_t uc) {
        m_type = MessagePropertyVariant::VT_UNICODE_CHAR;
    }

    bool isNull() const {
        return m_type == MessagePropertyVariant::VT_NULL;
    }
    bool isBool() const{
        return m_type == MessagePropertyVariant::VT_BOOL;
    }

    bool isByte() const {
        return m_type == MessagePropertyVariant::VT_BYTE;
    }

    bool isShort() const {
        return m_type == MessagePropertyVariant::VT_SHORT;
    }

    bool isInteger() const {
        return m_type == MessagePropertyVariant::VT_INT;
    }

    bool isLong() const {
        return m_type == MessagePropertyVariant::VT_LONG;
    }

    bool isFloat() const {
        return m_type == MessagePropertyVariant::VT_FLOAT;
    }

    bool isDouble() const {
        return m_type == MessagePropertyVariant::VT_DOUBLE;
    }

    bool isString() const {
        return m_type == MessagePropertyVariant::VT_STRING;
    }

    bool isBytes() const {
        return m_type == MessagePropertyVariant::VT_BYTES;
    }

    bool isUnicodeChar() const{
        return m_type == MessagePropertyVariant::VT_UNICODE_CHAR;
    }

    bool isNumber() const {
        return isByte() || isShort() || isInteger() || isLong() || isFloat() || isDouble() || isUnicodeChar();
    }

    double getNumberValue() const {
        switch (m_type) {
        case VT_BYTE:
            return boost::get<int8_t>(m_variant);
        case VT_SHORT:
            return boost::get<int16_t>(m_variant);
        case VT_INT:
            return boost::get<int32_t>(m_variant);
        case VT_LONG:
            return boost::get<int64_t>(m_variant);
        case VT_FLOAT:
            return boost::get<float>(m_variant);
        case VT_DOUBLE:
            return boost::get<double>(m_variant);
        case VT_UNICODE_CHAR:
            return boost::get<uint16_t>(m_variant);
        default:
            return 0;
            
        }
    }

    bool getBooleanValue() const {
        return boost::get<bool>(m_variant);
    }

    int8_t getByteValue() const {
        return boost::get<int8_t>(m_variant);
    }

    int16_t getShortValue() const {
        return boost::get<int16_t>(m_variant);
    }

    int32_t getIntegerValue() const {
        return boost::get<int32_t>(m_variant);
    }

    int64_t getLongValue() const {
        return boost::get<int64_t>(m_variant);
    }

    float getFloatValue() const {
        return boost::get<float>(m_variant);
    }

    double getDoubleValue() const {
        return boost::get<double>(m_variant);
    }

    const std::string& getStringValue() const {
        return boost::get<std::string>(m_variant);
    }

    uint16_t getUnicodeCharValue() const {
        return boost::get<uint16_t>(m_variant);
    }
  
    bool getBooleanValue(bool& value) const{
        return getValue(value);
    }

    bool getByteValue(int8_t& value) const {
        return getValue(value);
    }

    bool getShortValue(int16_t& value) const {
        return getValue(value);
    }

    bool getIntegerValue(int32_t& value) const {
        return getValue(value);
    }

    bool getLongValue(int64_t& value) const {
        return getValue(value);
    }

    bool getFloatValue(float& value) const {
        return getValue(value);
    }

    bool getDoubleValue(double& value) const {
        return getValue(value);
    }

    bool getStringValue(std::string& value) const {
        return getValue(value);
    }

    bool getUnicodeCharValue(uint16_t& value) const {
        return getValue(value);
    }

    std::string toString() const {
        switch (m_type) {
        case VT_BOOL:
            return boost::get<bool>(m_variant) ? "true" : "false";
        case VT_BYTE:
            return std::to_string(boost::get<int8_t>(m_variant));
        case VT_SHORT:
            return std::to_string(boost::get<int16_t>(m_variant));
        case VT_INT:
            return std::to_string(boost::get<int32_t>(m_variant));
        case VT_LONG:
            return std::to_string(boost::get<int64_t>(m_variant));
        case VT_FLOAT:
            return std::to_string(boost::get<float>(m_variant));
        case VT_DOUBLE:
            return std::to_string(boost::get<double>(m_variant)) ;
        case VT_UNICODE_CHAR:
            return std::to_string(boost::get<uint16_t>(m_variant));
        case VT_STRING:
        case VT_BYTES:
            return boost::get<std::string>(m_variant);
        default:
            return 0;

        }
    }

private:
    template<class T>
    bool getValue(T& value) const {
        const T* pValue = boost::get<T>(&m_variant);
        if (pValue) {
            value = *pValue;
            return true;
        }
        return false;
    }
private:
    boost::variant<bool, int8_t, int16_t, uint16_t, int32_t, int64_t, float, double, std::string> m_variant;
    VariantType m_type = VT_NULL;
};

}}

#endif
