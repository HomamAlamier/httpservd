#include <Network/SocketStream.h>
#include <Network/Socket.h>
#include <Core/ByteArray.h>
#include <memory.h>
SocketStream::SocketStream(Socket* socket, Mode mode)
    : Stream(mode, false)
    , _internalSocket(socket)
{
    _c = 0;
}
unsigned SocketStream::availableBytes()
{
    if (_internalSocket == nullptr)
        return 0;
    else
        return _internalSocket->dataAvailable();
}
unsigned SocketStream::read(ByteArray* buffer, unsigned size)
{
    if (_internalSocket == nullptr && availableBytes() == 0 && _mode == WriteOnly)
        return 0;
    unsigned rd = size > availableBytes() ? availableBytes() : size;

    unsigned c = _internalSocket->Read(buffer, rd);
    _c += c;
    return c;
}
unsigned SocketStream::read(char* buffer, unsigned size)
{
    if (_internalSocket == nullptr && availableBytes() == 0 && _mode == WriteOnly)
        return 0;
    unsigned rd = size > availableBytes() ? availableBytes() : size;
    if (rd == 0)
        return rd;
    ByteArray ba;
    unsigned c = _internalSocket->Read(&ba, rd);
    memcpy(ba.dataPtr(), buffer, ba.size());
    _c += c;
    return c;
}
unsigned SocketStream::write(const ByteArray* buffer)
{
    if (_internalSocket == nullptr && _mode == ReadOnly)
        return 0;
    
    return _internalSocket->Write(*buffer);
}
unsigned SocketStream::write(const char* buffer, unsigned size)
{
    if (_internalSocket == nullptr && _mode == ReadOnly)
        return 0;
    
    return _internalSocket->Write(ByteArray(buffer, size));
}
int SocketStream::seek(unsigned to)
{
    return 0;
}