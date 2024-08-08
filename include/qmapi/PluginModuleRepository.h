////////////////////////////////////////////////////////////////////////////////
//
// PluginModuleRepository.h
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

#ifndef PLUGINMODULEREPOSITORY_H_
#define PLUGINMODULEREPOSITORY_H_

#include "IPluginModuleRepository.h"

#include "ModuleManager.h"

namespace dep {
    class ISeverityLog;
    class Properties;
}

namespace lightmq { namespace qmapi {

/**
 * Framework Plugin Repository
 */
class PluginModuleRepository : public IPluginModuleRepository {
public:
    /**
     * Constructor
     */
    PluginModuleRepository(const dep::Properties& properties, dep::ISeverityLog& logger);

    /**
     * Destructor
     */
    virtual ~PluginModuleRepository() = default;

    virtual IMessageFilterFactory* getFilterFactory() const override;

    virtual ICompressionFactory* getCompressionFactory() const override;

    virtual ICryptoFactory* getCryptoFactory() const override;

    virtual SnapshotEngineService* getSnapshotEngineService() const override;

public:
    void setSnapshotEngineService(SnapshotEngineService* snapshotEngineService);

private:
    dep::ModuleManager<IMessageFilterFactory> m_filterManager;
    dep::ModuleManager<ICompressionFactory> m_compressionManager;
    dep::ModuleManager<ICryptoFactory> m_cryptoManager;
    IMessageFilterFactory* m_filterFactory {nullptr};
    ICompressionFactory* m_compressionFactory {nullptr};
    ICryptoFactory* m_cryptoFactory {nullptr};

    SnapshotEngineService* m_snapshotEngineService {nullptr};
};

}} // namespace lightmq { namespace qmapi {

#endif /* PLUGINMODULEREPOSITORY_H_ */
