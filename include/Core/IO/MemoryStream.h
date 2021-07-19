#ifndef MEMORYSTREAM_H
#define MEMORYSTREAM_H

#include "Stream.h"
class MemoryStream : public Stream
{
public:
    MemoryStream(ByteArray* buffer, Mode mode);
    MemoryStream(Mode mode);
    ~MemoryStream();
    unsigned read(char* buffer, unsigned size) override;
    unsigned read(ByteArray* buffer, unsigned size) override;
    unsigned write(const ByteArray* buffer) override;
    unsigned write(const char* buffer, unsigned size) override;
    int seek(unsigned to) override;
    unsigned availableBytes() override;
    const ByteArray* dataPtr() const { return _internalBuffer; }
private:
    ByteArray* _internalBuffer;
    bool _isBufferOnlyInternal;
};




#endif // !MEMORYSTREAM_H