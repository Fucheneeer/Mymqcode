////////////////////////////////////////////////////////////////////////////////
//
// IChainProcessor.h
//
// Copyright (c) 2020 CFETSIT.
// Building 3, 491 YinDong Road, Shanghai, China.
// All rights reserved.
//
// LightMQ is the copyrighted,
// proprietary property of CFETSIT which retain all right, title and interest therein.
//
// Description: Header of IChainProcessor
//
////////////////////////////////////////////////////////////////////////////////

#ifndef ICHAINPROCESSOR_H_
#define ICHAINPROCESSOR_H_

#include <string>

#include "LightmqMessageHead.h"

namespace lightmq { namespace qmapi {

// class dep::ByteBuffer;
class InternalLightmqMessage;
/**
 * Interface of chain processor
 */
class IChainProcessor {
public:
    /**
     * The type of process result
     */
    enum class ProcessResult {
        SUCCESS,                //!< SUCCESS
        UNKNOWN,                //!< UNKNOWN
        COMPRESSION_FAILURE,    //!< COMPRESSION_FAILURE
        ENCRYPTION_FAILURE,     //!< ENCRYPTION_FAILURE
        UMA_SEND_FAILURE,       //!< UMA_SEND_FAILURE
        MESSAGE_SIZE_EXCEED,    //!< Message size exceed maxMsgSize
        INSTANCE_NAME_EMPTY,    //!< Instance name is empty when sending unicast message
        INSTANCE_NAME_TOO_LONG, //!< Instance name exceed limit
        SERVICE_NOT_READY,      //!< Service registration or subscription not yet completed
        OUT_OF_BANDWIDTH,       //!< Throttled
        REPLY_TIMEOUT,          //!< Reply timeout
        MESSAGE_PERSIST_FAILURE, //!< Message persist failure
        SEQ_NOT_READY,           //!< Channel sequence not synchronized
        BUFFER_IS_FULL          //!<  BUFFER IS FULL
    };

    /**
     * Struct of processor info
     */
    class ProcessorInfo {
    public:
        const std::string* pChannelName{nullptr}; /**< Channel name */
        const std::string* pTargetInstance{nullptr}; /**< Instance name for unicast message */
        const std::string* pSnapshotKey{nullptr};  /**< Snapshot key for the message */
        IChainProcessor* pLastProcessor{nullptr}; /**< The chain processor to forward at last. Used by RouterChainProcessor */
		//DEPMessageHeader* pDEPMessageHeader{ nullptr };  /**< DEP message header*/
        util::LightmqMessageHead* pMessageHead{ nullptr };  /**< LIGHTMQ message header*/
        ProcessResult processResult{ProcessResult::UNKNOWN};        /**< Process result*/
    };

public:
    /**
     * Destructor
     */
    virtual ~IChainProcessor() = default;

    /**
     * Set the next chain processor
     *
     * @param pNextProcessor    The next processor
     * @return  The next processor
     */
    virtual IChainProcessor* setNext(IChainProcessor* pNextProcessor) = 0;

    /**
     * Process the message
     *
     * @param processorInfo     Info of processor
     * @param message           Message Buffer
     */
    virtual void process(ProcessorInfo& processorInfo, InternalLightmqMessage &imessage) = 0;
};

}} // namespace lightmq { namespace qmapi {

#endif /* ICHAINPROCESSOR_H_ */
