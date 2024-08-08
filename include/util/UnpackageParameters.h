////////////////////////////////////////////////////////////////////////////////
//
// UnpackageParameters.h
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
#ifndef UNPACKAGE_ORDERED_PARAMETERS_H_
#define UNPACKAGE_ORDERED_PARAMETERS_H_
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <google/protobuf/wire_format_lite.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>
#define NO_METAOBJ
#ifndef NO_METAOBJ
#include "MetaObject.h"
#endif
#include "ParameterPackagingException.h"
#include "ByteBuffer.h"
#include "TypeTraits.h"
#include "MessagePropertyVariant.h"


//�����˳�����еĲ���

#define CHECK_PARAMINDEX_EQ_FIELDNUMBER(paramIndex) uint32_t tag = m_codeInputStream.ReadTag();    \
if(tag == 0){   \
    std::ostringstream ostr;         \
    ostr << "Parameter index " << paramIndex << ": the input stream had consumed over"; \
    throw ParameterUnpackagingException(ostr.str()); \
}   \
int fieldNumer = WireFormatLite::GetTagFieldNumber(tag);   \
if (fieldNumer != paramIndex) { \
    std::ostringstream ostr;         \
    ostr << "Pb field number(" << fieldNumer << ") not euqal to parameter index(" << paramIndex << ")"; \
    throw ParameterUnpackagingException(ostr.str()); \
}

#define CHECK_READING_SUCCESS(type) \
    if (!r) {       \
        std::ostringstream ostr;     \
        ostr << "Parameter index " << paramIndex << ": read " << #type << " value error"; \
        throw ParameterUnpackagingException(ostr.str()); \
    }
#define CHECK_READING_UNORDERED_SUCCESS(type) \
    if (!r) {       \
        std::ostringstream ostr;     \
        int paramIndex = WireFormatLite::GetTagFieldNumber(tag);        \
        ostr << "Parameter index " << paramIndex << ": read " << #type << " value error"; \
        throw ParameterUnpackagingException(ostr.str()); \
    }


namespace lightmq{ namespace util{

class CodeInputStreamHandler {
public:
    using WireFormatLite = google::protobuf::internal::WireFormatLite;
    using CodedInputStream = google::protobuf::io::CodedInputStream;

    inline CodeInputStreamHandler(CodedInputStream& inputStream);


    inline void read(int paramIndex, bool& param);
    inline bool read(bool& param);
    inline bool readInVector(bool& param);
    inline void readUnordered(int tag, bool& param);

    inline void read(int paramIndex, char& param);
    inline bool read(char& param);
    inline bool readInVector(char& param);
    inline void readUnordered(int tag, char& param);

    inline void read(int paramIndex, int8_t& param);
    inline bool read(int8_t& param);
    inline bool readInVector(int8_t& param);
    inline void readUnordered(int tag, int8_t& param);

    inline void read(int paramIndex, uint8_t& param);
    inline bool read(uint8_t& param);
    inline bool readInVector(uint8_t& param);
    inline void readUnordered(int tag, uint8_t& param);

    inline void read(int paramIndex, int16_t& param);
    inline bool read(int16_t& param);
    inline bool readInVector(int16_t& param);
    inline void readUnordered(int tag, int16_t& param);

    inline void read(int paramIndex, uint16_t& param);
    inline bool read(uint16_t& param);
    inline bool readInVector(uint16_t& param);
    inline void readUnordered(int tag, uint16_t& param);

    inline void read(int paramIndex, int32_t& param);
    inline bool read(int32_t& param);
    inline bool readInVector(int32_t& param);
    inline void readUnordered(int tag, int32_t& param);

    inline void read(int paramIndex, uint32_t& param);
    inline bool read(uint32_t& param);
    inline bool readInVector(uint32_t& param);
    inline void readUnordered(int tag, uint32_t& param);

    inline void read(int paramIndex, int64_t& param);
    inline bool read(int64_t& param);
    inline bool readInVector(int64_t& param);
    inline void readUnordered(int tag, int64_t& param);

