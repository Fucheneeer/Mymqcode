////////////////////////////////////////////////////////////////////////////////
//
// UpdateConfig.h
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

#include <cstdint>
#include <string>
#include <vector>
#include <map>

#include "BrokerProtocolConstant.h"
#include "IMessage.h"

namespace lightmq { namespace protocol {

class MsgUpdateConfigHA : public protocol::IMessage  
{
public:
    MsgUpdateConfigHA() = default;
    ~MsgUpdateConfigHA() = default;

    void parse(dep::ByteBuffer& buffer) override;

    void writeTo(dep::ByteBuffer& buffer) override;

    void setRequestID(uint64_t r);
    uint64_t getRequestID() const;

    void setUpdateJson(const std::string &json);
    std::string getUpdateJson() const;

    std::string debugString() const;
    uint8_t getMessageType() override {return protocol::BrokerProtocolConstant::MESSAGE_TYPE_UPDATE_CONFIG_HA;};

private:
    uint64_t m_requestID;
    std::string m_updateJson;
    // std::string m_key;
    // std::string m_value;
};

class MsgUpdateConfigHAAck : public protocol::IMessage  
{
public:
    MsgUpdateConfigHAAck() = default;
    ~MsgUpdateConfigHAAck() = default;

    void parse(dep::ByteBuffer& buffer) override;
    void writeTo(dep::ByteBuffer& buffer) override;

    void setRequestID(uint64_t requestID);
    uint64_t getRequestID() const;
    void addResult(const std::string &key, int result);
    int getResult(const std::string &key) const;

    std::string debugString() const;
    uint8_t getMessageType() override {return protocol::BrokerProtocolConstant::MESSAGE_TYPE_UPDATE_CONFIG_HA_ACK;};

private:
    uint64_t m_requestID;
    // 每个key值的result. 默认OK
    std::map<std::string,int> m_result;
};

class MsgSyncAllConfigHA : public protocol::IMessage
{
public:
    MsgSyncAllConfigHA() = default;
    ~MsgSyncAllConfigHA() = default;

    void parse(dep::ByteBuffer& buffer) override;
    void writeTo(dep::ByteBuffer& buffer) override;

    void addConfig(const std::string &key, const std::string &value);
    void getConfigs(std::vector<std::pair<std::string, std::string> > &configs); 

    uint8_t getMessageType() override {return protocol::BrokerProtocolConstant::MESSAGE_TYPE_SYNC_ALL_CONFIG_HA;};

private:
    std::map<std::string, std::string> m_configs;
};


}}
