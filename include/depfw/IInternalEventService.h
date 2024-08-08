////////////////////////////////////////////////////////////////////////////////
//
// IInternalEventService.h
//
// Copyright (c) 2020 CFETSIT.
// Building 3, 491 YinDong Road, Shanghai, China.
// All rights reserved.
//
// LightMQ is the copyrighted,
// proprietary property of CFETSIT which retain all right, title and interest therein.
//
// Description: Header file of IInternalEventService
//
////////////////////////////////////////////////////////////////////////////////

#ifndef IINTERNALEVENTSERVICE_H_
#define IINTERNALEVENTSERVICE_H_

#include <chrono>
#include <condition_variable>
#include <memory>
#include <mutex>

namespace dep {

/**
 * Provider internal asynchronous communication service.
 *
 * All event must extend IInternalEvent. Each event subclass should have an unique event type.
 * Event type is a 32 bit unsigned integer that combine EventCategory (16 bit) and a local event type (16 bit).
 *
 * To add a new event:
 *
 * 1. Create a new IInternalEvent::EventCategory
 * 2. Create an abstract class (e.g. class T) that extends IInternalEvent
 * 3. Define enum of EventType inside T. The value of EventType should be unique across all subclass of IInternalEvent
 * 4. Declare all necessary specific event class to extends T
 * 5. Implement IInternalEventHandler
 * 6. Invoke IInternalEventService::addInternalEventHandler() for all interested events during startup
 * 7. Invoke IInternalEventService::removeInternalEventHandler() for all interested events during startup
 */
class IInternalEventService {
public:
    class InternalEventType;
    class IInternalEvent;
    class OnCompleteNotifiableEvent;
    class NotifyEventCompleteHelper;
    class IInternalEventHandler;

public:
    /**
     * Destructor.
     */
    virtual ~IInternalEventService() = default;

public:
    /**
     * Publish an event.
     *
     * @param pInternalEvent The event
     * @return true on success
     */
    virtual bool publishInternalEvent(std::shared_ptr<IInternalEvent> pInternalEvent) = 0;

    /**
     * Add event handler of given event type.
     *
     * @param eventType The event type
     * @param handler The handler
     */
    virtual void addInternalEventHandler(const uint32_t eventType, IInternalEventHandler& handler) = 0;

    /**
     * Remove event handler of given given type
     *
     * @param eventType The event type
     * @param handler The handler
     */
    virtual void removeInternalEventHandler(const uint32_t eventType, IInternalEventHandler& handler) = 0;
};

/**
 * Base event class for all internal event.
 */
class IInternalEventService::IInternalEvent {
protected:
    /**
     * Category of event.
     * Purpose is to ensure uniqueness of event type.
     */
    enum EventCategory : uint16_t {
        BASE_APPLICATION,//!< BASE_APPLICATION
        CORE_APPLICATION,//!< CORE_APPLICATION
        DEPAPI,          //!< DEPAPI
        MESSAGE_AGENT    //!< MESSAGE_AGENT
    };

    /**
     * Combine a category and a local event type to a unique event type.
     *
     * @param category The event category
     * @param eventType The local event type
     * @return Unique event type
     */
    static constexpr uint32_t combine(const EventCategory category, const uint16_t eventType) {
        return (category << 16) | eventType;
    }

public:
    /**
     * Destructor
     */
    virtual ~IInternalEvent() = default;

public:
    /**
     * Get event type.
     *
     * @return The event type
     */
    virtual uint32_t getEventType() const = 0;
};

/**
 * @brief Internal event that allow producer thread to wait for complete
 *        processing of this event.
 *
 *  > Please make sure that the event producer and consumer are of different
 *  > thread otherwise it will result in deadlock.
 */
class IInternalEventService::OnCompleteNotifiableEvent : public IInternalEvent {
public:
    /**
     * Constructor
     */
    OnCompleteNotifiableEvent() = default;

    /**
     * Destructor
     */
    virtual ~OnCompleteNotifiableEvent() = default;

public:
    /**
     * @brief Should be invoked by event consumer thread to mark this event as completed.
     */
    void notifyComplete() {
        std::lock_guard<std::mutex> lock {m_lock};
        m_completed = true;
        m_completeCondition.notify_all();
    }

    /**
     * @brief Wait until this event is completed.
     */
    void awaitComplete() {
        std::unique_lock<std::mutex> lock {m_lock};
        m_completeCondition.wait(lock, [this]() {return isComplete();});
    }

    /**
     * @brief Wait until this event is completed or timeout.
     *
     * @param timeout The timeout to wait
     * @return true if this event is completed.
     */
    template<typename Rep, typename Period>
    bool awaitCompleteFor(const std::chrono::duration<Rep, Period>& timeout) {
        std::unique_lock<std::mutex> lock {m_lock};
        return m_completeCondition.wait_for(lock, timeout, [this]() { return isComplete(); });
    }

    /**
     * Whether this event is complete.
     *
     * @return true if this event is completed.
     */
    bool isComplete() const {
        return m_completed;
    }

private:
    std::mutex m_lock;
    std::condition_variable m_completeCondition;
    bool m_completed {false};
};

/**
 * Help to make sure OnCompleteNotifiableEvent::notifyComplete() is invoked.
 */
class IInternalEventService::NotifyEventCompleteHelper {
public:
    /**
     * Constructor
     *
     * @param event The Event to keep track of
     */
    explicit NotifyEventCompleteHelper(OnCompleteNotifiableEvent& event)
        : m_event(event) {
    }

    /**
     * Destructor to invoke OnCompleteNotifiableEvent::notifyComplete().
     */
    ~NotifyEventCompleteHelper() {
        m_event.notifyComplete();
    }

private:
    OnCompleteNotifiableEvent& m_event;
};

/**
 * Internal event handler.
 */
class IInternalEventService::IInternalEventHandler {
public:
    /**
     * Destructor.
     */
    virtual ~IInternalEventHandler() = default;

public:
    /**
     * Invoked when an internal event arrived.
     *
     * @param pInternalEvent The internal event
     */
    virtual void onInternalEvent(std::shared_ptr<IInternalEvent> pInternalEvent) = 0;
};

}  // namespace dep

#endif /* IINTERNALEVENTSERVICE_H_ */
