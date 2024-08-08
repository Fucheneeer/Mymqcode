////////////////////////////////////////////////////////////////////////////////
//
// TypeTraits.cpp
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
#ifndef TYPE_TRAITS_H_
#define TYPE_TRAITS_H_
#include <vector>
#include <queue>
#include <deque>
#include <list>
#include <stack>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>

namespace lightmq{ namespace util{
//LIST
template<class T> struct is_list_container : public std::false_type {};

template<class T, class Alloc>
struct is_list_container<std::vector<T, Alloc>> : public std::true_type {};

template<class T, class Alloc>
struct is_list_container<std::deque<T, Alloc>> : public std::true_type {};

template<class T, class Alloc>
struct is_list_container<std::queue<T, Alloc>> : public std::true_type {};

template<class T, class Alloc>
struct is_list_container<std::list<T, Alloc>> : public std::true_type {};

template<class T, class PR, class Alloc>
struct is_list_container<std::set<T, PR, Alloc>> : public std::true_type {};

template<class T, class Hasher, class Equal, class Alloc>
struct is_list_container<std::unordered_set<T, Hasher, Equal, Alloc>> : public std::true_type {};

//emplace back
template<class T> struct is_emplace_back : public std::false_type {};

template<class T, class Alloc>
struct is_emplace_back<std::vector<T, Alloc>> : public std::true_type {};

template<class T, class Alloc>
struct is_emplace_back<std::deque<T, Alloc>> : public std::true_type {};

template<class T, class Alloc>
struct is_emplace_back<std::list<T, Alloc>> : public std::true_type {};

//emplace
template<class T> struct is_emplace : public std::false_type {};

template<class T, class Alloc>
struct is_emplace<std::queue<T, Alloc>> : public std::true_type {};

template<class T, class PR, class Alloc>
struct is_emplace<std::set<T, PR, Alloc>> : public std::true_type {};

template<class T, class Hasher, class Equal, class Alloc>
struct is_emplace<std::unordered_set<T, Hasher, Equal, Alloc>> : public std::true_type {};


//MAP
template<class T> struct is_map : public std::false_type {};

template<class K, class V, class PR, class Alloc>
struct is_map<std::map<K, V, PR, Alloc>> : public std::true_type {};

template<class K, class V, class Hasher, class Equal, class Alloc>
struct is_map<std::unordered_map<K, V, Hasher, Equal, Alloc>> : public std::true_type {};

//VARIANT VALUE
template<class T> struct is_varvalue_type : public std::false_type {};

template<> struct is_varvalue_type<bool> : public std::true_type {};
template<> struct is_varvalue_type<int8_t> : public std::true_type {};
template<> struct is_varvalue_type<int16_t> : public std::true_type {};
template<> struct is_varvalue_type<int32_t> : public std::true_type {};
template<> struct is_varvalue_type<int64_t> : public std::true_type {};
template<> struct is_varvalue_type<float> : public std::true_type {};
template<> struct is_varvalue_type<double> : public std::true_type {};
template<> struct is_varvalue_type<uint16_t> : public std::true_type {};
template<> struct is_varvalue_type<std::string> : public std::true_type {};

}}
#endif
