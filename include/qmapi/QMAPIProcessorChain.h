////////////////////////////////////////////////////////////////////////////////
//
// QMAPIProcessorChain.h
//
// Copyright (c) 2020 CFETSIT.
// Building 3, 491 YinDong Road, Shanghai, China.
// All rights reserved.
//
// LightMQ is the copyrighted,
// proprietary property of CFETSIT which retain all right, title and interest therein.
//
// Description: Header file of ProcessorChain
//
////////////////////////////////////////////////////////////////////////////////

#ifndef PROCESSORCHAIN_H_
#define PROCESSORCHAIN_H_

#include <memory>
#include <vector>

#include "IChainProcessor.h"
#include "Uncopyable.h"


namespace dep {
    class ISeverityLog;
}



namespace lightmq { namespace qmapi {

class BaseProcessorAttribute;
class IApplicationMessageListener;
class IPluginModuleRepository;
class IHADataSerializer;

/**
 * Configuration to create processor chain, includes
 * - Plugin repository
 * - Severity log
 */
class QMAPIProcessorChainConfiguration {

public:
    /**
     * Constructor
     * @param repository    Plugin repository
     * @param logger        Severity log
     */
    QMAPIProcessorChainConfiguration(IPluginModuleRepository& repository, dep::ISeverityLog& logger)
        : m_repository(repository)
        , m_logger(logger)  {
    }

    /**
     * Get the severity log
     * @return
     */
    dep::ISeverityLog& getLogger() const {
        return m_logger;
    }

    /**
     * Get the plugin repository
     * @return
     */
    const IPluginModuleRepository& getRepository() const {
        return m_repository;
    }

private:
    IPluginModuleRepository& m_repository;
    dep::ISeverityLog& m_logger;

};

/**
 * Collection of chain processor.
 */
class QMAPIProcessorChain : private dep::Uncopyable {

public:
    /**
     * Constructor.
     *
     * @param attributes The configuration that control the members in processor chain.
     * @param configuration The set of class that required to create the processor
     */
    explicit QMAPIProcessorChain(const std::vector<std::reference_wrapper<BaseProcessorAttribute>>& attributes,
                            QMAPIProcessorChainConfiguration& configuration);

public:
    /**
     * Process a message.
     *
     * @param processorInfo The processor info
     * @param message The message
     */
    void process(IChainProcessor::ProcessorInfo& processorInfo, InternalLightmqMessage &imessage);

    /**
     * Add a chain process to this chain as the first prcessor
     * @param pProcessor The chain processor
     */
    void addFirst(std::unique_ptr<IChainProcessor> pProcessor);

    /**
     * Add a chain process to this chain.
     *
     * @param pProcessor The chain processor
     */
    void addLast(std::unique_ptr<IChainProcessor> pProcessor);

    uint32_t processorCount() const;

private:
    std::vector<std::unique_ptr<IChainProcessor>> m_processors;
};

}} // namespace lightmq { namespace qmapi {

#endif /* PROCESSORCHAIN_H_ */
