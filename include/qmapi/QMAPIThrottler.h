////////////////////////////////////////////////////////////////////////////////
//
// QMAPIThrottler.h
//
// Copyright (c) 2020 CFETSIT.
// Building 3, 491 YinDong Road, Shanghai, China.
// All rights reserved.
//
// LightMQ is the copyrighted,
// proprietary property of CFETSIT which retain all right, title and interest therein.
//
// Description: Header file of PeriodicRechargeTokenThrottler
//
////////////////////////////////////////////////////////////////////////////////

#ifndef QMAPITHROTTLER_H_
#define QMAPITHROTTLER_H_

#include <atomic>
#include <unordered_set>
#include <condition_variable>
#include "IThrottler.h"
#include "platform.h"

namespace lightmq { namespace qmapi {

class QMAPIThrottler : public IThrottler {
public:
    /**
     * Configuration parameters for QMAPIThrottler
     */
    struct ThrottlerParam {
        ThrottlerParam()
            : rechargeCap {0} {
        }

    uint32_t rechargeCap; /**< Max token capacity via recharge. If rechargeCap is 100, Token will not be recharged when it already has 100 token. */
    };

public:
    /**
     * Constructor
     */
    explicit QMAPIThrottler(const ThrottlerParam& throttleParam);

    /**
     * Destructor
     */
    ~QMAPIThrottler() = default;

    /**
     * Disable copy.
     */
    QMAPIThrottler(const QMAPIThrottler&) = delete;


public:
    bool reserveBandwidth(const BandwidthParameter& bandwidthParam) override;

    void commitBandwidth(bool bandwidthConsumed, const BandwidthParameter& bandwidthParam) override;

    void recharge(uint32_t tokenCount);


private:
    uint32_t m_rechargeCap;
    std::atomic<uint32_t> m_tokenRemaining {0};

private:
    inline uint32_t getCurrentToken() const {
        return m_tokenRemaining.load(MEMORY_ORDER);
    }

    inline uint32_t getTokenRequired(const BandwidthParameter& bandwidthParam) {
        return bandwidthParam.numberOfMessages;
    }

};

}} // namespace lightmq { namespace qmapi {

#endif /* QMAPITHROTTLER_H_ */
