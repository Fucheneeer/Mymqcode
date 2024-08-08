////////////////////////////////////////////////////////////////////////////////
//
// InternalEventService.h
//
// Copyright (c) 2020 CFETSIT.
// Building 3, 491 YinDong Road, Shanghai, China.
// All rights reserved.
//
// LightMQ is the copyrighted,
// proprietary property of CFETSIT which retain all right, title and interest therein.
//
// Description: Header file of InternalEventService
//
////////////////////////////////////////////////////////////////////////////////

#ifndef INTERNALEVENTSERVICE_H_
#define INTERNALEVENTSERVICE_H_

#include <memory>
#include <unordered_map>
#include <vector>

#include "IInternalEventService.h"
#include "SelectableQueue.h"
#include "Uncopyable.h"

namespace dep {

class ISelector;

/**
 * Default implementation of IInternalEventService.
 */
class InternalEventService : public IInternalEventService, private Uncopyable {
public:
    /**
     * Constructor.
     *
     * @param selector The selector
     */
    explicit InternalEventService(ISelector& selector);

    /**
     * Destructor.
     */
    ~InternalEventService();

public:
    bool publishInternalEvent(std::shared_ptr<IInternalEvent> pInternalEvent) override;

    void addInternalEventHandler(const uint32_t eventType, IInternalEventHandler& handler) override;

    void removeInternalEventHandler(const uint32_t eventType, IInternalEventHandler& handler) override;

private:
    void onInternalEvent(std::shared_ptr<IInternalEvent> pInternalEvent);

    bool isValidHandler(const uint32_t eventType, IInternalEventHandler* pHandler) const;

private:
    ISelector& m_selector;
    SelectableQueue<std::shared_ptr<IInternalEvent>> m_internalEventQueue{100};
    std::unordered_map<uint32_t, std::vector<IInternalEventService::IInternalEventHandler*>> m_handlers;
};

}  // namespace dep

#endif /* INTERNALEVENTSERVICE_H_ */
