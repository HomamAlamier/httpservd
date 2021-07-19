#include <Core/IO/MemoryStream.h>
#include <Core/ByteArray.h>
#include <memory.h>

MemoryStream::MemoryStream(ByteArray* buffer, Mode mode)
    : Stream(mode, true)
    , _internalBuffer(buffer)
    , _isBufferOnlyInternal(false)
{
    _c = 0;
}
MemoryStream::MemoryStream(Mode mode)
    : Stream(mode, true)
    , _internalBuffer(new ByteArray)
    , _isBufferOnlyInternal(true)
{   
    _c = 0;
}
MemoryStream::~MemoryStream()
{
    if (_isBufferOnlyInternal)
        delete _internalBuffer;
}
unsigned MemoryStream::availableBytes() 
{
    if (_internalBuffer == nullptr)
        return 0;
    else
        return _internalBuffer->size() - _c;
}
unsigned MemoryStream::read(ByteArray* buffer, unsigned size) 
{
    if (_internalBuffer == nullptr && availableBytes() == 0 && _mode == WriteOnly)
        return 0;
    unsigned rd = size > availableBytes() ? availableBytes() : size;
    buffer->copyData(&_internalBuffer->dataPtr()[_c], 0, rd);
    _c += rd;
    return rd;
}
unsigned MemoryStream::read(char* buffer, unsigned size) 
{
    if (_internalBuffer == nullptr && availableBytes() == 0 && _mode == WriteOnly)
        return 0;
    unsigned rd = size > availableBytes() ? availableBytes() : size;
    memcpy(buffer, &_internalBuffer->dataPtr()[_c], rd);
    _c += rd;
    return rd;
}
unsigned MemoryStream::write(const ByteArray* buffer)
{
    if (_internalBuffer == nullptr && _mode == ReadOnly)
        return 0;
    _internalBuffer->append(buffer->dataPtr(), buffer->size());
    return buffer->size();
}
unsigned MemoryStream::write(const char* buffer, unsigned size)
{
    if (_internalBuffer == nullptr && _mode == ReadOnly)
        return 0;
    _internalBuffer->append(buffer, size);
    return size;
}
int MemoryStream::seek(unsigned to)
{
    if (!_canSeek)
        return 0;
    if (to > _internalBuffer->size())
    {
        int ret = _internalBuffer->size() - _c;
        _c = _internalBuffer->size();
        return ret;
    }
    else
    {
        int ret = _c - to;
        _c = to;
        return ret;
    }
}