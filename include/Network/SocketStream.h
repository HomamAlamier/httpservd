#ifndef SOCKETREAM_H
#define SOCKETREAM_H

#include <Core/IO/Stream.h>
class Socket;
class SocketStream : public Stream
{
public:
    SocketStream(Socket* socket, Mode mode);
    unsigned read(ByteArray* buffer, unsigned size) override;
    unsigned read(char* buffer, unsigned size) override;
    unsigned write(const ByteArray* buffer) override;
    unsigned write(const char* buffer, unsigned size) override;
    int seek(unsigned to) override;
    unsigned availableBytes() override;
private:
    Socket* _internalSocket;
};




#endif // !SOCKETREAM_H
