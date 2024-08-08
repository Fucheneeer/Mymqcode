////////////////////////////////////////////////////////////////////////////////
//
// PackageParameters.h
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
#ifndef PACKAGE_PARAMETERS_H_
#define PACKAGE_PARAMETERS_H_
#include <string>
#include <sstream>
#include <TypeTraits.h>
#include <google/protobuf/wire_format_lite.h>
#include <google/protobuf/io/coded_stream.h>
#include "ByteBuffer.h"
#define NO_METAOBJ
#ifndef NO_METAOBJ
#include "MetaObject.h"
#endif
#include "ParameterPackagingException.h"
#include "ByteBufferOutputStream.h"
#include "MessagePropertyVariant.h"


namespace lightmq{ namespace util{
/*
supported types: bool, char,
map<string, string> =
message KeyValuePair{
    required string Key =1;
    required string Value =2;
};
message Map{
    repeated KeyValuePair KeyValue = paramIndex;
};

map<string, vector<string>> =
message KeyValuePair{
    required string Key =1;
    repeated string Value =2;
};
message Map{
    repeated KeyValuePair KeyValue = paramIndex;
};

vector<vector<string>> =
message StringVector{
    repeated string str = 1;
};
message StringVectorInnfer{
	StringVector vec = 1;
}
message StringVectorVector{
    repeated StringVector strvec = paramIndex;
};

vector<vector<vector<string>>> =
message StringVInner{
    repeated string str = 1;
};
message StringV{
    StringVInner strvec = 1;
};
message StringVVInner{
    repeated StringV strvec = 1;
};
message StringVV{
	StringVVInner strvec = 1;
};
message StringVVV{
    repeated StringVV strvec = paramIndex;
};

vector<map<string, string>> =
message KeyValuePair{
    required string Key =1;
    repeated string Value =2;
};
message Map{
    repeated KeyValuePair KeyValue = 1;
};
message Vector{
    repeated Map Maps;
}

*/

//�������


class CodeOutPutStreamHandler {
public:
    using WireFormatLite = google::protobuf::internal::WireFormatLite;
    using CodedOutputStream = google::protobuf::io::CodedOutputStream;

    CodeOutPutStreamHandler(CodedOutputStream& outputStream);

    template<bool ForceToWrite>	inline void put(int paramIndex, bool param);
	template<bool ForceToWrite>	inline void put(int paramIndex, char param);
	template<bool ForceToWrite>	inline void put(int paramIndex, unsigned char param);
	template<bool ForceToWrite>	inline void put(int paramIndex, int16_t param);
	template<bool ForceToWrite>	inline void put(int paramIndex, uint16_t param);
	template<bool ForceToWrite>	inline void put(int paramIndex, int32_t param);
	template<bool ForceToWrite>	inline void put(int paramIndex, uint32_t param);
	template<bool ForceToWrite>	inline void put(int paramIndex, int64_t param);
	template<bool ForceToWrite>	inline void put(int paramIndex, uint64_t param);
	template<bool ForceToWrite>	inline void put(int paramIndex, float param);
	template<bool ForceToWrite>	inline void put(int paramIndex, double param);
	template<bool ForceToWrite>	inline void put(int paramIndex, const dep::ByteBuffer& param);
	template<bool ForceToWrite> inline void put(int paramIndex, const char* param);

	template<bool ForceToWrite>	inline void put(int paramIndex, const std::string& param);

    template<bool ForceToWrite>	inline void put(int paramIndex, const MessagePropertyVariant& variant);

	//template<>	inline void put<true>(int paramIndex, const std::string& param);
	//template<>	inline void put<false>(int paramIndex, const std::string& param);

#ifndef NO_METAOBJ
    inline void put(int paramIndex, dep::MetaObject& param);

    template<typename T, bool IsMetaobject = std::is_base_of<dep::MetaObject, T>::value>
    inline void put(int paramIndex, std::shared_ptr<T>& param);

    template<typename T, bool IsMetaobject = std::is_base_of<dep::MetaObject, T>::value>
    inline void put(int paramIndex, std::unique_ptr<T>& param);

