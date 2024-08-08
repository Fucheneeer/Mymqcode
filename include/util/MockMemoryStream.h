////////////////////////////////////////////////////////////////////////////////
//
// MockMemoryStream.h
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

#ifndef LIGHTMQ_LIGHTMQUTIL_INC_MOCKMEMORYSTREAM_H_
#define LIGHTMQ_LIGHTMQUTIL_INC_MOCKMEMORYSTREAM_H_
#include <fstream>
#include <string>

namespace dep {
    class ByteBuffer;
}

namespace lightmq { namespace util {


    class MockMemoryStream {
    public:
        MockMemoryStream() = default;
        virtual ~MockMemoryStream() = default;
        MockMemoryStream(const std::string& fileName, std::ios_base::openmode openMode);
        MockMemoryStream(const MockMemoryStream&) = delete;
        MockMemoryStream(MockMemoryStream&& fileStream) = delete;
        void open(const std::string& fileName, std::ios_base::openmode openMode);
        bool is_open() const;
        void close();
        MockMemoryStream& read(dep::ByteBuffer& buffer, uint32_t length);
        MockMemoryStream& read(char* s, uint32_t length);
        MockMemoryStream& write(dep::ByteBuffer& buffer);
        MockMemoryStream& write(const char* s, uint32_t length);
        MockMemoryStream& ignore(uint32_t n);
        uint64_t tellp();
        uint64_t tellg();
        MockMemoryStream& seekg(uint64_t position);
        MockMemoryStream& seekg(int64_t offset, std::ios_base::seekdir way);
        MockMemoryStream& seekp(uint64_t position);
        MockMemoryStream& seekp(int64_t offset, std::ios_base::seekdir way);
        bool eof();
        void flush();
        bool sync();
        void clear();
        char peek();
    };

}} // namespace lightmq { namespace util {

#endif /* LIGHTMQ_LIGHTMQUTIL_INC_MOCKMEMORYSTREAM_H_ */
