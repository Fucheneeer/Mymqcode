////////////////////////////////////////////////////////////////////////////////
//
// IDEPMain.h
//
// Copyright (c) 2020 CFETSIT.
// Building 3, 491 YinDong Road, Shanghai, China.
// All rights reserved.
//
// LightMQ is the copyrighted,
// proprietary property of CFETSIT which retain all right, title and interest therein.
//
// Description: Define the interface for application starting with dep::main()
//
////////////////////////////////////////////////////////////////////////////////

#ifndef IDEPMAIN_H_
#define IDEPMAIN_H_

#include "IApplication.h"

namespace dep {

class ICommandLineArgumentValue;
class CommandLineSpec;
class IShutdownScheduler;
class SignalProcessor;

/**
 * @brief Base class for all application starting with dep::main()
 *
 * IDEPMain derived from IApplication and defines an interface for application
 * staring with main(argc, argv) function.
 *
 * The DEP_APP_MAIN macro can be used to implement the main(argc, argv) function
 * which in turns pass the derived application to dep::main() for instantiation,
 * service injection and start processing.
 *
 * To integrate other user-defined main(argc, argv) function, user can also instantiate
 * IDEPMain application by calling dep::main() in a separate thread.
 */
class IDEPMain : public virtual IApplication {

public:

    /**
     * Destructor
     */
    virtual ~IDEPMain() = default;

    /**
     * Define the CommandLineArguments and add them to the CommandLineSpec object.
     *
     * @param spec the CommandLineSpec reference
     */
    virtual void defineCommandLineArguments(CommandLineSpec& spec) = 0;

    /**
     * Get the command line arguments value
     *
     * @param argumentValue  Command line argument values
     */
    virtual void setCommandLineArguments(const ICommandLineArgumentValue* argumentValue) = 0;

    /**
     * This shutdown scheduler is passed in to an IApplication for it to call during shutdown
     *
     * @param shutdownScheduler the shutdown scheduler
     */
    virtual void setShutdownScheduler(IShutdownScheduler* shutdownScheduler) = 0;

    /**
     * Subclass implements this function to register their signals; it can be left blank if
     * no need to handle signals.
     *
     * @param processor the SignalProcessor singleton instance
     */
    virtual void registerSignalHandler(SignalProcessor* processor) = 0;

};

} /* namespace dep */



#endif /* IDEPMAIN_H_ */
