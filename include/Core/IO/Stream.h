#ifndef STREAM_H
#define STREAM_H

class ByteArray;
class Stream 
{
public:
    enum Mode
    {
        ReadOnly, WriteOnly, ReadWrite
    };
    Stream(Mode mode, bool canSeek) : _mode(mode) , _canSeek(canSeek) {}
    virtual unsigned read(ByteArray* buffer, unsigned size) = 0;
    virtual unsigned read(char* buffer, unsigned size) = 0;
    virtual unsigned write(const ByteArray* buffer) = 0;
    virtual unsigned write(const char* buffer, unsigned size) = 0;
    virtual unsigned availableBytes() = 0;
    virtual int seek(unsigned to) = 0;
    unsigned current() const { return _c; }
    Mode mode() const { return _mode; }
    bool canSeek() const { return _canSeek; }
protected:
    Mode _mode;
    bool _canSeek;
    unsigned _c;
};


#endif // !STREAM_H
