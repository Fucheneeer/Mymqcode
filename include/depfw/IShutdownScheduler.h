////////////////////////////////////////////////////////////////////////////////
//
// IShutdownScheduler.h
//
// Copyright (c) 2020 CFETSIT.
// Building 3, 491 YinDong Road, Shanghai, China.
// All rights reserved.
//
// LightMQ is the copyrighted,
// proprietary property of CFETSIT which retain all right, title and interest therein.
//
// Description: Interface of ShutdownScheduler
//
////////////////////////////////////////////////////////////////////////////////

#ifndef ISHUTDOWNSCHEDULER_H_
#define ISHUTDOWNSCHEDULER_H_

namespace dep {

class IApplication;

/**
 * This class provides an interface for a shutdown scheduler
 * which can be called to terminate an IApplication.
 */
class IShutdownScheduler {

public:
    /**
     * Virtual destructor.
     */
    virtual ~IShutdownScheduler() = default;

    /**
     * Calls the IApplication shutdown function.
     *
     * @param application the IApplication which needs to be shutdown
     * @return true if shutdown successful and false otherwise
     */
    virtual bool scheduleShutdown(IApplication* application) = 0;
};


} /* namespace dep */

#endif /* ISHUTDOWNSCHEDULER_H_ */