    template<typename T, bool IsMetaobject = std::is_base_of<dep::MetaObject, T>::value>
    inline void put(int paramIndex, T* param);
#endif
    template<bool ForceToWrite, typename T, typename F = typename std::enable_if<is_list_container<T>::value>::type>
    inline void put(int paramIndex, const T& param);

    template<bool ForceToWrite, typename T, typename F = typename std::enable_if<is_map<T>::value>::type, int I = 1>
    inline void put(int paramIndex, const T & param);

    //put in vector
    inline void putInVector(int paramIndex, bool param) {
        put<true>(paramIndex, param);
    }
    inline void putInVector(int paramIndex, char param) {
        put<true>(paramIndex, param);
    }
    inline void putInVector(int paramIndex, unsigned char param) {
        put<true>(paramIndex, param);
    }
    inline void putInVector(int paramIndex, int16_t param) {
        put<true>(paramIndex, param);
    }
    inline void putInVector(int paramIndex, uint16_t param) {
        put<true>(paramIndex, param);
    }
    inline void putInVector(int paramIndex, int32_t param) {
        put<true>(paramIndex, param);
    }
    inline void putInVector(int paramIndex, uint32_t param) {
        put<true>(paramIndex, param);
    }
    inline void putInVector(int paramIndex, int64_t param) {
        put<true>(paramIndex, param);
    }
    inline void putInVector(int paramIndex, uint64_t param) {
        put<true>(paramIndex, param);
    }
    inline void putInVector(int paramIndex, float param) {
        put<true>(paramIndex, param);
    }
    inline void putInVector(int paramIndex, double param) {
        put<true>(paramIndex, param);
    }
    inline void putInVector(int paramIndex, const std::string& param);

	inline void putInVector(int paramIndex, const dep::ByteBuffer& param) {
        put<true>(paramIndex, param);
    }
    inline void putInVector(int paramIndex, const char* param) {
        put<true>(paramIndex, param);
    }

#ifndef NO_METAOBJ
    inline void putInVector(int paramIndex, dep::MetaObject& param) {
        put(paramIndex, param);
    }

    template<typename T, bool IsMetaobject = std::is_base_of<dep::MetaObject, T>::value>
    inline void putInVector(int paramIndex, std::shared_ptr<T>& param) {
        put(paramIndex, param);
    }

    template<typename T, bool IsMetaobject = std::is_base_of<dep::MetaObject, T>::value>
    inline void putInVector(int paramIndex, std::unique_ptr<T>& param) {
        put(paramIndex, param);
    }

    template<typename T, bool IsMetaobject = std::is_base_of<dep::MetaObject, T>::value>
    inline void putInVector(int paramIndex, T* param) {
        put(paramIndex, param);
    }
#endif

    template<typename T, typename F = typename std::enable_if<is_list_container<T>::value>::type>
    inline  void putInVector(int paramIndex, const T& param);

    template<typename T, typename F = typename std::enable_if<is_map<T>::value>::type, int I = 1>
    inline void putInVector(int paramIndex, const T& param);
    
    //put no tag
    inline void putNoTag(bool param);
    inline void putNoTag(char param);
    inline void putNoTag(unsigned char param);
    inline void putNoTag(int8_t param);
    inline void putNoTag(int16_t param);
    inline void putNoTag(uint16_t param);
    inline void putNoTag(int32_t param);
    inline void putNoTag(uint32_t param);
    inline void putNoTag(int64_t param);
    inline void putNoTag(uint64_t param);
    inline void putNoTag(float param);
    inline void putNoTag(double param);
    inline void putNoTag(const std::string& param);
    inline void putNoTag(const dep::ByteBuffer& param);
    inline void putNoTag(const char* param);
    inline void putNoTag(const MessagePropertyVariant& param);

#ifndef NO_METAOBJ
    inline void putNoTag(dep::MetaObject& param);

    template<typename T, bool IsMetaobject = std::is_base_of<dep::MetaObject, T>::value>
    inline void putNoTag(std::shared_ptr<T>& param);

    template<typename T, bool IsMetaobject = std::is_base_of<dep::MetaObject, T>::value>
    inline void putNoTag(std::unique_ptr<T>& param);

    template<typename T, bool IsMetaobject = std::is_base_of<dep::MetaObject, T>::value>
    inline void putNoTag(T* param);
#endif

