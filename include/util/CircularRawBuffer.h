////////////////////////////////////////////////////////////////////////////////
//
// CircularRawBuffer.h
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

#ifndef UTIL_CIRCULARRAWBUFFER_H_
#define UTIL_CIRCULARRAWBUFFER_H_
#include "ByteBuffer.h"
namespace lightmq { namespace util {

class CircularRawBuffer {
public:
    CircularRawBuffer(size_t defaultCapacity, size_t maxCapacity);
    virtual ~CircularRawBuffer();
    CircularRawBuffer (CircularRawBuffer&& o );
    CircularRawBuffer& operator=(CircularRawBuffer&& o);
    size_t size() const { return m_size; }
    size_t capacity() const { return m_capacity; }
    size_t maxCapacity() const { return m_maxCapacity; }
    //return write begin index
    int64_t write(dep::ByteBuffer& buffer);
    // Return buffer is copy
    bool read(dep::ByteBuffer& buffer, size_t bytes);
    bool copy(dep::ByteBuffer& buffer, size_t dataStart, size_t length);
    bool ignore(size_t bytes);
    size_t begin() {return m_begIndex;}
    size_t end(){return m_endIndex;}
    int64_t extendBuffer(const size_t &inputBytes);
private:
    CircularRawBuffer(const CircularRawBuffer& sourceBuffer);
    CircularRawBuffer& operator=(const CircularRawBuffer& sourceBuffer);
    void checkDeleteArray();

private:
    size_t m_begIndex;
    size_t m_endIndex;
    size_t m_size;
    size_t m_capacity;
    size_t m_maxCapacity;
    dep::byte *m_pData;
    bool m_deleteOnDestruction;
};

}} // namespace lightmq { namespace util {

#endif /* UTIL_CIRCULARRAWBUFFER_H_ */
