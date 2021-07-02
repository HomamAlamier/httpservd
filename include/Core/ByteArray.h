#ifndef BYTEARRAY_H
#define BYTEARRAY_H

#include <Object.h>

class ByteArray : public Object
{
public:
	ByteArray();
	ByteArray(unsigned size);
	ByteArray(const char* bytes, unsigned size);
	int size() const { return _size; }
	char* dataPtr() const { return _buffer; }
	void append(const char* bytes, unsigned size);
	void operator <<(const ByteArray& ba);
	std::string toStdString() const;
	void copyData(const char* bytes, int start, int size);
private:
	char* _buffer;
	unsigned _size;
};


#endif // !BYTEARRAY_H
