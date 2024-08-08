////////////////////////////////////////////////////////////////////////////////
//
// SessionScheduleUtility.h
//
// Copyright (c) 2020 CFETSIT.
// Building 3, 491 YinDong Road, Shanghai, China.
// All rights reserved.
//
// LightMQ is the copyrighted,
// proprietary property of CFETSIT which retain all right, title and interest therein.
//
// Description: Header file of SessionScheduleUtility
//
////////////////////////////////////////////////////////////////////////////////

#ifndef SESSIONSCHEDULEUTILITY_H_
#define SESSIONSCHEDULEUTILITY_H_

#include <chrono>
#include <string>
#include <unordered_set>
#include <memory>
#include <mutex>

#include "LocalTime.h"
#include "BrokerTypes.h"
#include "IClockSource.h"
#include "ISessionScheduleUtility.h"
#include "ITimer.h"

namespace dep{
    class LocalTime;
    class ISeverityLog;
}

namespace lightmq { namespace util {

/**
 * Calculate session reset schedule.
 */
class SessionScheduleUtility : public ISessionScheduleUtility {
public:
    using TimePoint = std::chrono::system_clock::time_point; /**< Point in time */


//     enum MODE {
//         DAY_MODE,
//         HOUR_MODE,
//         MINUTE_MODE,
//     };

    static const std::string SESSION_ID_FORMAT_HOUR_MODE ;
    static const std::string SESSION_ID_FORMAT_DAY_MODE;
    static const std::string SESSION_ID_FORMAT_MINUTE_MODE;


public:
    SessionScheduleUtility(dep::ISeverityLog* pLogger, dep::ITimer* pTimer, IClockSource* pClockSource, MODE modeToUse, int32_t validSessionSize);
    ~SessionScheduleUtility();

    void stopTimer() override;
    void startTimer() override;

    /**
     * Get currentSessionID
     * @return currentSessionID
     */
    SessionID getCurrentSessionID() override;

    /**
     * Get current and oldest valid sessionID
     * @return
     */
    void getSessionIDRange(SessionID& oldest, SessionID& current) override;
    /**
     * register session change Listener
     * @param pListener
     * @return currentSessionID (for transaction behavior)
     */
    SessionID registerSessionChangeListener(ISessionChangeListener* pListener) override;

    /**
     * Unregister session change listener
     * @param pListener
     */
    void unregisterSessionChangeListener(ISessionChangeListener* pListener) override;
    /**
     * Calculate session ID of next session.
     *
     * @param sessionID The current session ID
     * @return difference How many session forward, negative number is used for previous session
     */
    SessionID nextSessionID(const SessionID sessionID, int32_t difference) override;

    /**
     * check is the sessionID already expired
     * @param sessionID
     * @return
     */
    bool isSessionExpired(SessionID sessionID) override;

    bool onSessionChange(SessionID oldestValidSessionID, SessionID latestSessionID) override;

    const MODE getMode() override { return m_mode; }
private:
    void onSessionchange(dep::ITimer::TimerID timerID);

    SessionID lastSessionID(const TimePoint& currentTime);

    static const std::string& getFormatString(MODE mode);

    static const std::chrono::minutes getOneSessionTimeDifferent(MODE mode);

    dep::ISeverityLog* m_pLogger;
    dep::ITimer* m_pTimer;
    std::unique_ptr<IClockSource> m_pClockSource;
    MODE m_mode;
    dep::ITimer::TimerID m_timerID;
    SessionID m_currentSessionID;
  //  SessionID m_nextSessionID;
    std::unordered_set<ISessionChangeListener*> m_sessionChangeListenerSet;
	std::recursive_mutex m_lock;
	std::recursive_mutex m_nextSessionIDLock; //多线程调用nextSessionID（）会死锁
    int32_t m_validSessionSize;
    SessionID m_oldestValidSessionID;
    bool m_timerStarted{false};
};


}} // namespace lightmq { namespace util {


#endif /* SESSIONSCHEDULEUTILITY_H_ */
