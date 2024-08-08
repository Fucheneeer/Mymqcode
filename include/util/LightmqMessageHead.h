////////////////////////////////////////////////////////////////////////////////
//
// LightmqMessageHead.h
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

#include <string>
#include <vector>
#include <functional>
#include <map>
#include "ByteBuffer.h"
#include "VariantMap.h"
#include "CommonConstant.h"


namespace lightmq { namespace util {


	class LightmqMessageHead {
	public:
		//using MessageHead = uint32_t;
		//static constexpr int HEAD_SEGMENT_SIZE = sizeof(uint32_t);

        enum ProcessFlag
        {
            EndBit = 0,
            ENCRYPT = 1, 
            DECRYPT,
            COMPRESS,
            EXTRACT,
        };

        using Type = uint8_t;
        static constexpr Type PREFIX_EMPTY = 'e';
        static constexpr Type PREFIX_STRING = 'S';
        static constexpr Type PREFIX_BYTEBUFFER = 'B';
        static constexpr Type PREFIX_MAP = 'M';
        static constexpr Type PREFIX_STREAM = 's';
        static constexpr Type PREFIX_OBJECT = 'O';

	public:
		LightmqMessageHead() ;
		LightmqMessageHead(const LightmqMessageHead &o);
		// LightmqMessageHead(LightmqMessageHead &o);
		LightmqMessageHead(LightmqMessageHead &&o);
		virtual ~LightmqMessageHead() = default;

		void parseAll(dep::ByteBuffer& input);
		void parseOnBroker(dep::ByteBuffer& input, int32_t headStartPos, bool moveInputPos = false);
        // parse from clientID.
		void parseProperties(dep::ByteBuffer& input, int32_t headStartPos, bool moveInputPos);
        static void skip(dep::ByteBuffer& input);

		size_t writeTo(dep::ByteBuffer& buffer) const;

		void setMessageType(uint8_t messageType);
		uint8_t getMessageType() const;

        void addProcessFlag(uint8_t flag);

        const std::vector<int32_t>& getProcessFlags() const;
        static int32_t oppositeFlag(int32_t flag);
        void copyFrom(const LightmqMessageHead &srcHead);
        // only copy application data
        void copyAppdataFrom(const LightmqMessageHead &srcHead);
        void clear();
        VariantMap& getVariantMap();
        const VariantMap& getVariantMap() const;
		bool isMessageCompressed() const;
		bool isMessageEncrypted() const;

        std::string toString() const;
		std::string toStringPretty() const;
		bool isMessageCompressed();
		bool isMessageEncrypted();


        // property
        bool checkPropertiesLimits() const;
        bool checkPropertyKeyLimits(const std::string& key) const;
        
        uint8_t addBooleanProperty(const std::string &key, const bool &v);
        uint8_t addByteProperty(const std::string &key, const int8_t &v);
        uint8_t addShortProperty(const std::string &key, const int16_t &v);
        uint8_t addIntegerProperty(const std::string &key, const int32_t &v);
        uint8_t addLongProperty(const std::string &key, const int64_t &v);
        uint8_t addFloatProperty(const std::string &key, const float &v);
        uint8_t addDoubleProperty(const std::string &key, const double &v);
        uint8_t addStringProperty(const std::string &key, const std::string &v);
        uint8_t addVariantProperty(const std::string &key, const MessagePropertyVariant &v);
       
        bool getBooleanProperty(const std::string &key, bool &v) const;
        bool getByteProperty(const std::string &key, int8_t &v) const;
        bool getShortProperty(const std::string &key, int16_t &v) const;
        bool getIntegerProperty(const std::string &key, int32_t &v) const;
        bool getLongProperty(const std::string &key, int64_t &v) const;
        bool getFloatProperty(const std::string &key, float &v) const;
        bool getDoubleProperty(const std::string &key, double &v) const;
        bool getStringProperty(const std::string &key, std::string &v) const;
        const MessagePropertyVariant* getVariantProperty(const std::string &key) const;
        void propertyKeys(std::vector<std::string> &keys) const;
        void clearProperties();
        bool propertyExist(const std::string &key) const;
        void removeProperty(const std::string &key);

        typedef void(LightmqMessageHead::*UnPackFunc)(dep::ByteBuffer& input); 
        void parseInternal(dep::ByteBuffer& input, int32_t headStartPos, bool moveInputPos, UnPackFunc unpackf);
        void parseAllFunc(dep::ByteBuffer& input);
        void parsePropertiesFunc(dep::ByteBuffer& input);
        void parseBrokerFunc(dep::ByteBuffer& input);

        // JMS prop.
        uint8_t setJMSMessageID(const std::string &id);
        const std::string& getJMSMessageID() const;
        void setJMSTimestamp(const uint64_t timestamp);
        uint64_t getJMSTimestamp() const;
        uint8_t setJMSCorrelationID(const std::string &id);
        const std::string& getJMSCorrelationID() const;
        uint8_t setJMSReplyTo(const std::string &to);
        const std::string& getJMSReplyTo() const;

        uint8_t setJMSDestination(const std::string &dest);
        const std::string& getJMSDestination() const;
        uint8_t setDeliveryMode(int32_t deliveryMode);
        int32_t getDeliveryMode() const;
        void setJMSRedelivered(bool redelivered);
        bool getJMSRedelivered() const;
        uint8_t setJMSType(const std::string &tp);
        const std::string& getJMSType() const;
        void setJMSExpiration(uint64_t expiration);
        uint64_t getJMSExpiration() const;
        void setJMSDeliveryTime(uint64_t deliveryTime);
        uint64_t getJMSDeliveryTime() const;
        uint8_t setPriority(int32_t pri);
        int32_t getPriority() const;

	private:
		uint8_t m_messageType{ 0 };
        std::vector<int32_t> m_processTags;
        VariantMap m_variantMap;
        //int64_t m_sendTime;
        //std::string m_transactionSessionID;
        //int64_t m_transactionID;
        //int64_t m_expirationMs;
        //int8_t m_priority;

        std::string m_messageID;
        uint64_t m_timestamp{ 0 };
        std::string m_correlationID;
        int32_t m_deliveryMode{ 0 };
        bool m_redelivered{ false };
        std::string m_jmsType;
        uint64_t m_expiration{ 0 };
        uint64_t m_deliveryTime{ 0 };
        int32_t m_priority{ util::CommonConstant::DEFAULT_MESSAGE_LEVEL };
        std::string m_replyTo;
        std::string m_jmsDestination;
	};

}} // namespace lightmq { namespace util {

