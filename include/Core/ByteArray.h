#ifndef BYTEARRAY_H
#define BYTEARRAY_H

#include <string>

class ByteArray
{
public:
	ByteArray();
	ByteArray(unsigned size);
	ByteArray(const char* bytes, unsigned size);
	ByteArray(const std::string& str);
	ByteArray(const ByteArray& byteArray);
	~ByteArray();

	int size() const { return _size; }
	char* dataPtr() const { return _buffer; }
	void append(const char* bytes, unsigned size);
	void remove(unsigned from, unsigned count);
	void allocate(unsigned size);
	std::string toStdString() const;
	void copyData(const char* bytes, int start, int size);


	void operator <<(const ByteArray& ba);
	void operator <<(char c);
	char operator [](int index) const;
private:
	char* _buffer;
	unsigned _size;
};


#endif // !BYTEARRAY_H
