////////////////////////////////////////////////////////////////////////////////
//
// IClockSource.h
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

#ifndef ICLOCKSOURCE_H_
#define ICLOCKSOURCE_H_

namespace lightmq { namespace util {
class IClockSource {
public:
    IClockSource() = default;
    virtual ~IClockSource() = default;

    virtual std::chrono::system_clock::time_point now() = 0;
};

}} // namespace lightmq { namespace util {

#endif /* ICLOCKSOURCE_H_ */
