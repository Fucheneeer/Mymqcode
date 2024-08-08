////////////////////////////////////////////////////////////////////////////////
//
// LogWrapper.h
//
// Copyright (c) 2020 CFETSIT.
// Building 3, 491 YinDong Road, Shanghai, China.
// All rights reserved.
//
// Distributed Service Platform (DSP) is the copyrighted,
// proprietary property of CFETSIT which retain all right, title and interest therein.
//
// Description: Severity Logging Interface class
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <unistd.h> // for syscall.
#include <sys/syscall.h>
#include "ISeverityLog.h"


namespace lightmq { namespace util {

std::string getCurrentTimeString();

}}

#ifdef NDEBUG
#ifndef DEBUG_PRINT
#define DEBUG_PRINT(fmt, ...) 
#endif

#else

#ifndef DEBUG_PRINT
#ifndef gettid
#define gettid() syscall(__NR_gettid) 
#endif
#define DEBUG_PRINT(fmt, ...) printf("<%s %s:%d:%lu> " fmt "\n", lightmq::util::getCurrentTimeString().c_str(), __FILE__, __LINE__, syscall(__NR_gettid), ##__VA_ARGS__)
#endif



#endif
