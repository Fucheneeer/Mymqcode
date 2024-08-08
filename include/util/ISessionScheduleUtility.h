////////////////////////////////////////////////////////////////////////////////
//
// ISessionScheduleUtility.h
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

#ifndef ISESSIONSCHEDULEUTILITY_H_
#define ISESSIONSCHEDULEUTILITY_H_



namespace lightmq { namespace util {

class ISessionChangeListener {
public:
    ISessionChangeListener()=default;
    virtual ~ISessionChangeListener()=default;

    virtual void onSessionChanged(SessionID oldestValidSessionID, SessionID newSessionID)=0;

};

class ISessionScheduleUtility {
public:
    enum MODE {
        DAY_MODE,
        HOUR_MODE,
        MINUTE_MODE,
    };
    ISessionScheduleUtility() = default;
    virtual ~ISessionScheduleUtility() = default;

    virtual SessionID getCurrentSessionID() = 0;
    virtual void getSessionIDRange(SessionID& oldest, SessionID& current) = 0;
    virtual SessionID registerSessionChangeListener(ISessionChangeListener* pListener) = 0;
    virtual void unregisterSessionChangeListener(ISessionChangeListener* pListener) = 0;
    virtual SessionID nextSessionID(const SessionID sessionID, int32_t difference) = 0;
    virtual bool isSessionExpired(SessionID sessionID) = 0;
    virtual bool onSessionChange(SessionID oldestValidSessionID, SessionID latestSessionID) = 0;
    virtual void stopTimer() = 0;
    virtual void startTimer() = 0;
    virtual const MODE getMode() = 0;
};

}} // namespace lightmq { namespace util {

#endif /* ISESSIONSCHEDULEUTILITY_H_ */
