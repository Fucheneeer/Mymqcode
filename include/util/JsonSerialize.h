////////////////////////////////////////////////////////////////////////////////
//
// JsonSerialize.h
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
#ifndef JSON_WRITER_H_
#define JSON_WRITER_H_

#include <string>
#include <vector>
#include <set>
#include <map>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <rapidjson/rapidjson.h>
#include <rapidjson/reader.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/optimized_writer.h>
#include <rapidjson/prettywriter.h>
#include "JSONObject.h"


namespace lightmq { namespace util {

    static constexpr auto JSON_KEY_USERID = "userID";
    static constexpr auto JSON_KEY_ENCRYPTED = "encrypted";
    static constexpr auto JSON_KEY_USERPASSWORD = "userPassword";

    static constexpr auto JSON_KEY_GROUP_NAME = "name";
    static constexpr auto JSON_KEY_MEMBERS = "members";

    static constexpr auto JSON_KEY_CHANNEL = "channel";
    static constexpr auto JSON_KEY_READGROUPS = "readGroups";
    static constexpr auto JSON_KEY_WRITEROUPS = "writeGroups";

	static constexpr auto JSON_KEY_QUERYABLE = "queryAble";
	static constexpr auto JSON_KEY_MODIFYABLE = "modifyAble";

    struct AccountInfo
    {
        std::string userID;
        bool encrypted;
        std::string password;
    };

	struct MonitorAccountInfo
	{
		std::string userID;
		bool encrypted;
		std::string password;
		bool queryAble;
		bool modifyAble;
	};

    struct GroupInfo {
        std::string name;
        std::vector<std::string> members;
    };

