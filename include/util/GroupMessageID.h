
#pragma once

#include <boost/uuid/uuid.hpp>



namespace lightmq {
namespace util {

class VariantMap;

typedef boost::uuids::uuid BUUID;
typedef BUUID GroupMessageID;

namespace uuidFun {

GroupMessageID makeUUID();
std::string toString(BUUID &uuid);

void writeGroupMessgaeID(VariantMap *vmap, GroupMessageID &id);
GroupMessageID propGetGroupMessageID(VariantMap *vmap);
uint16_t propGetGroupMessageSequence(VariantMap *vmap);

} // end uuidFun.
}} // end lightmq::util.

