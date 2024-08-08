////////////////////////////////////////////////////////////////////////////////
//
// IApplication.h
//
// Copyright (c) 2020 CFETSIT.
// Building 3, 491 YinDong Road, Shanghai, China.
// All rights reserved.
//
// LightMQ is the copyrighted,
// proprietary property of CFETSIT which retain all right, title and interest therein.
//
// Description: Define the interface of IApplication
//
////////////////////////////////////////////////////////////////////////////////

#ifndef IAPPLICATION_H_
#define IAPPLICATION_H_

#include <string>

#include "ExitCode.h"

namespace dep {

/**
 * @brief Base class for all application and defines the application lifecycle
 *
 * IApplication defines the application lifecycle which follows a sequence:
 * 1. init() - notify application to perform initialization with the command line arguments
 * 2. run() - notify application to start processing
 * 3. shutdown() - notify application on preparing shutdown
 *
 * IApplication acts as the basic application unit and its main logic should be
 * implemented in the run() function.
 */
class IApplication {

public:

    /**
     * Destructor
     */
    virtual ~IApplication() = default;

    /**
     * Perform initialization
     *
     */
    virtual void init() = 0;

    /**
     * Perform application processing
     *
     * @return application exitcode
     */
    virtual ExitCode run() = 0;

    /**
     * Perform shutdown work.
     *
     * Implementation must ensure this function is idempotent because it may be
     * invoked multiple times.
     */
    virtual void shutdown() = 0;

    /**
     * Get the name of this application
     *
     * @return The name of this application
     */
    virtual const std::string& getName() const = 0;

    /**
     * Get application version.
     *
     * @return application version
     */
    virtual const std::string& getVersion() const = 0;
};

} /* namespace dep */

#endif /* IAPPLICATION_H_ */
