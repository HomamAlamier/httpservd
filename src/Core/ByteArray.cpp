#include <Core/ByteArray.h>
#include <memory.h>
#include <Log/Log.h>

ByteArray::ByteArray()
	: _buffer(nullptr)
	, _size(0)
{
}
ByteArray::ByteArray(const ByteArray& byteArray)
	: ByteArray(byteArray.dataPtr(), byteArray.size())
{
}

ByteArray::ByteArray(const std::string& str)
	: ByteArray(str.data(), str.size())
{
}

ByteArray::ByteArray(const char* bytes, unsigned size)
	: ByteArray()
{
	_buffer = new char[size_t(size) + 1];
	_size = size;
	copyData(bytes, 0, size);
	_buffer[size] = '\0';
}
ByteArray::ByteArray(unsigned size)
	: ByteArray()
{
	_buffer = new char[size_t(size) + 1];
	_size = size;
}
ByteArray::~ByteArray()
{
	delete[] _buffer;
}

void ByteArray::append(const char* bytes, unsigned size)
{
	char* nbuf = new char[_size + size_t(size) + 1];
	if (_buffer != nullptr)
		memcpy(nbuf, _buffer, _size);
	memcpy(&nbuf[_size], bytes, size);
	if (_buffer != nullptr)
		delete[] _buffer;
	_buffer = nbuf;
	_size += size;
	_buffer[_size] = '\0';

}

std::string ByteArray::toStdString() const
{
	return std::string(_buffer, _size);
}
void ByteArray::copyData(const char* bytes, int start, int size)
{
	if (start + size > _size)
		return;
	memcpy(&_buffer[start], bytes, size);
}

void ByteArray::operator <<(const ByteArray& ba)
{
	append(ba._buffer, ba._size);
}
void ByteArray::operator <<(char c)
{
	append(&c, 1);
}
char ByteArray::operator [](int index) const
{
	if (index >= 0 && index < _size)
		return _buffer[index];
	else
		return '\0';
}