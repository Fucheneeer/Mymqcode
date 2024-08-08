////////////////////////////////////////////////////////////////////////////////
//
// util.h
//
// Copyright (c) 2020 CFETSIT.
// Building 3, 491 YinDong Road, Shanghai, China.
// All rights reserved.
//
// LightMQ is the copyrighted,
// proprietary property of CFETSIT which retain all right, title and interest therein.
//
// Description: Implementation of NoLock using CAS operations.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <string>

namespace lightmq { namespace util {

std::string hexdumpLimit(const void* p, size_t length, size_t limit);
}}
