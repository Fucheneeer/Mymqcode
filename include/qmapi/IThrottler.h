////////////////////////////////////////////////////////////////////////////////
//
// IThrottler.h
//
// Copyright (c) 2020 CFETSIT.
// Building 3, 491 YinDong Road, Shanghai, China.
// All rights reserved.
//
// LightMQ is the copyrighted,
// proprietary property of CFETSIT which retain all right, title and interest therein.
//
// Description: Header file of IThrottler
//
////////////////////////////////////////////////////////////////////////////////

#ifndef ITHROTTLER_H_
#define ITHROTTLER_H_

//#include "DEPMSGType.h"

namespace lightmq { namespace qmapi {

/**
 * Bandwidth throttling mechanism.
 *
 * \code{.cpp}
 *     IThrottler pThrottler = ...;
 *     dep::ByteBuffer message = ...;
 *
 *     // Setup bandwidth reservation parameters
 *     BandwidthParameter param;
 *     param.channelID = m_channelID;
 *     param.streamID = m_streamID;
 *     param.numberOfMessages = 1;
 *     param.totalMessageSize = message.remaining();
 *
 *     // Reserve bandwdith
 *     if (!pThrottler->reserveBandwidth(param)) {
 *         // Insufficient bandwidth
 *         return;
 *     }
 *
 *     // Can proceed to send message
 *     bool sendMessageSuccess = .....;
 *
 *     // Must invoke returnBandwidth after reservation
 *     // In case when it failed to send message, previously
 *     // reserved bandwidth will be return back to throttler
 *     pThrottler->commitBandwidth(sendMessageSuccess, param);
 * \endcode
 */
class IThrottler {
public:
    /**
     * Bandwidth reservation parameters.
     */
//    struct BandwidthParameter {
//        BandwidthParameter()
//            : channelID {0}
//            , streamID {0}
//            , numberOfMessages {0}
//            , totalMessageSize {0} {
//        }
//
//        ChannelID channelID; /**< Channel ID*/
//        StreamID streamID; /**< Stream ID */
//        uint32_t numberOfMessages; /**< Number of messages to reserve */
//        uint32_t totalMessageSize; /**< Total size of messages in bytes */
//    };

    struct BandwidthParameter {
        BandwidthParameter()
            : destinationName {""}
            , numberOfMessages {0}
            , totalMessageSize {0} {
        }

        std::string destinationName;
        uint32_t numberOfMessages; /**< Number of messages to reserve */
        uint32_t totalMessageSize; /**< Total size of messages in bytes */
    };


public:
    /**
     * Destructor
     */
    virtual ~IThrottler() = default;

public:
    /**
     * Reserve bandwidth according to given parameters.
     *
     * @param bandwidthParam Reservation parameters
     * @return true if bandwidth is reserved successfully
     */
    virtual bool reserveBandwidth(const BandwidthParameter& bandwidthParam) = 0;

    /**
     * Commit bandwidth. Must be invoked after reserveBandwidth().
     *
     * @param bandwidthConsumed Whether previously reserved bandwidth is consumed
     * @param bandwidthParam Reservation parameters
     */
    virtual void commitBandwidth(bool bandwidthConsumed, const BandwidthParameter& bandwidthParam) = 0;
};

}} // namespace lightmq { namespace qmapi {

#endif /* ITHROTTLER_H_ */
