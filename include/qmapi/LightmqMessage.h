////////////////////////////////////////////////////////////////////////////////
//
// LightmqMessage.h
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

#ifndef LIGHTMQMESSAGE_H_
#define LIGHTMQMESSAGE_H_
#include <functional>
#include <memory>
#include "ByteBuffer.h"
#include "LightmqMessageHead.h"
#include "BrokerTypes.h"
#include "VariantMap.h"

namespace lightmq { namespace qmapi {

using lightmq::util::MessagePropertyVariant ;
using lightmq::util::LightmqMessageHead;
class IMessageConsumer;
class InternalLightmqMessage;

class LightmqMessage {
public:
    using Type = uint8_t;
    friend class InternalLightmqMessage;

    static constexpr uint32_t PREFIX_POS = 0;
    static constexpr uint32_t PREFIX_SIZE = 1;

    static constexpr Type PREFIX_EMPTY = LightmqMessageHead::PREFIX_EMPTY;
    static constexpr Type PREFIX_STRING = LightmqMessageHead::PREFIX_STRING;
    static constexpr Type PREFIX_BYTEBUFFER = LightmqMessageHead::PREFIX_BYTEBUFFER;
    static constexpr Type PREFIX_MAP = LightmqMessageHead::PREFIX_MAP;
    static constexpr Type PREFIX_STREAM = LightmqMessageHead::PREFIX_STREAM;
    static constexpr Type PREFIX_OBJECT = LightmqMessageHead::PREFIX_OBJECT;

public:
    virtual ~LightmqMessage() = default;

    LightmqMessage(const LightmqMessage&);    // copy constructor
    LightmqMessage(LightmqMessage&&);    // move constructor
	//LightmqMessage& operator=(const LightmqMessage& o);
	LightmqMessage& operator=(LightmqMessage&& o);

    LightmqMessage();

    /**
     * Get LightmqMessage message type
     *
     * @return PREFIX_EMTPY, PREFIX_STRING or PREFIX_BYTEBUFFER
     *
     */
    Type getType() const;
    void setType(Type tp);

    /**
     * Set the message body only, not change messageType.
     *
     * @return 
     *
     */
    void setBody(const dep::ByteBuffer& buffer); 

    /**
     * Set the message body of dep::ByteBuffer type
     *
     * @return true if the operation is successful
     *
     */
    bool setByteBuffer(const dep::ByteBuffer& buffer);
	bool setByteBuffer(std::function<void(dep::ByteBuffer& buffer)> &&f);

    /**
     * Set the message body of String type
     *
     * @return true if the operation is successful
     *
     */
    bool setString(const std::string& buffer);

    /**
     * Get the message body of dep::ByteBuffer type
     *
     * @return true if the operation is successful
     *
     */
    bool getByteBuffer(dep::ByteBuffer& buffer) const;

    /**
     * Get the message body of String type
     *
     * @return true if the operation is successful
     *
     */
    bool getString(std::string& buffer) const;

    bool checkAndSetMapContent();
    bool addBooleanContent(const std::string &key, const bool &v);
    bool addByteContent(const std::string &key, const int8_t &v);
    bool addShortContent(const std::string &key, const int16_t &v);
    bool addUniCodeCharContent(const std::string& key, const uint16_t& v);
    bool addIntegerContent(const std::string& key, const int32_t& v);
    bool addLongContent(const std::string &key, const int64_t &v);
    bool addFloatContent(const std::string &key, const float &v);
    bool addDoubleContent(const std::string &key, const double &v);
    bool addStringContent(const std::string &key, const std::string &v);
    bool addBytesContent(const std::string& key, const int8_t* pBytes, int length);
    bool addVariantContent(const std::string &key, const MessagePropertyVariant &v);

    bool getBooleanContent(const std::string &key, bool &v) const;
    bool getByteContent(const std::string &key, int8_t &v) const;
    bool getShortContent(const std::string &key, int16_t &v) const;
    bool getIntegerContent(const std::string &key, int32_t &v) const;
    bool getLongContent(const std::string &key, int64_t &v) const;
    bool getFloatContent(const std::string &key, float &v) const;
    bool getDoubleContent(const std::string &key, double &v) const;
    bool getStringContent(const std::string &key, std::string &v) const;
    const MessagePropertyVariant* getVariantContent(const std::string &key) const;
    const util::VariantMap& getMapContent() const;
    void contentKeys(std::vector<std::string> &keys) const;


    // property
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
    bool getDoubleProperty(const std::string &key, double &v) const ;
    bool getStringProperty(const std::string &key, std::string &v) const;
    const MessagePropertyVariant* getVariantProperty(const std::string &key) const;
    void propertyKeys(std::vector<std::string> &keys) const;

    const util::VariantMap& getProperties() const;
    void clearProperties();
    void removeProperty(const std::string &key);
    bool propertyExist(const std::string &key) const;
    // uint8_t getMessageLevel() const;
    // void setMessageLevel(uint8_t level);


    /**
     * Clear the internal buffer for new setXXX() operation
     *
     */
    void clear();
    void clearBody();
    bool hasBody() const;

    /**
     * Get the internal buffer, including the type prefix dep::byte
     *
     */
    //const dep::ByteBuffer& get() const;

    // /**
    //  * Set the internal buffer, including the type prefix dep::byte
    //  *
    //  */
    // bool set(const dep::ByteBuffer& message);

    // /**
    //  * Set the internal buffer by move operator
    //  */
    // bool set(dep::ByteBuffer&& message);

    // // write buffType and pure data.
    // void set(uint8_t buffType, const dep::ByteBuffer& message);

	const util::LightmqMessageHead & getLightmqMessageHead() const;

    util::LightmqMessageHead& getLightmqMessageHead();

    bool acknowledge() const;

    void setConsumer(const std::weak_ptr<IMessageConsumer>& pConsumer);

    void setDestination(const std::string& destination);

    void setDestination(std::string&& destination);
    const std::string& getDestination() const;

    void setMessageID(const MessageID& msgID);
    const MessageID& getMessageID() const;

	//IMessageConsumer* getConsuemr() const;


    // JMS
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
    dep::ByteBuffer m_internalBuffer;
     
    util::VariantMap m_mapContent;
    //Type m_type;
    util::LightmqMessageHead m_lightmqMessageHead;
    std::weak_ptr<IMessageConsumer> m_pConsumer;
    std::string m_destination;
    MessageID m_msgId;
};



}} // namespace lightmq { namespace qmapi {



#endif /* LIGHTMQMESSAGE_H_ */
