////////////////////////////////////////////////////////////////////////////////
//
// SelectorApplication.h
//
// Copyright (c) 2020 CFETSIT.
// Building 3, 491 YinDong Road, Shanghai, China.
// All rights reserved.
//
// LightMQ is the copyrighted,
// proprietary property of CFETSIT which retain all right, title and interest therein.
//
// Description: Header file for class SelectorApplication
//
////////////////////////////////////////////////////////////////////////////////

#ifndef SELECTORAPPLICATION_H_
#define SELECTORAPPLICATION_H_

#include <memory>
#include <string>

#include "IApplication.h"
#include "IInternalEventService.h"

namespace dep {

class ISelector;
class ITimer;

/**
 * @brief Derived class of IApplication which offer ISelector and ITimer service
 *
 * SelectorApplication inherits from IApplication and framework would inject extra
 * ISelector and ITimer service to the this application. In the run() function,
 * SelectorApplication should also register its interested event to the ISelector
 * service.
 *
 */
class SelectorApplication : public virtual IApplication {
public:

    /**
     * Constructor
     */
    SelectorApplication() = default;

    /**
     * Destructor
     */
    virtual ~SelectorApplication() = default;

    /**
     * Perform initialization.
     * Subclass should invoke this method in first statement if it would like to
     * override this method.
     *
     */
    virtual void init() override;

    /**
     * Perform application processing
     *
     * @return application exitcode
     */
    virtual ExitCode run() override;

    /**
     * Perform shutdown work.
     * Subclass should invoke this method in last statement if it would like to
     * override this method.
     */
    virtual void shutdown() override;

    /**
     * Obtain a framework provided selector.
     *
     * @param pSelector The selector
     */
    virtual void setSelector(ISelector* pSelector);

    /**
     * Obtain a framework provided timer.
     *
     * @param pTimer The timer
     */
    virtual void setTimer(ITimer* pTimer) {
        m_pTimer = pTimer;
    }

protected:
    ISelector* m_pSelector{nullptr};  //!< Selector Service
    ITimer* m_pTimer{nullptr};        //!< Timer Service
    std::unique_ptr<IInternalEventService> m_pInternalEventService; /**< Internal event service */
};

} /* namespace dep */

#endif /* SELECTORAPPLICATION_H_ */