    template<typename T>
    void putNoTag(const T& param);


private:
    CodedOutputStream& m_codeOutputStream;
};

inline CodeOutPutStreamHandler::CodeOutPutStreamHandler(CodedOutputStream& outputStream) :
    m_codeOutputStream(outputStream){
}

template<bool ForceToWrite>
inline void CodeOutPutStreamHandler::put(int paramIndex, bool param) {
    WireFormatLite::WriteBool(paramIndex, param, &m_codeOutputStream);
}
template<>
inline void CodeOutPutStreamHandler::put<false>(int paramIndex, bool param) {
    if (param) {
        WireFormatLite::WriteBool(paramIndex, param, &m_codeOutputStream);
    }
}

template<bool ForceToWrite>
inline void CodeOutPutStreamHandler::put(int paramIndex, char param) {
    WireFormatLite::WriteInt32(paramIndex, static_cast<int32_t>(param), &m_codeOutputStream);
}
template<>
inline void CodeOutPutStreamHandler::put<false>(int paramIndex, char param) {
    if (param) {
        WireFormatLite::WriteInt32(paramIndex, static_cast<int32_t>(param), &m_codeOutputStream);
    }
}

template<bool ForceToWrite>
inline void CodeOutPutStreamHandler::put(int paramIndex, unsigned char param) {
    WireFormatLite::WriteInt32(paramIndex, static_cast<int32_t>(param), &m_codeOutputStream);
}
template<>
inline void CodeOutPutStreamHandler::put<false>(int paramIndex, unsigned char param) {
    if (param) {
        WireFormatLite::WriteInt32(paramIndex, static_cast<int32_t>(param), &m_codeOutputStream);
    }
}

template<bool ForceToWrite>
inline void CodeOutPutStreamHandler::put(int paramIndex, int16_t param) {
    WireFormatLite::WriteInt32(paramIndex, static_cast<int32_t>(param), &m_codeOutputStream);
}
template<>
inline void CodeOutPutStreamHandler::put<false>(int paramIndex, int16_t param) {
    if (param) {
        WireFormatLite::WriteInt32(paramIndex, static_cast<int32_t>(param), &m_codeOutputStream);
    }
}

template<bool ForceToWrite>
inline void CodeOutPutStreamHandler::put(int paramIndex, uint16_t param) {
    WireFormatLite::WriteUInt32(paramIndex, static_cast<uint32_t>(param), &m_codeOutputStream);
}
template<>
inline void CodeOutPutStreamHandler::put<false>(int paramIndex, uint16_t param) {
    if (param) {
        WireFormatLite::WriteUInt32(paramIndex, static_cast<uint32_t>(param), &m_codeOutputStream);
    }
}

template<bool ForceToWrite>
inline void CodeOutPutStreamHandler::put(int paramIndex, int32_t param) {
    WireFormatLite::WriteInt32(paramIndex, param, &m_codeOutputStream);
}
template<>
inline void CodeOutPutStreamHandler::put<false>(int paramIndex, int32_t param) {
    if (param) {
        WireFormatLite::WriteInt32(paramIndex, param, &m_codeOutputStream);
    }
}

template<bool ForceToWrite>
inline void CodeOutPutStreamHandler::put(int paramIndex, uint32_t param) {
    WireFormatLite::WriteUInt32(paramIndex, param, &m_codeOutputStream);
}
template<>
inline void CodeOutPutStreamHandler::put<false>(int paramIndex, uint32_t param) {
    if (param) {
        WireFormatLite::WriteUInt32(paramIndex, param, &m_codeOutputStream);
    }
}

template<bool ForceToWrite>
inline void CodeOutPutStreamHandler::put(int paramIndex, int64_t param) {
    WireFormatLite::WriteInt64(paramIndex, param, &m_codeOutputStream);
}
template<>
inline void CodeOutPutStreamHandler::put<false>(int paramIndex, int64_t param) {
    if (param) {
        WireFormatLite::WriteInt64(paramIndex, param, &m_codeOutputStream);
    }
}

template<bool ForceToWrite>
inline void CodeOutPutStreamHandler::put(int paramIndex, uint64_t param) {
    WireFormatLite::WriteUInt64(paramIndex, param, &m_codeOutputStream);
}
template<>
inline void CodeOutPutStreamHandler::put<false>(int paramIndex, uint64_t param) {
    if (param) {
        WireFormatLite::WriteUInt64(paramIndex, param, &m_codeOutputStream);
    }
}

template<bool ForceToWrite>
inline void CodeOutPutStreamHandler::put(int paramIndex, float param) {
    WireFormatLite::WriteFloat(paramIndex, param, &m_codeOutputStream);
}
template<>
inline void CodeOutPutStreamHandler::put<false>(int paramIndex, float param) {
    if (0 != param) {
        WireFormatLite::WriteFloat(paramIndex, param, &m_codeOutputStream);
    }
}

template<bool ForceToWrite>
inline void CodeOutPutStreamHandler::put(int paramIndex, double param) {
        WireFormatLite::WriteDouble(paramIndex, param, &m_codeOutputStream);
}

template<>
inline void CodeOutPutStreamHandler::put<false>(int paramIndex, double param) {
    if (0 != param) {
        WireFormatLite::WriteDouble(paramIndex, param, &m_codeOutputStream);
    }
}


template<bool ForceToWrite>
void CodeOutPutStreamHandler::put(int paramIndex, const MessagePropertyVariant& variant) {
    WireFormatLite::WriteTag(paramIndex, WireFormatLite::WIRETYPE_LENGTH_DELIMITED, &m_codeOutputStream);
    putNoTag(variant);
}




// template<bool ForceToWrite>
// inline void CodeOutPutStreamHandler::put(int paramIndex, const std::string& param) {
//     WireFormatLite::WriteString(paramIndex, param, &m_codeOutputStream);
// }

template<>
inline void CodeOutPutStreamHandler::put<false>(int paramIndex, const std::string& param) {
    if (param.empty()) {
        return;
    }
	WireFormatLite::WriteString(paramIndex, param, &m_codeOutputStream);
}

template<>
inline void CodeOutPutStreamHandler::put<true>(int paramIndex, const std::string& param) {
    WireFormatLite::WriteString(paramIndex, param, &m_codeOutputStream);
}


inline void CodeOutPutStreamHandler::putInVector(int paramIndex, const std::string& param) {
	put<true>(paramIndex, param);
}

template<bool ForceToWrite>
inline void CodeOutPutStreamHandler::put(int paramIndex, const dep::ByteBuffer& param) {
	WireFormatLite::WriteTag(paramIndex, WireFormatLite::WIRETYPE_LENGTH_DELIMITED, &m_codeOutputStream);
	int length = param.getLimit();
	m_codeOutputStream.WriteVarint32(length);
	m_codeOutputStream.WriteRaw(param.getArray(), length);
}

template<bool ForceToWrite>
inline void CodeOutPutStreamHandler::put(int paramIndex, const char* param) {
    WireFormatLite::WriteTag(paramIndex, WireFormatLite::WIRETYPE_LENGTH_DELIMITED, &m_codeOutputStream);
    int length = strlen(param);
    m_codeOutputStream.WriteVarint32(length);
    m_codeOutputStream.WriteRaw(param, length);
}

#ifndef NO_METAOBJ
inline void CodeOutPutStreamHandler::put(int paramIndex, dep::MetaObject& param) {
    std::string serializeString;
    param.serialize(serializeString);
    WireFormatLite::WriteString(paramIndex, serializeString, &m_codeOutputStream);
}

template<typename T, bool IsMetaobject>
inline void CodeOutPutStreamHandler::put(int paramIndex, std::shared_ptr<T>& param) {
    if (!param) {
        std::ostringstream ostr;
        ostr << "Parameter index " << paramIndex << ": failed to serialize with null metaobject shared pointer";
        throw ParameterPackagingException(ostr.str());
    }
    put(paramIndex, *param);
}

template<typename T, bool IsMetaobject>
inline void CodeOutPutStreamHandler::put(int paramIndex, std::unique_ptr<T>& param) {
    if (!param) {
        std::ostringstream ostr;
        ostr << "Parameter index " << paramIndex << ": failed to serialize with null metaobject unique pointer" ;
        throw ParameterPackagingException(ostr.str());
    }
    put(paramIndex, *param);
}

template<typename T, bool IsMetaobject>
inline void CodeOutPutStreamHandler::put(int paramIndex, T* param) {
    if (!param) {
        std::ostringstream ostr;
        ostr << "Parameter index " << paramIndex << ": failed to serialize with null metaobject pointer";
        throw ParameterPackagingException(ostr.str());
    }
    put(paramIndex, *param);
}
#endif

template<bool ForceToWrite, typename T, typename F/* = typename std::enable_if<is_list_container<T>::value>::type*/>
inline void CodeOutPutStreamHandler::put(int paramIndex, const T& param) {
    for (auto& vecElem : param) {
        putInVector(paramIndex, vecElem);
    }
}

template<typename T, typename F/* = typename std::enable_if<is_list_container<T>::value>::type*/>
inline void CodeOutPutStreamHandler::putInVector(int paramIndex, const T& param) {
    WireFormatLite::WriteTag(paramIndex, WireFormatLite::WireTypeForFieldType(WireFormatLite::TYPE_MESSAGE), &m_codeOutputStream);
    //write vector as a message
    dep::ByteBuffer subByteBuffer(512);
    ByteBufferOutputStream byteBufferOutputStream(&subByteBuffer);
    CodedOutputStream codeOutputStream(&byteBufferOutputStream);
    CodeOutPutStreamHandler outHander(codeOutputStream);
    codeOutputStream.Trim();

    outHander.put<true>(1, param);

    codeOutputStream.Trim();
    subByteBuffer.flip();

    m_codeOutputStream.WriteVarint32(subByteBuffer.getLimit());
    m_codeOutputStream.WriteRaw(subByteBuffer.getArray(), subByteBuffer.getLimit());
}

template<bool ForceToWrite, typename T, typename F/* = typename std::enable_if<is_map<T>::value>::type*/, int I/* = 1*/>
inline void CodeOutPutStreamHandler::put(int paramIndex, const T& param) {
    dep::ByteBuffer subByteBuffer(512);
    for (auto& kv : param) {
        WireFormatLite::WriteTag(paramIndex, WireFormatLite::WireTypeForFieldType(WireFormatLite::TYPE_MESSAGE), &m_codeOutputStream);
        subByteBuffer.clear();
        ByteBufferOutputStream byteBufferOutputStream(&subByteBuffer);
        CodedOutputStream codeOutputStream(&byteBufferOutputStream);
        CodeOutPutStreamHandler outHander(codeOutputStream);

        outHander.put<true>(1, kv.first);
        outHander.put<true>(2, kv.second);

        codeOutputStream.Trim();
        subByteBuffer.flip();

        m_codeOutputStream.WriteVarint32(subByteBuffer.getLimit());
        m_codeOutputStream.WriteRaw(subByteBuffer.getArray(), subByteBuffer.getLimit());
    }
}


template<typename T, typename F/* = typename std::enable_if<is_map<T>::value>::type*/, int I/* = 1*/>
inline void CodeOutPutStreamHandler::putInVector(int paramIndex, const T& param) {
    WireFormatLite::WriteTag(paramIndex, WireFormatLite::WireTypeForFieldType(WireFormatLite::TYPE_MESSAGE), &m_codeOutputStream);
    //write vector as a message
    dep::ByteBuffer subByteBuffer(512);
    ByteBufferOutputStream byteBufferOutputStream(&subByteBuffer);
    CodedOutputStream codeOutputStream(&byteBufferOutputStream);
    CodeOutPutStreamHandler outHander(codeOutputStream);

    outHander.put<true>(1, param);

    codeOutputStream.Trim();
    subByteBuffer.flip();

    m_codeOutputStream.WriteVarint32(subByteBuffer.getLimit());
    m_codeOutputStream.WriteRaw(subByteBuffer.getArray(), subByteBuffer.getLimit());
}


//write no tag
inline void CodeOutPutStreamHandler::putNoTag(bool param) {
    WireFormatLite::WriteBoolNoTag(param, &m_codeOutputStream);
}
inline void CodeOutPutStreamHandler::putNoTag(char param) {
    WireFormatLite::WriteInt32NoTag(static_cast<int32_t>(param), &m_codeOutputStream);
}
inline void CodeOutPutStreamHandler::putNoTag(int8_t param) {
    WireFormatLite::WriteInt32NoTag(static_cast<int32_t>(param), &m_codeOutputStream);
}
inline void CodeOutPutStreamHandler::putNoTag(unsigned char param) {
    WireFormatLite::WriteUInt32NoTag(static_cast<uint32_t>(param), &m_codeOutputStream);
}
inline void CodeOutPutStreamHandler::putNoTag(int16_t param) {
    WireFormatLite::WriteInt32NoTag(static_cast<int32_t>(param), &m_codeOutputStream);
}
inline void CodeOutPutStreamHandler::putNoTag(uint16_t param) {
    WireFormatLite::WriteUInt32NoTag(static_cast<uint32_t>(param), &m_codeOutputStream);
}
inline void CodeOutPutStreamHandler::putNoTag(int32_t param) {
    WireFormatLite::WriteInt32NoTag(param, &m_codeOutputStream);
}
inline void CodeOutPutStreamHandler::putNoTag(uint32_t param) {
    WireFormatLite::WriteUInt32NoTag(param, &m_codeOutputStream);
}
inline void CodeOutPutStreamHandler::putNoTag(int64_t param) {
    WireFormatLite::WriteInt64NoTag(param, &m_codeOutputStream);
}
inline void CodeOutPutStreamHandler::putNoTag(uint64_t param) {
    WireFormatLite::WriteUInt64NoTag(param, &m_codeOutputStream);
}
inline void CodeOutPutStreamHandler::putNoTag(float param) {
    WireFormatLite::WriteFloatNoTag(param, &m_codeOutputStream);
}
inline void CodeOutPutStreamHandler::putNoTag(double param) {
    WireFormatLite::WriteDoubleNoTag(param, &m_codeOutputStream);
}
inline void CodeOutPutStreamHandler::putNoTag(const std::string& param) {
    int length = param.size();
    m_codeOutputStream.WriteVarint32(length);
    m_codeOutputStream.WriteRaw(param.data(), length);
}
inline void CodeOutPutStreamHandler::putNoTag(const dep::ByteBuffer& param) {
    int length = param.getLimit();
    m_codeOutputStream.WriteVarint32(length);
    m_codeOutputStream.WriteRaw(param.getArray(), length);
}
inline void CodeOutPutStreamHandler::putNoTag(const char* param) {
    int length = strlen(param);
    m_codeOutputStream.WriteVarint32(length);
    m_codeOutputStream.WriteRaw(param, length);
}

#ifndef NO_METAOBJ
inline void CodeOutPutStreamHandler::putNoTag(dep::MetaObject& param) {
    std::string serializeString;
    param.serialize(serializeString);
    putNoTag(serializeString);
}

template<typename T, bool IsMetaobject>
inline void CodeOutPutStreamHandler::putNoTag(std::shared_ptr<T>& param) {
    if (!param) {
        std::ostringstream ostr;
        ostr << "failed to serialize with null metaobject shared pointer";
        throw ParameterPackagingException(ostr.str());
    }
    putNoTag(*param);
}

template<typename T, bool IsMetaobject>
inline void CodeOutPutStreamHandler::putNoTag(std::unique_ptr<T>& param) {
    if (!param) {
        std::ostringstream ostr;
        ostr << "failed to serialize with null metaobject unique pointer";
        throw ParameterPackagingException(ostr.str());
    }
    putNoTag(*param);
}

template<typename T, bool IsMetaobject>
inline void CodeOutPutStreamHandler::putNoTag(T* param) {
    if (!param) {
        std::ostringstream ostr;
        ostr << "failed to serialize with null metaobject pointer";
        throw ParameterPackagingException(ostr.str());
    }
    putNoTag(*param);
}
#endif

template<typename T>
inline void CodeOutPutStreamHandler::putNoTag(const T& param) {
    put<true>(1, param);
}

void CodeOutPutStreamHandler::putNoTag(const MessagePropertyVariant& variant){
    thread_local static dep::ByteBuffer subByteBufferForVariant(64);
    subByteBufferForVariant.clear();

    ByteBufferOutputStream byteBufferOutputStream(&subByteBufferForVariant);
    CodedOutputStream codeOutputStream(&byteBufferOutputStream);
    CodeOutPutStreamHandler outHander(codeOutputStream);

    switch (variant.getType())
    {
    case MessagePropertyVariant::VT_BOOL:
        outHander.put<true>(variant.getType(), variant.getBooleanValue());
        break;
    case MessagePropertyVariant::VT_BYTE:
        outHander.put<true>(variant.getType(), variant.getByteValue());
        break;
    case MessagePropertyVariant::VT_SHORT:
        outHander.put<true>(variant.getType(), variant.getShortValue());
        break;
    case MessagePropertyVariant::VT_INT:
        outHander.put<true>(variant.getType(), variant.getIntegerValue());
        break;
    case MessagePropertyVariant::VT_LONG:
        outHander.put<true>(variant.getType(), variant.getLongValue());
        break;
    case MessagePropertyVariant::VT_FLOAT:
        outHander.put<true>(variant.getType(), variant.getFloatValue());
        break;
    case MessagePropertyVariant::VT_DOUBLE:
        outHander.put<true>(variant.getType(), variant.getDoubleValue());
        break;
    case MessagePropertyVariant::VT_UNICODE_CHAR:
        outHander.put<true>(variant.getType(), variant.getUnicodeCharValue());
        break;
    case MessagePropertyVariant::VT_STRING:
    case MessagePropertyVariant::VT_BYTES:
        outHander.put<true>(variant.getType(), variant.getStringValue());
        break;
    default:
        break;
    }

    codeOutputStream.Trim();
    subByteBufferForVariant.flip();

    m_codeOutputStream.WriteVarint32(subByteBufferForVariant.getLimit());
    m_codeOutputStream.WriteRaw(subByteBufferForVariant.getArray(), subByteBufferForVariant.getLimit());
}

//������������
template<typename T>
inline void PackageSingleParameter(dep::ByteBuffer& byteBuffer, T&& param) {
    ByteBufferOutputStream byteBufferOutputStream(&byteBuffer);
    google::protobuf::io::CodedOutputStream codedOutputStream(&byteBufferOutputStream);
    CodeOutPutStreamHandler outPut(codedOutputStream);
    outPut.putNoTag(std::forward<T>(param));
    codedOutputStream.Trim();
}

//�Բ���λ�ú���Ϊfield number�������
template<typename T, typename ... Args>
inline void PackageParameters(dep::ByteBuffer& byteBuffer, T&& param, Args&&... args) {
    ByteBufferOutputStream byteBufferOutputStream(&byteBuffer);
    google::protobuf::io::CodedOutputStream codedOutputStream(&byteBufferOutputStream);
    CodeOutPutStreamHandler outPut(codedOutputStream);
    PackageParametersInternal(outPut, 1, param, std::forward<Args>(args)...);
    codedOutputStream.Trim();
}

template<typename T>
inline void PackageParametersInternal(CodeOutPutStreamHandler& outPut, int paramIndex, T& param) {
    outPut.put<true>(paramIndex, param);
}

template<typename T, typename ... Args>
inline void PackageParametersInternal(CodeOutPutStreamHandler& outPut, int paramIndex, T&& param, Args&&... args) {
    outPut.put<true>(paramIndex, param);
    PackageParametersInternal(outPut, ++paramIndex, std::forward<Args>(args)...);
}

//�������Ŵ������
template<typename T, typename ... Args>
inline void PackageParametersWithId(dep::ByteBuffer& byteBuffer, int paramId, T&& param, Args&&... args) {
    ByteBufferOutputStream byteBufferOutputStream(&byteBuffer);
    google::protobuf::io::CodedOutputStream codedOutputStream(&byteBufferOutputStream);
    CodeOutPutStreamHandler outPut(codedOutputStream);
    PackageParametersWithIdInternal(outPut, paramId, param, std::forward<Args>(args)...);
    codedOutputStream.Trim();
}

template<typename T>
inline void PackageParametersWithIdInternal(CodeOutPutStreamHandler& outPut, int paramIndex, T& param) {
    outPut.put<false>(paramIndex, param);
}

template<typename T, typename ... Args>
inline void PackageParametersWithIdInternal(CodeOutPutStreamHandler& outPut, int paramId, T&& param, Args&&... args) {
    outPut.put<false>(paramId, param);
    PackageParametersWithIdInternal(outPut, std::forward<Args>(args)...);
}

}}

#endif