    inline void read(int paramIndex, uint64_t& param);
    inline bool read(uint64_t& param);
    inline bool readInVector(uint64_t& param);
    inline void readUnordered(int tag, uint64_t& param);

    inline void read(int paramIndex, float& param);
    inline bool read(float& param);
    inline bool readInVector(float& param);
    inline void readUnordered(int tag, float& param);

    inline void read(int paramIndex, double& param);
    inline bool read(double& param);
    inline bool readInVector(double& param);
    inline void readUnordered(int tag, double& param);

    inline void read(int paramIndex, std::string& param);
    inline bool read(std::string& param);
    inline bool readInVector(std::string& param);
    inline void readUnordered(int tag, std::string& param);

    inline void read(int paramIndex, MessagePropertyVariant& param);
    inline bool read(MessagePropertyVariant& param);
    inline bool readInVector(MessagePropertyVariant& param);
    inline void readUnordered(int tag, MessagePropertyVariant& param);

    //inline void readUnordered(int tag, MessagePropertyVariant& variant);

#ifndef NO_METAOBJ
    inline void read(int paramIndex, dep::MetaObject& param);
    inline bool read(dep::MetaObject& param);
    inline bool readInVector(dep::MetaObject& param);
    inline void readUnordered(int tag, dep::MetaObject& param);

    template<typename T, bool IsMetaobject = std::is_base_of<dep::MetaObject, T>::value>
    inline void read(int paramIndex, std::shared_ptr<T>& param);
    template<typename T, bool IsMetaobject = std::is_base_of<dep::MetaObject, T>::value>
    inline bool read(std::shared_ptr<T>& param);
    template<typename T, bool IsMetaobject = std::is_base_of<dep::MetaObject, T>::value>
    inline bool readInVector(std::shared_ptr<T>& param);
    template<typename T, bool IsMetaobject = std::is_base_of<dep::MetaObject, T>::value>
    inline void readUnordered(int tag, std::shared_ptr<T>& param);
#endif
    
    template<typename T, typename F = typename std::enable_if<is_list_container<T>::value>::type>
    inline void read(int paramIndex, T& param);
    template<typename T, typename F = typename std::enable_if<is_list_container<T>::value>::type>
    inline void readInVector(T& param);
    template<typename T, typename F = typename std::enable_if<is_list_container<T>::value>::type>
    inline void readUnordered(int tag, T& param);

    template<typename T, typename F = typename std::enable_if<is_map<T>::value>::type, int I = 1>
    inline void read(int paramIndex, T& param);
    template<typename T, typename F = typename std::enable_if<is_map<T>::value>::type, int I = 1>
    inline void readInVector(T& param);
    template<typename T, typename F = typename std::enable_if<is_map<T>::value>::type, int I = 1>
    inline void readUnordered(int tag, T& param);

    template<typename T, typename F = typename std::enable_if<is_emplace_back<T>::value>::type>
    inline void emplace_back(T & container, typename T::value_type && val) {
        container.emplace_back(std::move(val));
    }

    template<typename T, typename F = typename std::enable_if<is_emplace<T>::value>::type, int I = 1>
    inline void emplace_back(T & container, typename T::value_type && val) {
        container.emplace(std::move(val));
    }

private:
    CodedInputStream& m_codeInputStream;
};



inline CodeInputStreamHandler::CodeInputStreamHandler(CodedInputStream& inputStream) : m_codeInputStream(inputStream) {
}

//variant
inline void CodeInputStreamHandler::read(int paramIndex, MessagePropertyVariant& param) {
    CHECK_PARAMINDEX_EQ_FIELDNUMBER(paramIndex);
    bool r = read(param);
    CHECK_READING_SUCCESS(MessagePropertyVariant);
}


inline bool CodeInputStreamHandler::read(MessagePropertyVariant& param) {
    uint32_t length;
    if (!m_codeInputStream.ReadVarint32(&length)) {
        return false;
    }
    if (0 == length) {
        param.clear();
        return true;
    }
    int bufferLength{ 0 };
    const void* bufferStart{ nullptr };
    m_codeInputStream.GetDirectBufferPointer(&bufferStart, &bufferLength);
    if ((uint32_t)bufferLength < length) {
        throw ParameterUnpackagingException("parse MessagePropertyVariant no enough length");
    }
    
    google::protobuf::io::ArrayInputStream arrayInputStream(bufferStart, length);
    CodedInputStream codeInputStream(&arrayInputStream);
    CodeInputStreamHandler inputStreamHandler(codeInputStream);
    uint32_t tag = codeInputStream.ReadTag();
    if (tag == 0) {
        throw ParameterUnpackagingException("parse MessagePropertyVariant encounter zero tag");
    }
    int fieldNumer = WireFormatLite::GetTagFieldNumber(tag);

#define FIELD_CASE(enum_name, type_name) case MessagePropertyVariant::enum_name:{ \
    type_name value = type_name{};    \
    inputStreamHandler.read(value); \
    param = std::move(value);   \
    break;} 


