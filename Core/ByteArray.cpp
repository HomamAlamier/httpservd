#include <Core/ByteArray.h>
#include <memory>

ByteArray::ByteArray()
	: Object("ByteArray")
	, _buffer(nullptr)
	, _size(0)
{
	
}
ByteArray::ByteArray(const char* bytes, unsigned size)
	: ByteArray()
{
	_buffer = new char[size];
	_size = size;
	copyData(bytes, 0, size);
}
ByteArray::ByteArray(unsigned size)
	: ByteArray()
{
	_buffer = new char[size];
	_size = size;
}
void ByteArray::append(const char* bytes, unsigned size)
{
	char* nbuf = new char[_size + size];
	if (_buffer != nullptr)
		memcpy_s(nbuf, _size, _buffer, _size);
	memcpy_s(&nbuf[_size], size, bytes, size);

	delete _buffer;
	_buffer = nbuf;
	_size += size;

}
void ByteArray::operator <<(const ByteArray& ba)
{
	append(ba._buffer, ba._size);
}
std::string ByteArray::toStdString() const
{
	return std::string(_buffer, _size);
}
void ByteArray::copyData(const char* bytes, int start, int size)
{
	if (start + size > _size)
		return;
	memcpy_s(&_buffer[start], size, bytes, size);
}