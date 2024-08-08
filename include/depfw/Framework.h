////////////////////////////////////////////////////////////////////////////////
//
// Framework.h
//
// Copyright (c) 2020 CFETSIT.
// Building 3, 491 YinDong Road, Shanghai, China.
// All rights reserved.
//
// LightMQ is the copyrighted,
// proprietary property of CFETSIT which retain all right, title and interest therein.
//
// Description: Header file for class Framework
//
////////////////////////////////////////////////////////////////////////////////

#ifndef FRAMEWORK_H_
#define FRAMEWORK_H_

#include <memory>

namespace dep {

class ISelectService;
class ITimer;

/**
 * @brief A central API to get access to framework provided services.
 *
 */
class Framework {
public:

    /**
     * Select Service Implementation Hints
     */
    enum class SelectServiceImpl {
        POLL, //!< POLL
        EPOLL //!< EPOLL
    };

    Framework() = delete;

    /**
     * Create a select service according to given implementation hint.
     *
     * @param impl The implementation hint, default is POLL
     * @return a new select service
     */

    static std::unique_ptr<ISelectService> createSelectService(
                    SelectServiceImpl impl = SelectServiceImpl::POLL);

    /**
     * Create a new standalone timer service which have its own thread and was started.
     * The internal thread will be terminated when destructor of TimerService is invoked.
     *
     * @return a new started timer service
     */
    static std::unique_ptr<ITimer> createTimer();

};

} /* namespace dep */

#endif /* FRAMEWORK_H_ */