    switch (fieldNumer)
    {
        FIELD_CASE(VT_BOOL, bool)
        FIELD_CASE(VT_BYTE, int8_t)
        FIELD_CASE(VT_SHORT, int16_t)
        FIELD_CASE(VT_INT, int32_t)
        FIELD_CASE(VT_LONG, int64_t)
        FIELD_CASE(VT_FLOAT, float)
        FIELD_CASE(VT_DOUBLE, double)
        FIELD_CASE(VT_UNICODE_CHAR, uint16_t)
        //FIELD_CASE(VT_STRING, std::string) 
    case MessagePropertyVariant::VT_STRING:
        {
            std::string value;
            inputStreamHandler.read(value); 
            param = std::move(value);             
            break; 
        }
    case MessagePropertyVariant::VT_BYTES: {
            std::string value;
            inputStreamHandler.read(value);
            param.setBytes(std::move(value));
            break;
        }
    default:
        break;
    }

    m_codeInputStream.Skip(length);
    return true;
}

bool CodeInputStreamHandler::readInVector(MessagePropertyVariant& param) {
    return read(param);
}


//read bool
inline void CodeInputStreamHandler::read(int paramIndex, bool& param) {
    CHECK_PARAMINDEX_EQ_FIELDNUMBER(paramIndex);
    bool r = read(param);
    CHECK_READING_SUCCESS(bool);
}

inline bool CodeInputStreamHandler::read(bool& param) {
    return WireFormatLite::ReadPrimitive<bool, WireFormatLite::TYPE_BOOL>(&m_codeInputStream, &param);
}

inline bool CodeInputStreamHandler::readInVector(bool& param) {
    return read(param);
}

inline void CodeInputStreamHandler::readUnordered(int tag, bool& param) {
    bool r = read(param);
    CHECK_READING_UNORDERED_SUCCESS(bool);
}


void CodeInputStreamHandler::readUnordered(int tag, MessagePropertyVariant& param){
    
}

//read char
inline void CodeInputStreamHandler::read(int paramIndex, char& param) {
    CHECK_PARAMINDEX_EQ_FIELDNUMBER(paramIndex);
    bool r = read(param);
    CHECK_READING_SUCCESS(char);
}

inline bool CodeInputStreamHandler::read(char& param) {
    int32_t temp;
    if (!read(temp)) {
        return false;
    }
    param = static_cast<char>(temp);
    return true;
}

inline bool CodeInputStreamHandler::readInVector(char& param) {
    return read(param);
}

inline void CodeInputStreamHandler::readUnordered(int tag, char& param) {
    bool r = read(param);
    CHECK_READING_UNORDERED_SUCCESS(char);
}

//read int8t
inline void CodeInputStreamHandler::read(int paramIndex, int8_t& param) {
    CHECK_PARAMINDEX_EQ_FIELDNUMBER(paramIndex);
    bool r = read(param);
    CHECK_READING_SUCCESS(int8_t);
}

inline bool CodeInputStreamHandler::read(int8_t& param) {
    int32_t temp;
    if (!read(temp)) {
        return false;
    }
    param = static_cast<int8_t>(temp);
    return true;
}

inline bool CodeInputStreamHandler::readInVector(int8_t& param) {
    return read(param);
}

inline void CodeInputStreamHandler::readUnordered(int tag, int8_t& param) {
    bool r = read(param);
    CHECK_READING_UNORDERED_SUCCESS(int8_t);
}

//read uint8_t
inline void CodeInputStreamHandler::read(int paramIndex, uint8_t& param) {
    CHECK_PARAMINDEX_EQ_FIELDNUMBER(paramIndex);
    bool r = read(param);
    CHECK_READING_SUCCESS(uint8_t);
}

inline bool CodeInputStreamHandler::read(uint8_t& param) {
    uint32_t temp;
    if (!read(temp)) {
        return false;
    }
    param = static_cast<uint8_t>(temp);
    return true;
}

inline bool CodeInputStreamHandler::readInVector(uint8_t& param) {
    return read(param);
}

inline void CodeInputStreamHandler::readUnordered(int tag, uint8_t& param) {
    bool r = read(param);
    CHECK_READING_UNORDERED_SUCCESS(uint8_t);
}

//read int16
inline void CodeInputStreamHandler::read(int paramIndex, int16_t& param) {
    CHECK_PARAMINDEX_EQ_FIELDNUMBER(paramIndex);
    bool r = read(param);
    CHECK_READING_SUCCESS(int16_t);
}

inline bool CodeInputStreamHandler::read(int16_t& param) {
    int32_t temp;
    if (!read(temp)) {
        return false;
    }
    param = static_cast<int16_t>(temp);
    return true;
}

inline bool CodeInputStreamHandler::readInVector(int16_t& param) {
    return read(param);
}

inline void CodeInputStreamHandler::readUnordered(int tag, int16_t& param) {
    bool r = read(param);
    CHECK_READING_UNORDERED_SUCCESS(int16_t);
}

//read uint16
inline void CodeInputStreamHandler::read(int paramIndex, uint16_t& param) {
    CHECK_PARAMINDEX_EQ_FIELDNUMBER(paramIndex);
    bool r = read(param);
    CHECK_READING_SUCCESS(uint16_t);
}

inline bool CodeInputStreamHandler::read(uint16_t& param) {
    uint32_t temp;
    if (!read(temp)) {
        return false;
    }
    param = static_cast<uint16_t>(temp);
    return true;
}

inline bool CodeInputStreamHandler::readInVector(uint16_t& param) {
    return read(param);
}

inline void CodeInputStreamHandler::readUnordered(int tag, uint16_t& param) {
    bool r = read(param);
    CHECK_READING_UNORDERED_SUCCESS(uint16_t);
}

//read int32
inline void CodeInputStreamHandler::read(int paramIndex, int32_t& param) {
    CHECK_PARAMINDEX_EQ_FIELDNUMBER(paramIndex);
    bool r = read(param);
    CHECK_READING_SUCCESS(int32_t);
}

inline bool CodeInputStreamHandler::read(int32_t& param) {
    return WireFormatLite::ReadPrimitive<int32_t, WireFormatLite::TYPE_INT32>(&m_codeInputStream, &param);
}

inline bool CodeInputStreamHandler::readInVector(int32_t& param) {
    return read(param);
}

inline void CodeInputStreamHandler::readUnordered(int tag, int32_t& param) {
    bool r = read(param);
    CHECK_READING_UNORDERED_SUCCESS(int32_t);
}

//read uint32
inline void CodeInputStreamHandler::read(int paramIndex, uint32_t& param) {
    CHECK_PARAMINDEX_EQ_FIELDNUMBER(paramIndex);
    bool r = read(param);
    CHECK_READING_SUCCESS(uint32_t);
}

inline bool CodeInputStreamHandler::read(uint32_t& param) {
    return WireFormatLite::ReadPrimitive<uint32_t, WireFormatLite::TYPE_UINT32>(&m_codeInputStream, &param);
}

inline bool CodeInputStreamHandler::readInVector(uint32_t& param) {
    return read(param);
}

inline void CodeInputStreamHandler::readUnordered(int tag, uint32_t& param) {
    bool r = read(param);
    CHECK_READING_UNORDERED_SUCCESS(uint32_t);
}

//read int64
inline void CodeInputStreamHandler::read(int paramIndex, int64_t& param) {
    CHECK_PARAMINDEX_EQ_FIELDNUMBER(paramIndex);
    bool r = read(param);
    CHECK_READING_SUCCESS(int64_t);
}

inline bool CodeInputStreamHandler::read(int64_t& param) {
    return WireFormatLite::ReadPrimitive<int64_t, WireFormatLite::TYPE_INT64>(&m_codeInputStream, &param);
}

inline bool CodeInputStreamHandler::readInVector(int64_t& param) {
    return read(param);
}

inline void CodeInputStreamHandler::readUnordered(int tag, int64_t& param) {
    bool r = read(param);
    CHECK_READING_UNORDERED_SUCCESS(int64_t);
}


//read uint64
inline void CodeInputStreamHandler::read(int paramIndex, uint64_t& param) {
    CHECK_PARAMINDEX_EQ_FIELDNUMBER(paramIndex);
    bool r = read(param);
    CHECK_READING_SUCCESS(uint64_t);
}

inline bool CodeInputStreamHandler::read(uint64_t& param) {
    return WireFormatLite::ReadPrimitive<uint64_t, WireFormatLite::TYPE_UINT64>(&m_codeInputStream, &param);
}

inline bool CodeInputStreamHandler::readInVector(uint64_t& param) {
    return read(param);
}

inline void CodeInputStreamHandler::readUnordered(int tag, uint64_t& param) {
    bool r = read(param);
    CHECK_READING_UNORDERED_SUCCESS(uint64_t);
}

//read float
inline void CodeInputStreamHandler::read(int paramIndex, float& param) {
    CHECK_PARAMINDEX_EQ_FIELDNUMBER(paramIndex);
    bool r = read(param);
    CHECK_READING_SUCCESS(float);
}

inline bool CodeInputStreamHandler::read(float& param) {
    return WireFormatLite::ReadPrimitive<float, WireFormatLite::TYPE_FLOAT>(&m_codeInputStream, &param);
}

inline bool CodeInputStreamHandler::readInVector(float& param) {
    return read(param);
}

inline void CodeInputStreamHandler::readUnordered(int tag, float& param) {
    bool r = read(param);
    CHECK_READING_UNORDERED_SUCCESS(float);
}

//read double
inline void CodeInputStreamHandler::read(int paramIndex, double& param) {
    CHECK_PARAMINDEX_EQ_FIELDNUMBER(paramIndex);
    bool r = read(param);
    CHECK_READING_SUCCESS(double);
}

inline bool CodeInputStreamHandler::read(double& param) {
    return WireFormatLite::ReadPrimitive<double, WireFormatLite::TYPE_DOUBLE>(&m_codeInputStream, &param);
}

inline bool CodeInputStreamHandler::readInVector(double& param) {
    return read(param);
}

inline void CodeInputStreamHandler::readUnordered(int tag, double& param) {
    bool r = read(param);
    CHECK_READING_UNORDERED_SUCCESS(double);
}

//read string
inline void CodeInputStreamHandler::read(int paramIndex, std::string& param) {
    CHECK_PARAMINDEX_EQ_FIELDNUMBER(paramIndex);
    bool r = read(param);
    CHECK_READING_SUCCESS(string);
}

inline bool CodeInputStreamHandler::read(std::string& param) {
    return WireFormatLite::ReadString(&m_codeInputStream, &param);
}

inline bool CodeInputStreamHandler::readInVector(std::string& param) {
    return read(param);
}

inline void CodeInputStreamHandler::readUnordered(int tag, std::string& param) {
    bool r = read(param);
    CHECK_READING_UNORDERED_SUCCESS(string);
}

#ifndef NO_METAOBJ
//read metaobject
inline void CodeInputStreamHandler::read(int paramIndex, dep::MetaObject& param) {
    CHECK_PARAMINDEX_EQ_FIELDNUMBER(paramIndex);
    try {
        read(param);
    }
    catch (ParameterUnpackagingException& ex) {
        std::ostringstream errorInfo;
        errorInfo << "Parameter index " << paramIndex << ": " << ex.what();
        throw ParameterUnpackagingException(errorInfo.str());
    }
    //CHECK_READING_SUCCESS(MetaObject);
}

inline bool CodeInputStreamHandler::read(dep::MetaObject& param) {
    uint32_t length;
    if (!m_codeInputStream.ReadVarint32(&length)) {
        throw ParameterUnpackagingException("failed to read metaobject length");
    }

    int bufferLength{ 0 };
    const void *bufferStart{ nullptr };
    m_codeInputStream.GetDirectBufferPointer(&bufferStart, &bufferLength);
    if ((uint32_t)bufferLength < length) {
        std::ostringstream errorInfo;
        errorInfo << "parse metaobject no enough length";
        throw ParameterUnpackagingException(errorInfo.str());
    }
    
    param.deserialize((uint8_t*)bufferStart, length);
    m_codeInputStream.Skip(length);
    return true;
}

inline bool CodeInputStreamHandler::readInVector(dep::MetaObject& param) {
    return read(param);
}

inline void CodeInputStreamHandler::readUnordered(int tag, dep::MetaObject& param) {
    try {
        read(param);
    }
    catch (ParameterUnpackagingException& ex) {
        std::ostringstream errorInfo;
        int paramIndex = WireFormatLite::GetTagFieldNumber(tag);
        errorInfo << "Parameter index " << paramIndex << ": " << ex.what();
        throw ParameterUnpackagingException(errorInfo.str());
    }
    //CHECK_READING_SUCCESS(MetaObject);
}

//shared_ptr<Metaobject>
template<typename T, bool IsMetaobject>
inline void CodeInputStreamHandler::read(int paramIndex, std::shared_ptr<T>& param) {
    param.reset(static_cast<T*>(T::CreateMetaObject()));
    read(paramIndex, *param);
}
template<typename T, bool IsMetaobject>
inline bool CodeInputStreamHandler::read(std::shared_ptr<T>& param) {
    param.reset(static_cast<T*>(T::CreateMetaObject()));
    return read(*param);
}

template<typename T, bool IsMetaobject>
inline bool CodeInputStreamHandler::readInVector(std::shared_ptr<T>& param) {
    return read(param);
}

template<typename T, bool IsMetaobject>
inline void CodeInputStreamHandler::readUnordered(int tag, std::shared_ptr<T>& param) {
    read(param);
}
#endif

//read vector
template<typename T, typename F/* = typename std::enable_if<is_list_container<T>::value>::type*/>
inline void CodeInputStreamHandler::read(int paramIndex, T& param) {
    //first element
    CHECK_PARAMINDEX_EQ_FIELDNUMBER(paramIndex);
    try {
        typename T::value_type value;
        readInVector(value);
        emplace_back(param, std::move(value));
        while (m_codeInputStream.ExpectTag(tag)) {
            typename T::value_type value;
            readInVector(value);
            emplace_back(param, std::move(value));
        }
    }
    catch (ParameterUnpackagingException& ex) {
        std::ostringstream ostr;
        ostr << "Parameter index " << paramIndex << ": " << ex.what();
        throw ParameterUnpackagingException(ostr.str());
    }
}

template<typename T, typename F/* = typename std::enable_if<is_list_container<T>::value>::type*/>
inline void CodeInputStreamHandler::readInVector(T& param) {
    //read key value message length
    uint32_t length;
    if (!m_codeInputStream.ReadVarint32(&length)) {
        throw ParameterUnpackagingException("failed to read key-value message length in map");
    }
    //get buffer start
    int bufferLength{ 0 };
    const void *bufferStart{ nullptr };
    m_codeInputStream.GetDirectBufferPointer(&bufferStart, &bufferLength);
    if ((uint32_t)bufferLength < length) {
        std::ostringstream errorInfo;
        errorInfo << "parse vector message no enough length";
        throw ParameterUnpackagingException(errorInfo.str());
    }

    google::protobuf::io::ArrayInputStream subInputStream(bufferStart, length);
    google::protobuf::io::CodedInputStream subCodeInputStream(&subInputStream);
    CodeInputStreamHandler subInputStreamHandler(subCodeInputStream);
    subInputStreamHandler.read(1, param);
    m_codeInputStream.Skip(length);
}

template<typename T, typename F/* = typename std::enable_if<is_list_container<T>::value>::type*/>
inline void CodeInputStreamHandler::readUnordered(int tag, T& param) {
    try {
        typename T::value_type value;
        readInVector(value);
        emplace_back(param, std::move(value));
        while (m_codeInputStream.ExpectTag(tag)) {
            typename T::value_type value;
            readInVector(value);
            emplace_back(param, std::move(value));
        }
    }
    catch (ParameterUnpackagingException& ex) {
        std::ostringstream ostr;
        int paramIndex = WireFormatLite::GetTagFieldNumber(tag);
        ostr << "Parameter index " << paramIndex << ": " << ex.what();
        throw ParameterUnpackagingException(ostr.str());
    }
}

//read map
template<typename K, typename V>
inline void readKeyValue(K& key, V& value, google::protobuf::io::CodedInputStream& codeInputStream) {
    //read key value message length
    uint32_t length;
    if (!codeInputStream.ReadVarint32(&length)) {
        throw ParameterUnpackagingException("failed to read key-value message length in map");
    }
    //get buffer start
    int bufferLength{ 0 };
    const void *bufferStart{ nullptr };
    codeInputStream.GetDirectBufferPointer(&bufferStart, &bufferLength);
    if ((uint32_t)bufferLength < length) {
        std::ostringstream errorInfo;
        errorInfo << "parse key value pair message no enough length";
        throw ParameterUnpackagingException(errorInfo.str());
    }

    //google::protobuf::io::ArrayInputStream subInputStream(bufferStart, length);
    google::protobuf::io::CodedInputStream subCodeInputStream((const uint8_t*)bufferStart, length);
    CodeInputStreamHandler subInputStreamHandler(subCodeInputStream);
    subInputStreamHandler.read(1, key);

	
	if (!subCodeInputStream.ExpectAtEnd()) {
		subInputStreamHandler.read(2, value);
	}

    codeInputStream.Skip(length);
}

template<typename T, typename F/* = typename std::enable_if<is_map<T>::value>::type*/, int /*I = 1*/>
inline void CodeInputStreamHandler::read(int paramIndex, T& param) {
    typedef typename T::key_type KeyType;
    typedef typename T::mapped_type ValueType;

    CHECK_PARAMINDEX_EQ_FIELDNUMBER(paramIndex);
    try {
        KeyType key;
        ValueType value;
        readKeyValue(key, value, m_codeInputStream);
        param.emplace(std::move(key), std::move(value));

        while (m_codeInputStream.ExpectTag(tag)) {
            KeyType key;
            ValueType value;
            readKeyValue(key, value, m_codeInputStream);
            param.emplace(std::move(key), std::move(value));
        }
    }
    catch (ParameterUnpackagingException& ex) {
        std::ostringstream ostr;
        ostr << "Parameter index " << paramIndex << ": " << ex.what();
        throw ParameterUnpackagingException(ostr.str());
    }

}

template<typename T, typename F/* = typename std::enable_if<is_map<T>::value>::type*/, int /*I = 1*/>
inline void CodeInputStreamHandler::readInVector(T& param) {

    //read key value message length
    uint32_t length;
    if (!m_codeInputStream.ReadVarint32(&length)) {
        throw ParameterUnpackagingException("failed to read key-value message length in map");
    }
    //get buffer start
    int bufferLength{ 0 };
    const void *bufferStart{ nullptr };
    m_codeInputStream.GetDirectBufferPointer(&bufferStart, &bufferLength);
    if ((uint32_t)bufferLength < length) {
        std::ostringstream errorInfo;
        errorInfo << "parse vector message no enough length";
        throw ParameterUnpackagingException(errorInfo.str());
    }

    google::protobuf::io::ArrayInputStream subInputStream(bufferStart, length);
    google::protobuf::io::CodedInputStream subCodeInputStream(&subInputStream);
    CodeInputStreamHandler subInputStreamHandler(subCodeInputStream);
    subInputStreamHandler.read(1, param);
    //skip the length
    m_codeInputStream.Skip(length);

}

template<typename T, typename F/* = typename std::enable_if<is_map<T>::value>::type*/, int /*I = 1*/>
inline void CodeInputStreamHandler::readUnordered(int tag, T& param) {
    typedef typename T::key_type KeyType;
    typedef typename T::mapped_type ValueType;

    try {
        KeyType key;
        ValueType value;
        readKeyValue(key, value, m_codeInputStream);
        param.emplace(std::move(key), std::move(value));

        while (m_codeInputStream.ExpectTag(tag)) {
            KeyType key;
            ValueType value;
            readKeyValue(key, value, m_codeInputStream);
            param.emplace(std::move(key), std::move(value));
        }
    }
    catch (ParameterUnpackagingException& ex) {
        std::ostringstream ostr;
        int paramIndex = WireFormatLite::GetTagFieldNumber(tag);
        ostr << "Parameter index " << paramIndex << ": " << ex.what();
        throw ParameterUnpackagingException(ostr.str());
    }

}

//unpackage functions
template<typename T, typename F = typename std::enable_if<!is_list_container<T>::value && !is_map<T>::value>::type>
inline void UnpackageSingleParameter(dep::ByteBuffer& inputByteBuffer, T& param) {
    google::protobuf::io::ArrayInputStream arrayInputStream(inputByteBuffer.getArray() + inputByteBuffer.getPosition(), inputByteBuffer.remaining());
    google::protobuf::io::CodedInputStream codeInputStream(&arrayInputStream);
    CodeInputStreamHandler input(codeInputStream);
    input.read(param);
}

template<typename T, typename F = typename std::enable_if<is_list_container<T>::value || is_map<T>::value>::type, int I = 1>
void UnpackageSingleParameter(dep::ByteBuffer& inputByteBuffer, T& param) {
    google::protobuf::io::ArrayInputStream arrayInputStream(inputByteBuffer.getArray() + inputByteBuffer.getPosition(), inputByteBuffer.remaining());
    google::protobuf::io::CodedInputStream codeInputStream(&arrayInputStream);
    CodeInputStreamHandler input(codeInputStream);
    input.read(1, param);
}

// template<typename T, typename F = typename std::enable_if<is_map<T>::value>::type, bool B = true>
// void UnpackageSingleParameter(dep::ByteBuffer& inputByteBuffer, T& param) {
//     google::protobuf::io::ArrayInputStream arrayInputStream(inputByteBuffer.getArray() + inputByteBuffer.getPosition(), inputByteBuffer.remaining());
//     google::protobuf::io::CodedInputStream codeInputStream(&arrayInputStream);
//     CodeInputStreamHandler input(codeInputStream);
//     input.read(1, param);
// }

template<typename T, typename ... Args>
inline void UnpackageOrderedParameters(dep::ByteBuffer& inputByteBuffer, T&& param, Args&&... args) {
    google::protobuf::io::ArrayInputStream arrayInputStream(inputByteBuffer.getArray() + inputByteBuffer.getPosition(), inputByteBuffer.remaining());
    google::protobuf::io::CodedInputStream codeInputStream(&arrayInputStream);
    CodeInputStreamHandler input(codeInputStream);
    UnpackageOrderedParametersInternal(input, 1, param, std::forward<Args>(args)...);
}

template<typename T>
inline void UnpackageOrderedParametersInternal(CodeInputStreamHandler& input, int paramIndex, T& param) {
    input.read(paramIndex, param);
}

template<typename T, typename ... Args>
inline void UnpackageOrderedParametersInternal(CodeInputStreamHandler& input, int paramIndex, T&& param, Args&&... args) {
    input.read(paramIndex, param);
    UnpackageOrderedParametersInternal(input, ++paramIndex, std::forward<Args>(args)...);
}


}}

#endif
