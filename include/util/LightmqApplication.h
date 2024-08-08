////////////////////////////////////////////////////////////////////////////////
//
// LightmqApplication.h
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

#ifndef LIGHTMQAPPLICATION_H_
#define LIGHTMQAPPLICATION_H_

#include <string>
#include <memory>

#include "SelectorApplication.h"
#include "IDEPMain.h"
#include "SignalProcessor.h"
#include "Properties.h"
#include "ApplicationLog.h"
#include "CommandLineArgument.h"
#include "CommandLineSpec.h"
#include "ICommandLineArgumentValue.h"
#include "DEPException.h"
#include "MemoryBackport.h"
#include "IShutdownScheduler.h"
#include "LockedProperties.h"

namespace lightmq { namespace util {

class SelectableSignalHandler;

class LightmqApplication : public dep::SelectorApplication
                      , public dep::IDEPMain
                      , public dep::IInternalEventService::IInternalEventHandler {
protected:
    /**
     * Internal event specific for DEPCoreApplication.
     */
    class DEPCoreApplicationInternalEvent: public dep::IInternalEventService::IInternalEvent {
    public:
        /**
         * Event type specific for DEPCoreApplication.
         */
        enum EventType : uint32_t {
            UNIX_SIGNAL = combine(EventCategory::CORE_APPLICATION, 0) //!< UNIX_SIGNAL
        };

    public:
        /**
         * Destructor.
         */
        virtual ~DEPCoreApplicationInternalEvent() = default;
    };

    /**
     * UNIX signal event.
     */
    class UNIXSignalEvent : public DEPCoreApplicationInternalEvent {
    public:
        /**
         * Constructor.
         *
         * @param signalNum signal received
         */
        explicit UNIXSignalEvent(int signalNum)
            : m_signalNum{signalNum} {
        }
    public:
        virtual uint32_t getEventType() const override {
            return EventType::UNIX_SIGNAL;
        }

        /**
         * Get signal number.
         *
         * @return The signal number.
         */
        int getSignalNum() const {
            return m_signalNum;
        }

    private:
        int m_signalNum;
    };

    /**
     * Private inner class to implements the signal handler
     * This handler put the signal number to the given SelectableQueue
     */
    class SelectableSignalHandler : public dep::SignalHandler {
    public:
        /**
         * Constructor
         * @param internalEventService Internal event service
         */
        explicit SelectableSignalHandler(dep::IInternalEventService& internalEventService)
            : m_internalEventService(internalEventService) {
        }

        void onUnixSignal(int signalNum) override {
            m_internalEventService.publishInternalEvent(std::make_shared<UNIXSignalEvent>(signalNum));
        }

    private:
        dep::IInternalEventService& m_internalEventService;
    };

public:
    LightmqApplication();
    virtual ~LightmqApplication();

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
    virtual dep::ExitCode run() override;

    /**
     * Perform shutdown work.
     * Subclass should invoke this method in last statement if it would like to
     * override this method.
     */
    virtual void shutdown() override;


    /**
     * Get the name of this application
     *
     * @return The name of this application
     */
    virtual const std::string& getName() const =0;

    /**
     * Get application version.
     *
     * @return application version
     */
    virtual const std::string& getVersion() const =0;

    /**
     * Define the CommandLineArguments and add them to the CommandLineSpec object.
     *
     * @param spec the CommandLineSpec reference
     */
    virtual void defineCommandLineArguments(dep::CommandLineSpec& spec) override;

    /**
     * Get the command line arguments value
     *
     * @param argumentValue  Command line argument values
     */
    virtual void setCommandLineArguments(const dep::ICommandLineArgumentValue* argumentValue) override;

    /**
     * Subclass implements this function to register their signals; it can be left blank if
     * no need to handle signals.
     *
     * @param processor the SignalProcessor singleton instance
     */
    virtual void registerSignalHandler(dep::SignalProcessor* processor) override;

    /**
     * Set the ShutdownScheduler for the application to call during shutdown
     *
     * @param shutdownScheduler the shutdown scheduler
     */
    virtual void setShutdownScheduler(dep::IShutdownScheduler* shutdownScheduler) override {
        m_shutdownScheduler = shutdownScheduler;
    }



    virtual void onInternalEvent(std::shared_ptr<dep::IInternalEventService::IInternalEvent> pInternalEvent) override;

protected:
    dep::ISeverityLog& getLogger() {
        return m_applicationLog.getLogger();
    }

    LockedProperties &getProperties() {
        return m_applicationProperties;
    }

    const LockedProperties &getProperties() const {
        return m_applicationProperties;
    }

private:
    void initLogger(const dep::Properties& logProperties);

protected:
    // dep::Properties m_applicationProperties;  //!< all application configuration
    LockedProperties m_applicationProperties;  //!< all application configuration
private:

    dep::ApplicationLog m_applicationLog;     //!< application logger


    std::unique_ptr<SelectableSignalHandler> m_signalHandler{nullptr}; //!< Signal Handler

    std::string m_applicationConfigurationFileName{""}; /**< Path to application specific properties configuration file */

    dep::IShutdownScheduler* m_shutdownScheduler{nullptr};  //!< Shutdown Scheduler

};


}} // namespace lightmq { namespace util {

#endif /* LIGHTMQAPPLICATION_H_ */