    struct AuthorizationInfo {
        std::string channel;
        std::vector<std::string> readGroups;
        std::vector<std::string> writeGroups;

    };
	struct MonitorAuthorizationInfo {
		std::vector<std::string> QueryGroups;
		std::vector<std::string> ModifyGroups;
	};

//json writer declare
template<typename Writer>
inline void PutJsonValue(Writer& writer, int value);

template<typename Writer>
inline void PutJsonValue(Writer& writer, uint32_t value);

template<typename Writer>
inline void PutJsonValue(Writer& writer, int64_t value);

template<typename Writer>
inline void PutJsonValue(Writer& writer, uint64_t value);

template<typename Writer>
inline void PutJsonValue(Writer& writer, double value);

template<typename Writer>
inline void PutJsonValue(Writer& writer, float value);

template<typename Writer>
inline void PutJsonValue(Writer& writer, const char* value);

template<typename Writer>
inline void PutJsonValue(Writer& writer, const std::string& value);

template<typename Writer>
inline void PutJsonValue(Writer& writer, bool value);

template<typename Writer, typename T>
inline void PutJsonValue(Writer& writer, const std::vector<T>& value);

template<typename Writer, typename T>
inline void PutJsonValue(Writer& writer, const std::set<T>& value);

template<typename Writer, typename T>
inline void PutJsonValue(Writer& writer, const std::unordered_set<T>& value);

template<typename Writer, typename T>
inline void PutJsonValue(Writer& writer, const std::stack<T>& value);

template<typename Writer, typename K, typename V>
inline void PutJsonValue(Writer& writer, const std::map<K, V>& value);

template<typename Writer, typename K, typename V>
inline void PutJsonValue(Writer& writer, const std::unordered_map<K, V>& value);

//json writer definition
template<typename Writer>
inline void PutJsonValue(Writer& writer, int value) {
	writer.AddInt(value);
}

template<typename Writer>
inline void PutJsonValue(Writer& writer, uint32_t value) {
	writer.AddUint(value);
}

template<typename Writer>
inline void PutJsonValue(Writer& writer, int64_t value) {
	writer.AddInt64(value);
}

template<typename Writer>
inline void PutJsonValue(Writer& writer, uint64_t value) {
	writer.AddUint64(value);
}

template<typename Writer>
inline void PutJsonValue(Writer& writer, double value) {
	writer.Double(value);
}

template<typename Writer>
inline void PutJsonValue(Writer& writer, float value) {
	writer.Double(value);
}

template<typename Writer>
inline void PutJsonValue(Writer& writer, const char* value) {
	writer.String(value, strlen(value));
}

template<typename Writer>
inline void PutJsonValue(Writer& writer, const std::string& value) {
	PutJsonValue(writer, value.c_str());
}

template<typename Writer>
inline void PutJsonValue(Writer& writer, const AccountInfo& value) {
    writer.StartObject();
    writer.Key(JSON_KEY_USERID);
    PutJsonValue(writer, value.userID);
    writer.Key(JSON_KEY_ENCRYPTED);
    PutJsonValue(writer, value.encrypted);
    writer.Key(JSON_KEY_USERPASSWORD);
    PutJsonValue(writer, value.password);
    writer.EndObject();
}

template<typename Writer>
inline void PutJsonValue(Writer& writer, const MonitorAccountInfo& value) {
	writer.StartObject();
	writer.Key(JSON_KEY_USERID);
	PutJsonValue(writer, value.userID);
	writer.Key(JSON_KEY_ENCRYPTED);
	PutJsonValue(writer, value.encrypted);
	writer.Key(JSON_KEY_USERPASSWORD);
	PutJsonValue(writer, value.password);
	writer.Key(JSON_KEY_QUERYABLE);
	PutJsonValue(writer, value.queryAble);
	writer.Key(JSON_KEY_MODIFYABLE);
	PutJsonValue(writer, value.modifyAble);
	writer.EndObject();
}

template<typename Writer>
inline void PutJsonValue(Writer& writer, const GroupInfo& value) {
    writer.StartObject();
    writer.Key(JSON_KEY_GROUP_NAME);
    PutJsonValue(writer, value.name);
    writer.Key(JSON_KEY_MEMBERS);
    PutJsonValue(writer, value.members);
    writer.EndObject();
}

template<typename Writer>
inline void PutJsonValue(Writer& writer, const AuthorizationInfo& value) {
    writer.StartObject();
    writer.Key(JSON_KEY_CHANNEL);
    PutJsonValue(writer, value.channel);
    writer.Key(JSON_KEY_READGROUPS);
    PutJsonValue(writer, value.readGroups);
    writer.Key(JSON_KEY_WRITEROUPS);
    PutJsonValue(writer, value.writeGroups);
    writer.EndObject();
}

template<typename Writer>
inline void PutJsonValue(Writer& writer, bool value) {
	writer.Bool(value);
}

template<typename Writer, typename T>
inline void PutJsonValue(Writer& writer, const std::vector<T>& value) {
	writer.StartArray();
	for (const auto& e : value) {
		PutJsonValue(writer, e);
	}
	writer.EndArray();
}

template<typename Writer, typename T>
inline void PutJsonValue(Writer& writer, const std::set<T>& value) {
	writer.StartArray();
	for (const auto& e : value) {
		PutJsonValue(writer, e);
	}
	writer.EndArray();
}

template<typename Writer, typename T>
inline void PutJsonValue(Writer& writer, const std::unordered_set<T>& value) {
	writer.StartArray();
	for (const auto& e : value) {
		PutJsonValue(writer, e);
	}
	writer.EndArray();
}

template<typename Writer, typename T>
inline void PutJsonValue(Writer& writer, const std::stack<T>& value) {
	writer.StartArray();
	for (const auto& e : value) {
		PutJsonValue(writer, e);
	}
	writer.EndArray();
}

template<typename Writer, typename K, typename V>
inline void PutJsonValue(Writer& writer, const std::map<K, V>& value) {
	writer.StartArray();
	for (const auto& kv : value) {
		writer.StartObject();
		writer.Key("key");
		PutJsonValue(writer, kv.first);
		writer.Key("value");
		PutJsonValue(writer, kv.second);
		writer.EndObject();
	}
	writer.EndArray();
}

template<typename Writer, typename K, typename V>
inline void PutJsonValue(Writer& writer, const std::unordered_map<K, V>& value) {
	writer.StartArray();
	for (const auto& kv : value) {
		writer.StartObject();
		writer.Key("key");
		PutJsonValue(writer, kv.first);
		writer.Key("value");
		PutJsonValue(writer, kv.second);
		writer.EndObject();
	}
	writer.EndArray();
}

template<typename Writer, typename T>
inline void WriteToJsonInternal(Writer& writer, const std::string key, T& param) {
	writer.Key(key.c_str());
	PutJsonValue(writer, param);
}


template<typename Writer, typename T, typename ... Args>
inline void WriteToJsonInternal(Writer& writer, const std::string key, T&& param, Args&&... args) {
	writer.Key(key.c_str());
	PutJsonValue(writer, param);
	WriteToJsonInternal(writer, std::forward<Args>(args)...);
}

template<typename T, typename ... Args>
inline std::string WriteToJson(bool isprettyJson, const std::string key, T&& param, Args&&... args) {
	using RAPIDJSON_NAMESPACE::StringBuffer;
	using RAPIDJSON_NAMESPACE::OptimizedWriter;
	using RAPIDJSON_NAMESPACE::PrettyWriter;
	using RAPIDJSON_NAMESPACE::Writer;

	typedef PrettyWriter<StringBuffer> PrettyJsonWriter;
	typedef OptimizedWriter<StringBuffer> OptimizedJsonWriter;


	StringBuffer stringBuffer;

	if (isprettyJson) {
		PrettyJsonWriter writer(stringBuffer);
		writer.StartObject();
		WriteToJsonInternal(writer, key, param, std::forward<Args>(args)...);
		writer.EndObject();
	}
	else {
		OptimizedJsonWriter writer(stringBuffer);
		writer.StartObject();
		WriteToJsonInternal(writer, key, param, std::forward<Args>(args)...);
		writer.EndObject();
	}

	return std::string(stringBuffer.GetString(), stringBuffer.GetSize());
}

//read json value declare
inline void ReadJsonValue(const dep::JSONObject* jsObj, const std::string& key, std::string& param);

inline void ReadJsonValue(const dep::JSONObject* jsObj, const std::string& key, int& param);

inline void ReadJsonValue(const dep::JSONObject* jsObj, const std::string& key, uint32_t& param);

inline void ReadJsonValue(const dep::JSONObject* jsObj, const std::string& key, int64_t& param);

inline void ReadJsonValue(const dep::JSONObject* jsObj, const std::string& key, uint64_t& param);

inline void ReadJsonValue(const dep::JSONObject* jsObj, const std::string& key, double& param);

inline void ReadJsonValue(const dep::JSONObject* jsObj, const std::string& key, bool& param);

template<typename T>
inline void ReadJsonValue(const dep::JSONObject* jsObj, const std::string& key, std::vector<T>& param);

template<typename T>
inline void ReadJsonValue(const dep::JSONObject* jsObj, const std::string& key, std::stack<T>& param);

template<typename T>
inline void ReadJsonValue(const dep::JSONObject* jsObj, const std::string& key, std::set<T>& param);

template<typename T>
inline void ReadJsonValue(const dep::JSONObject* jsObj, const std::string& key, std::unordered_set<T>& param);

template<typename K, typename V>
inline void ReadJsonValue(const dep::JSONObject* jsObj, const std::string& key, std::map<K, V>& param);

template<typename K, typename V>
inline void ReadJsonValue(const dep::JSONObject* jsObj, const std::string& key, std::unordered_map<K, V>& param);

template<typename K, typename V>
inline void GetMapEntryItem(const dep::JSONObject* jsObj, K& key, V& value);

inline void GetJsonArrayItem(const dep::JSONArray* jsArray, uint32_t index, std::string& value);

inline void GetJsonArrayItem(const dep::JSONArray* jsArray, uint32_t index, int& value);

inline void GetJsonArrayItem(const dep::JSONArray* jsArray, uint32_t index, uint32_t& value);

inline void GetJsonArrayItem(const dep::JSONArray* jsArray, uint32_t index, int64_t& value);

inline void GetJsonArrayItem(const dep::JSONArray* jsArray, uint32_t index, uint64_t& value);

inline void GetJsonArrayItem(const dep::JSONArray* jsArray, uint32_t index, double& value);

inline void GetJsonArrayItem(const dep::JSONArray* jsArray, uint32_t index, bool& value);

template<typename T>
inline void GetJsonArrayItem(const dep::JSONArray* jsArray, uint32_t index, std::vector<T>& value);

template<typename T>
inline void GetJsonArrayItem(const dep::JSONArray* jsArray, uint32_t index, std::set<T>& value);

template<typename T>
inline void GetJsonArrayItem(const dep::JSONArray* jsArray, uint32_t index, std::unordered_set<T>& value);

template<typename T>
inline void GetJsonArrayItem(const dep::JSONArray* jsArray, uint32_t index, std::stack<T>& value);

template<typename K, typename V>
inline void GetJsonArrayItem(const dep::JSONArray* jsArray, uint32_t index, std::map<K, V>& value);
template<typename K, typename V>
inline void GetJsonArrayItem(const dep::JSONArray* jsArray, uint32_t index, std::unordered_map<K, V>& value);

//read json value definition
inline void ReadJsonValue(const dep::JSONObject* jsObj, const std::string& key, std::string& param) {
	param = jsObj->getString(key);
}

inline void ReadJsonValue(const dep::JSONObject* jsObj, const std::string& key, int& param) {
	param = jsObj->getInteger32(key);
}

inline void ReadJsonValue(const dep::JSONObject* jsObj, const std::string& key, uint32_t& param) {
	param = jsObj->getUnsignedInteger32(key);
}

inline void ReadJsonValue(const dep::JSONObject* jsObj, const std::string& key, int64_t& param) {
	param = jsObj->getInteger64(key);
}

inline void ReadJsonValue(const dep::JSONObject* jsObj, const std::string& key, uint64_t& param) {
	param = jsObj->getUnsignedInteger64(key);
}

inline void ReadJsonValue(const dep::JSONObject* jsObj, const std::string& key, double& param) {
	param = jsObj->getDouble(key);
}

inline void ReadJsonValue(const dep::JSONObject* jsObj, const std::string& key, bool& param) {
	param = jsObj->getBoolean(key);
}

template<typename T>
inline void ReadJsonValue(const dep::JSONObject* jsObj, const std::string& key, std::vector<T>& param) {
	dep::JSONArray* jsArray = jsObj->getJSONArray(key);
    if (nullptr == jsArray) return ;
	uint32_t arrayLen = jsArray->getLength();
	for (uint32_t i = 0; i < arrayLen; ++i) {
		T arrayItem;
		GetJsonArrayItem(jsArray, i, arrayItem);
		param.emplace_back(std::move(arrayItem));
	}
}

template<typename T>
inline void ReadJsonValue(const dep::JSONObject* jsObj, const std::string& key, std::stack<T>& param) {
	dep::JSONArray* jsArray = jsObj->getJSONArray(key);
    if (nullptr == jsArray) return ;
	uint32_t arrayLen = jsArray->getLength();
	for (uint32_t i = 0; i < arrayLen; ++i) {
		T arrayItem;
		GetJsonArrayItem(jsArray, i, arrayItem);
		param.emplace_back(std::move(arrayItem));
	}
}

template<typename T>
inline void ReadJsonValue(const dep::JSONObject* jsObj, const std::string& key, std::set<T>& param) {
	dep::JSONArray* jsArray = jsObj->getJSONArray(key);
    if (nullptr == jsArray) return ;
	uint32_t arrayLen = jsArray->getLength();
	for (uint32_t i = 0; i < arrayLen; ++i) {
		T arrayItem;
		GetJsonArrayItem(jsArray, i, arrayItem);
		param.emplace(std::move(arrayItem));
	}
}

template<typename T>
inline void ReadJsonValue(const dep::JSONObject* jsObj, const std::string& key, std::unordered_set<T>& param) {
	dep::JSONArray* jsArray = jsObj->getJSONArray(key);
    if (nullptr == jsArray) return ;
	uint32_t arrayLen = jsArray->getLength();
	for (uint32_t i = 0; i < arrayLen; ++i) {
		T arrayItem;
		GetJsonArrayItem(jsArray, i, arrayItem);
		param.emplace(std::move(arrayItem));
	}
}

template<typename K, typename V>
inline void ReadJsonValue(const dep::JSONObject* jsObj, const std::string& key, std::map<K, V>& param) {
	dep::JSONArray* jsArray = jsObj->getJSONArray(key);
    if (nullptr == jsArray) return ;
	uint32_t arrayLen = jsArray->getLength();
	for (uint32_t i = 0; i < arrayLen; ++i) {
		dep::JSONObject* pjsMapEntry = jsArray->getJSONObject(i);
		K key;
		V value;
		GetMapEntryItem(pjsMapEntry, key, value);
		param.emplace(std::move(key), std::move(value));
	}
}

template<typename K, typename V>
inline void ReadJsonValue(const dep::JSONObject* jsObj, const std::string& key, std::unordered_map<K, V>& param) {
	dep::JSONArray* jsArray = jsObj->getJSONArray(key);
    if (nullptr == jsArray) return ;
	uint32_t arrayLen = jsArray->getLength();
	for (uint32_t i = 0; i < arrayLen; ++i) {
		dep::JSONObject* pjsMapEntry = jsArray->getJSONObject(i);
		K key;
		V value;
		GetMapEntryItem(pjsMapEntry, key, value);
		param.emplace(std::move(key), std::move(value));
	}
}

template<typename K, typename V>
inline void GetMapEntryItem(const dep::JSONObject* jsObj, K& key, V& value) {
	ReadJsonValue(jsObj, "key", key);
	ReadJsonValue(jsObj, "value", value);
}



inline void GetJsonArrayItem(const dep::JSONArray* jsArray, uint32_t index, std::string& value) {
	value = jsArray->getString(index);
}

inline void GetJsonArrayItem(const dep::JSONArray* jsArray, uint32_t index, int& value) {
	value = jsArray->getInteger32(index);
}

inline void GetJsonArrayItem(const dep::JSONArray* jsArray, uint32_t index, uint32_t& value) {
	value = jsArray->getUnsignedInteger32(index);
}

inline void GetJsonArrayItem(const dep::JSONArray* jsArray, uint32_t index, int64_t& value) {
	value = jsArray->getInteger64(index);
}

inline void GetJsonArrayItem(const dep::JSONArray* jsArray, uint32_t index, uint64_t& value) {
	value = jsArray->getUnsignedInteger64(index);
}

inline void GetJsonArrayItem(const dep::JSONArray* jsArray, uint32_t index, double& value) {
	value = jsArray->getDouble(index);
}

inline void GetJsonArrayItem(const dep::JSONArray* jsArray, uint32_t index, bool& value) {
	value = jsArray->getBoolean(index);
}

template<typename T>
inline void GetJsonArrayItem(const dep::JSONArray* jsArray, uint32_t index, std::vector<T>& value) {
	dep::JSONArray* jsInnerArray = jsArray->getJSONArray(index);
	uint32_t arrayLen = jsArray->getLength();
	for (uint32_t i = 0; i < arrayLen; ++i) {
		T arrayItem;
		GetJsonArrayItem(jsInnerArray, i, arrayItem);
		value.emplace_back(std::move(arrayItem));
	}
}

template<typename T>
inline void GetJsonArrayItem(const dep::JSONArray* jsArray, uint32_t index, std::set<T>& value) {
	dep::JSONArray* jsInnerArray = jsArray->getJSONArray(index);
	uint32_t arrayLen = jsArray->getLength();
	for (uint32_t i = 0; i < arrayLen; ++i) {
		T arrayItem;
		GetJsonArrayItem(jsInnerArray, i, arrayItem);
		value.emplace(std::move(arrayItem));
	}
}

template<typename T>
inline void GetJsonArrayItem(const dep::JSONArray* jsArray, uint32_t index, std::unordered_set<T>& value) {
	dep::JSONArray* jsInnerArray = jsArray->getJSONArray(index);
	uint32_t arrayLen = jsArray->getLength();
	for (uint32_t i = 0; i < arrayLen; ++i) {
		T arrayItem;
		GetJsonArrayItem(jsInnerArray, i, arrayItem);
		value.emplace(std::move(arrayItem));
	}
}

template<typename T>
inline void GetJsonArrayItem(const dep::JSONArray* jsArray, uint32_t index, std::stack<T>& value) {
	dep::JSONArray* jsInnerArray = jsArray->getJSONArray(index);
	uint32_t arrayLen = jsArray->getLength();
	for (uint32_t i = 0; i < arrayLen; ++i) {
		T arrayItem;
		GetJsonArrayItem(jsInnerArray, i, arrayItem);
		value.emplace_back(std::move(arrayItem));
	}
}

template<typename K, typename V>
inline void GetJsonArrayItem(const dep::JSONArray* jsArray, uint32_t index, std::map<K, V>& value) {
	dep::JSONArray* jsInnerArray = jsArray->getJSONArray(index);
	uint32_t arrayLen = jsArray->getLength();
	for (uint32_t i = 0; i < arrayLen; ++i) {
		dep::JSONObject* pjsObj = jsArray->getJSONObject(i);
		K key;
		V value;
		GetMapEntryItem(pjsObj, key, value);
		value.emplace(std::move(key), std::move(value));
	}
}

template<typename K, typename V>
inline void GetJsonArrayItem(const dep::JSONArray* jsArray, uint32_t index, std::unordered_map<K, V>& value) {
	dep::JSONArray* jsInnerArray = jsArray->getJSONArray(index);
	uint32_t arrayLen = jsArray->getLength();
	for (uint32_t i = 0; i < arrayLen; ++i) {
		dep::JSONObject* pjsObj = jsArray->getJSONObject(i);
		K key;
		V value;
		GetMapEntryItem(pjsObj, key, value);
		value.emplace(std::move(key), std::move(value));
	}
}

inline void GetJsonArrayItem(const dep::JSONArray* jsArray, uint32_t index, AccountInfo& value) {
    dep::JSONObject* object = jsArray->getJSONObject(index);
    value.userID = object->getString(JSON_KEY_USERID);
    value.encrypted = object->getBoolean(JSON_KEY_ENCRYPTED);
    value.password = object->getString(JSON_KEY_USERPASSWORD);
}

inline void GetJsonArrayItem(const dep::JSONArray* jsArray, uint32_t index, MonitorAccountInfo& value) {
	dep::JSONObject* object = jsArray->getJSONObject(index);
	value.userID = object->getString(JSON_KEY_USERID);
	value.encrypted = object->getBoolean(JSON_KEY_ENCRYPTED);
	value.password = object->getString(JSON_KEY_USERPASSWORD);
	value.queryAble = object->getBoolean(JSON_KEY_QUERYABLE);
	value.modifyAble = object->getBoolean(JSON_KEY_MODIFYABLE);

}

inline void GetJsonArrayItem(const dep::JSONArray* jsArray, uint32_t index, GroupInfo& value) {
    dep::JSONObject* object = jsArray->getJSONObject(index);
    value.name = object->getString(JSON_KEY_GROUP_NAME);
    dep::JSONArray* pJsArray = object->getJSONArray(JSON_KEY_MEMBERS);
    uint32_t arrayLength = pJsArray->getLength();
    for (uint32_t i = 0; i < arrayLength; ++i) {
        value.members.push_back(pJsArray->getString(i));
    }
}

inline void GetJsonArrayItem(const dep::JSONArray* jsArray, uint32_t index, AuthorizationInfo& value) {
	dep::JSONObject* object = jsArray->getJSONObject(index);
	value.channel = object->getString(JSON_KEY_CHANNEL);
	dep::JSONArray* pJsArray = object->getJSONArray(JSON_KEY_READGROUPS);
	uint32_t arrayLength = pJsArray->getLength();
	for (uint32_t i = 0; i < arrayLength; ++i) {
		value.readGroups.push_back(pJsArray->getString(i));
	}

	pJsArray = object->getJSONArray(JSON_KEY_WRITEROUPS);
	arrayLength = pJsArray->getLength();
	for (uint32_t i = 0; i < arrayLength; ++i) {
		value.writeGroups.push_back(pJsArray->getString(i));
	}
}

//interface
template< typename T, typename ... Args>
inline void ReadJsonInternal(const dep::JSONObject* jsObj, const std::string key, T& param) {
	ReadJsonValue(jsObj, key, param);
}

template< typename T, typename ... Args>
inline void ReadJsonInternal(const dep::JSONObject* jsObj, const std::string key, T&& param, Args&&... args) {
	ReadJsonValue(jsObj, key, param);
	ReadJsonInternal(jsObj, std::forward<Args>(args)...);
}


template< typename T, typename ... Args>
inline void ReadJson(const char* jsonStr, uint32_t len, const std::string key, T&& param, Args&&... args) {
    dep::JSONByteArrayTokenizer tok;
	tok.initialize(jsonStr, len);
	auto jsObj = dep::make_unique<dep::JSONObject>(tok);
	ReadJsonInternal(jsObj.get(), key, param, std::forward<Args>(args)...);
}

template< typename T, typename ... Args>
inline void ReadJson(const std::string& jsonStr, const std::string key, T&& param, Args&&... args) {
    dep::JSONByteArrayTokenizer tok;
	tok.initialize(jsonStr.data(), jsonStr.length());
	auto jsObj = dep::make_unique<dep::JSONObject>(tok);
	ReadJsonInternal(jsObj.get(), key, param, std::forward<Args>(args)...);
}

}} // namespace lightmq { namespace util {


#endif
