////////////////////////////////////////////////////////////////////////////////
//
// CommonConstant.h
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
namespace lightmq { namespace util {

class CommonConstant {
public:
    CommonConstant()=delete;
    ~CommonConstant()=delete;

    static constexpr auto MAX_TCP_MSG_BUFFER_SIZE = 96 * 1024 * 1024;
    static constexpr auto MAX_TCP_MSG_RX_BUFFER_SIZE = 112 * 1024 * 1024;



    static constexpr auto BASE_GROUP_SEQUENCE = 1;
	static constexpr auto INNER_GROUP_NAME = "__INNER_GROUP_NAME__";
	static constexpr auto INNER_GROUP_ID = "__INNER_GROUP_ID__";
	static constexpr auto INNER_GROUP_SEQ = "__INNER_GROUP_SEQ__";
	static constexpr auto INNER_GROUP_ISEND = "__INNER_GROUP_ISEND__";

    static constexpr auto MAX_MESSAGE_LEVEL = 9; // [0,9]
    static constexpr auto MAX_MESSAGE_LEVEL_COUNT = 10; // 
    static constexpr auto DEFAULT_MESSAGE_LEVEL = 4; // 
};

}} // namespace lightmq { namespace broker {

