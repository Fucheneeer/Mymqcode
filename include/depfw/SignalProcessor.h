////////////////////////////////////////////////////////////////////////////////
//
// SignalProcessor.h
//
// Copyright (c) 2020 CFETSIT.
// Building 3, 491 YinDong Road, Shanghai, China.
// All rights reserved.
//
// LightMQ is the copyrighted,
// proprietary property of CFETSIT which retain all right, title and interest therein.
//
// Description: Header file for class SignalProcessor
//
////////////////////////////////////////////////////////////////////////////////

#ifndef SIGNALPROCESSOR_H_
#define SIGNALPROCESSOR_H_

#include <array>
#include <csignal>
#include <list>
#include <vector>

namespace dep {

class SignalHandler;

/**
 * @brief Singleton class for registering Unix signal handler.
 *
 * Currently supports Unix signal from 1 to 31.
 * When a signal is detected, it will fire all registered callbacks (SignalHandler) for that signal.
 * Since the only implemented callbacks are currently terminating signals (SIGQUIT and SIGTERM), there
 * is no signal reregisteration after a callback has been fired.
 *
 */
class SignalProcessor {

public:
    /// Maximum supported POSIX signals is 31
    static const int MAX_SIGNAL = 31;

public:
    /**
     * Get the SignalProcessor singleton instance.
     *
     * @return the singleton instance
     */
    static SignalProcessor& getInstance() {
        return m_signalProcessor;
    }

    /**
     * Fires all registered signal handlers for a particular Unix signal.
     * The signal handlers for a particular signal are placed under an array slot,
     * which further contains a vector which can contain multiple handlers.
     * This function is static as it needs to call getInstance
     *
     * @param signalNum the Unix signal which is detected
     */
    static void handleSignal(int signalNum);

    /**
     * Register a signal with a handler.
     * The handler will be stored in a vector according to its Unix signal.
     *
     * @param signal    the Unix signal
     * @param handler   the associated signal handler
     */
    void registerSignalHandler(int signal, SignalHandler* const handler);

    /**
     * Unregister a handlers for a signal.
     *
     * @param signal  the Unix signal to unregister.
     * @param handler the associated signal handler
     */
    void unregisterSignalHandler(int signal,SignalHandler* const handler);

    /**
     * Unregister all handlers for all signals.
     *
     */
    void unregisterAllSignalHandler();

private:
    // not to be called directly for Singleton class
    SignalProcessor() {}
    ~SignalProcessor() {}
    SignalProcessor(const SignalProcessor&);
    const SignalProcessor& operator=(const SignalProcessor&);

private:
    static SignalProcessor m_signalProcessor;
    std::array<std::list<SignalHandler*>, MAX_SIGNAL> m_handlerSignalArray;

};

/**
 * @brief Base Handler for all signal handler that registered to the SignalProcessor
 *
 * This class cooperates closely with the SingalProcessor and help keep tracks
 * the signal number has been registered to the SignalProcessor.
 * During the handler destruction, it will automatically unregistered all the
 * signal number from SignalProcessor.
 */
class SignalHandler {

friend SignalProcessor;

public:
   /**
    * Constructor
    */
    SignalHandler() = default;

    /**
     * Destructor
     *
     * It will automatically unregister all of the signal handler from SignalProcessor
     */
    virtual ~SignalHandler() {
        while (!m_registeredSignal.empty()) {
            SignalProcessor::getInstance().unregisterSignalHandler(m_registeredSignal.front(), this);
        }
    }

    /**
     * Callback function for received signal
     *
     * @param signalNum  signal number received
     */
    virtual void onUnixSignal(int signalNum) = 0;

private:
    /**
     * Save the signal number that has been registered
     * This function should be only accessed by the friend class SignalProcessor
     *
     * @param signalNum  Signal number registered to Signal Processor
     */
    void addSignal(int signalNum) {
        m_registeredSignal.push_back(signalNum);
    }

    /**
     * Remove the signal number
     * This function should be only accessed by the friend class SignalProcessor
     *
     * @param signalNum  Signal number unregistered from Signal Processor
     */
    void removeSignal(int signalNum) {
        m_registeredSignal.remove(signalNum);
    }

private:
    /** Keep the registered signal */
    std::list<int> m_registeredSignal;
};

}  /* namespace dep */

#endif /* SIGNALPROCESSOR_H_ */
