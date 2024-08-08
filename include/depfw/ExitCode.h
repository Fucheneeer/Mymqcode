////////////////////////////////////////////////////////////////////////////////
//
// ExitCode.h
//
// Copyright (c) 2020 CFETSIT.
// Building 3, 491 YinDong Road, Shanghai, China.
// All rights reserved.
//
// LightMQ is the copyrighted,
// proprietary property of CFETSIT which retain all right, title and interest therein.
//
// Description: Define the exit code of application
//
////////////////////////////////////////////////////////////////////////////////

#ifndef EXITCODE_H_
#define EXITCODE_H_

#include <cstdint>

namespace dep {

/**
 * Application Exit Code
 */
enum class ExitCode : int16_t {
    NORMAL          = 0, //!< No Error
    INVALID_USAGE   = 1, //!< Invalid Usage
    EXCEPTION       = 99,//!< Exception Thrown
};

} /* namespace dep */

#endif /* EXITCODE_H_ */
