////////////////////////////////////////////////////////////////////////////////
//
// ApplicationLog.h
//
// Copyright (c) 2020 CFETSIT.
// Building 3, 491 YinDong Road, Shanghai, China.
// All rights reserved.
//
// LightMQ is the copyrighted,
// proprietary property of CFETSIT which retain all right, title and interest therein.
//
// Description: Header of ApplicationLog class
//
////////////////////////////////////////////////////////////////////////////////

#ifndef APPLICATIONLOG_H_
#define APPLICATIONLOG_H_

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>

#include "CaseInsensitiveCompare.h"
#include "SeverityLog.h"

namespace dep {

class Properties;
class FileSink;
class LogDecorator;

/**
 * Class under Framework providing logging service.
 * The log settings are specified by a Properties file.
 *
 * For example:
 * <blockquote><pre>
 * dep.framework.log.sink.sink1.type=ConsoleSink                  <- specify sink info
 * dep.framework.log.sink.sink2.type=FileSink
 * dep.framework.log.sink.sink2.path=/export/home/share/test1.log <- FileSink needs to specify path as well
 * dep.framework.log.sink.sink3.type=FileSink
 * dep.framework.log.sink.sink3.path=/export/home/share/test2.log
 * dep.framework.log.debug=sink1                                  <- specify which mesg severity goes to which sink
 * dep.framework.log.warn=sink2
 * dep.framework.log.error=sink1,sink3
 * </pre></blockquote>
 *
 */
class ApplicationLog {

public:
    static const std::string DELIMITER;         /**< ApplicationLog DELIMITER Constant */
    static const std::string LOG;               /**< ApplicationLog LOG Constant */
    static const std::string SINK;              /**< ApplicationLog SINK Constant */
    static const std::string FILESINK;          /**< ApplicationLog FILESINK Constant */
    static const std::string CONSOLESINK;       /**< ApplicationLog CONSOLESINK Constant */
    static const std::string FILTERMODULE;      /**< ApplicationLog FILTERMODULE Constant */
    static const std::string TYPE;              /**< ApplicationLog TYPE Constant */
    static const std::string PATH;              /**< ApplicationLog PATH Constant */
    static const std::string ENABLEROLLING;     /**< ApplicationLog ENABLEROLLING Constant */
    static const std::string ENABLEROLLINGBYSIZE; /**< ApplicationLog ENABLEROLLINGBYSIZE Constant */
    static const std::string ROLLINGPATTERN;    /**< ApplicationLog ROLLINGPATTERN Constant */
    static const std::string ROLLINGSEPARATOR;  /**< ApplicationLog ROLLINGSEPARATOR Constant */
    static const std::string ROLLINGSIZE;       /**< ApplicationLog ROLLINGSIZE Constant */
    static const std::string ROLLINGFILECOUNT;  /**< ApplicationLog ROLLINGFILECOUNT Constant*/
    static const std::string DISABLE;           /**< ApplicationLog DISABLE Constant */

    static const std::string DEBUG;             /**< ApplicationLog DEBUG Constant */
    static const std::string INFO;             /**< ApplicationLog INFO Constant */
    static const std::string WARN;              /**< ApplicationLog WARN Constant */
    static const std::string ERROR;             /**< ApplicationLog ERROR Constant */
    static const std::string EVENT;             /**< ApplicationLog EVENT Constant */
    static const std::string AUDIT;             /**< ApplicationLog AUDIT Constant */
    static const std::string EXCEPTION;         /**< ApplicationLog EXCEPTION Constant */

private:
    static const std::map<SeverityLog::Severity, std::string> SEVERITY_STRINGS; //!< Mapping from severity to display string

    /**
     * Case insensitive mapping from display string to severity.
     */
    static const std::map<std::string, SeverityLog::Severity, CaseInsensitiveCompare> STRINGS_SEVERITY;

public:
    ApplicationLog();

    virtual ~ApplicationLog();

public:
    /**
     * Disable copy.
     */
    ApplicationLog(const ApplicationLog& log) = delete;

    /**
     * Disable assignment.
     */
    const ApplicationLog& operator=(const ApplicationLog& log) = delete;

public:
    /**
     * Initialize the specified settings using a Properties object.
     * This function throws a DEPException if there is error encountered
     *
     * @param logProperties     the Properties containing the log settings
	 * @param name              the application name for logging prefix
     * @return true if no error and false otherwise
     */
    void initialize(const Properties& logProperties, const std::string& name = "");

    /**
     * Initialize application log using given function callback.
     *
     * @param logConfigurator Function to configure this application log
     * @param name              the application name for logging prefix
     */
    void initialize(std::function<void(SeverityLog&)> logConfigurator, const std::string& name = "");

    /**
     * Sink creator is responsible for closing the sink.
     * Closes the sinks when done, if applicable.
     * Currently only FileSink requires closing.
     */
    void close();

    /**
     * Get the application logger
     *
     * @return the logger to be used by application
     */
    ISeverityLog& getLogger();

    /**
     * Get the actual logger implementation.
     *
     * @return the severity log instance initialized
     */
    ISeverityLog& getLoggerImpl();

    /**
     * Lookup enum value of severity from string.
     *
     * @param serverityString The serverity string
     * @return The serverity enum value
     */
    static ISeverityLog::Severity severityValueOf(const std::string& severityString);

private:
    /**
     * Get the severity string from the enum equivalent.
     *
     * @param severity              the severity enum
     * @param serverityString       the corresponding string
     * @return true if there exists a string representation for the severity and false otherwise
     */
    static bool getSeverityString(SeverityLog::Severity severity, std::string& serverityString);

    void initModuleSeverityLog(const std::string& name);

private:

    ISeverityLog* m_pLogger;  //!< Pointer pointing to the application logger
    SeverityLog m_severityLog;  //!< Underlying logger implementation
    std::unique_ptr<ISeverityLog> m_pLogDecorator;  //!< Decorator for the severityLog

    std::vector<std::shared_ptr<FileSink>> m_fileSinkVector;
};

inline dep::ISeverityLog& ApplicationLog::getLogger() {
    return *(m_pLogger);
}

inline dep::ISeverityLog& ApplicationLog::getLoggerImpl() {
    return m_severityLog;
}

} /* namespace dep */

#endif /* APPLICATIONLOG_H_ */
