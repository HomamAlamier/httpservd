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
	_buffer = new char[size_t(size)];
	_size = size;
	copyData(bytes, 0, size);
}
ByteArray::ByteArray(unsigned size)
	: ByteArray()
{
	_buffer = new char[size_t(size)];
	_size = size;
}
ByteArray::~ByteArray()
{
	delete[] _buffer;
}

void ByteArray::append(const char* bytes, unsigned size)
{
	char* nbuf = new char [_size + size_t(size)];
	if (_buffer != nullptr)
		memcpy(nbuf, _buffer, _size);
	memcpy(&nbuf[_size], bytes, size);
	if (_buffer != nullptr)
		delete[] _buffer;
	_buffer = nbuf;
	_size += size;

}
void ByteArray::remove(unsigned from, unsigned count)
{
	if (from + count > _size)
		return;
	unsigned newSize = _size - count;
	char* newBuffer = new char[newSize];
	if (from > 0)
		memcpy(newBuffer, _buffer, from);
	memcpy(&newBuffer[from], &_buffer[from + count], _size - (from + count));
	delete[] _buffer;
	_buffer = newBuffer;
	_size = newSize;
}
void ByteArray::allocate(unsigned size)
{
	if (_buffer != nullptr)
		delete[] _buffer;
	_buffer = new char[size];
	_size = size;
}
std::string ByteArray::toStdString() const
{
	return std::string(_buffer, _size);
}
void ByteArray::copyData(const char* bytes, int start, int size)
{
	if (start + size > _size)
	{
		char* nbuf = new char[_size + (_size - (start + size))];
		memcpy(nbuf, _buffer, _size);
		delete[] _buffer;
		_buffer = nbuf;
	}
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