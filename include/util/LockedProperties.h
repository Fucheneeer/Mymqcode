////////////////////////////////////////////////////////////////////////////////
//
// LockedProperties.h
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
#pragma once

#include "Properties.h"
#include "ILock.h"
#include "SpinLock.h"

namespace lightmq { namespace util {

class LockedProperties : public ILock
{
public:
    LockedProperties() = default;
    ~LockedProperties() = default;

    virtual bool try_lock() override {return m_oplock.try_lock();};
    virtual void lock() override {return m_oplock.lock();};
    virtual void unlock() override {return m_oplock.unlock();};

    // lock
    std::string dump() const;
    std::string dump(const std::set<std::string>& exclusions) const;
    bool getProperty(const std::string& key, std::string& value) const;
    void getPropertiesVector(std::vector< std::pair<std::string, std::string> >& mapVector) const;
    bool setProperty(const std::string& line);
    bool setProperty(std::string key, std::string value);
    void merge(const dep::Properties& rhs);

    bool load(const std::string& propertiesFileName);
    void store(const std::string& propertiesFile) const;

    dep::Properties& getProperties(); 
    const dep::Properties& getProperties() const; 
private:
    
    dep::SpinLock m_oplock;
    dep::Properties m_prop; 
};



}}
