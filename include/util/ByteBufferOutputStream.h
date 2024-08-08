#ifndef DSP_IMT_API_INC_IMT_INTERNAL_IMPL_DEP_BYTEBUFFEROUTPUTSTREAM_H_
#define DSP_IMT_API_INC_IMT_INTERNAL_IMPL_DEP_BYTEBUFFEROUTPUTSTREAM_H_
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include "ByteBuffer.h"

namespace lightmq{ namespace util{

// A ZeroCopyOutputStream which appends bytes to a string.
class ByteBufferOutputStream : public google::protobuf::io::ZeroCopyOutputStream {
public:
    explicit ByteBufferOutputStream(dep::ByteBuffer* target);

    // implements ZeroCopyOutputStream ---------------------------------
    bool Next(void** data, int* size) override;
    void BackUp(int count) override;
    int64_t ByteCount() const override;


private:
    static const int kMinimumSize = 1024;

    dep::ByteBuffer* target_;
    uint32_t startPostion_;

    GOOGLE_DISALLOW_EVIL_CONSTRUCTORS(ByteBufferOutputStream);
};

}}
#endif /* DSP_IMT_API_INC_IMT_INTERNAL_IMPL_DEP_BYTEBUFFEROUTPUTSTREAM_H_ */
