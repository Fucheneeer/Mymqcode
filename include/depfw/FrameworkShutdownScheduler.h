////////////////////////////////////////////////////////////////////////////////
//
// FrameworkShutdownScheduler.h
//
// Copyright (c) 2020 CFETSIT.
// Building 3, 491 YinDong Road, Shanghai, China.
// All rights reserved.
//
// LightMQ is the copyrighted,
// proprietary property of CFETSIT which retain all right, title and interest therein.
//
// Description: Header of FrameworkShutdownScheduler class
//
////////////////////////////////////////////////////////////////////////////////

#ifndef FRAMEWORKSHUTDOWNSCHEDULER_H_
#define FRAMEWORKSHUTDOWNSCHEDULER_H_

#include <memory>

#include "IShutdownScheduler.h"

namespace dep {

class ISelectService;
class IApplication;

/**
 * The ShutdownScheduler for Framework.
 * It will call IApplication shutdown and stop the select service.
 */
class FrameworkShutdownScheduler : public IShutdownScheduler {

public:

    /**
     * Default Constructor
     */
    FrameworkShutdownScheduler() = default;

    /**
     * Virtual destructor.
     */
    virtual ~FrameworkShutdownScheduler() = default;

    /**
     * It will call shutdown on passed-in IApplication object and stop the select service.
     *
     * @param pApplication
     * @return
     */
    bool scheduleShutdown(IApplication* pApplication);

    /**
     * Set the SelectService object.
     *
     * @param pSelectService    the SelectService object
     */
    void setSelectService(ISelectService* pSelectService);

private:
    ISelectService* m_pSelectService {nullptr};
};

} /* namespace dep */

#endif /* FRAMEWORKSHUTDOWNSCHEDULER_H_ */
