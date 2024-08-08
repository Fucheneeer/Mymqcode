////////////////////////////////////////////////////////////////////////////////
//
// IPluginModuleRepository.h
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

#ifndef IPLUGINMODULEREPOSITORY_H_
#define IPLUGINMODULEREPOSITORY_H_
namespace lightmq { namespace qmapi {

class IMessageFilterFactory;
class ICompressionFactory;
class ICryptoFactory;
class SnapshotEngineService;

/**
 * Repository to hold all plugin factory class
 */
class IPluginModuleRepository {
public:
    /**
     * Destructor
     */
    virtual ~IPluginModuleRepository() = default;

    /**
     * Get the filter module factory
     * @return the filter module factory
     */
    virtual IMessageFilterFactory* getFilterFactory() const = 0;

    /**
     * Get the compression factory
     * @return the compression factory
     */
    virtual ICompressionFactory* getCompressionFactory() const = 0;

    /**
     * Get the encryption factory
     * @return the encryption factory
     */
    virtual ICryptoFactory* getCryptoFactory() const = 0;

    /**
     * Get the snapshot engine service
     * @return snapshot engine service
     */
    virtual SnapshotEngineService* getSnapshotEngineService() const = 0;
};

}} // namespace lightmq { namespace qmapi {

#endif /* IPLUGINMODULEREPOSITORY_H_ */
