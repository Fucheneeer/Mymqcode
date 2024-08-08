////////////////////////////////////////////////////////////////////////////////
//
// LightMQ is the copyrighted,
// proprietary property of CFETSIT which retain all right, title and interest therein.
//
// Description:
//
////////////////////////////////////////////////////////////////////////////////

#ifndef PLATFORM_H
#define PLATFORM_H

#include<iostream>
#include<atomic>

namespace lightmq { namespace util {

#if defined  __aarch64__
#define MEMORY_ORDER  std::memory_order_release
#else
#define MEMORY_ORDER  std::memory_order_relaxed
#endif
}} // namespace lightmq { namespace util{
#endif
