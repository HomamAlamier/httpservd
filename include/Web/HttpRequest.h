#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include <string>
#include <map>
#include <vector>
#include <Web/HttpGlobals.h>

class ByteArray;
class HttpRequest
{
public:
	enum Method 
	{
		NoMethod,
		Get, 
		Post, 
		Connect, 
		Put,
		Delete
	};

	HttpRequest(const std::string& data, const ByteArray* rawData);

	HttpVersion httpVersion() const { return _version; }
	const std::string& path() const { return _path; }
	const ByteArray* data() const { return _data; }
	const std::string& operator [](const std::string& key);
	Method method() const { return _method; }

	void setData(const ByteArray* byteArray) { _data = byteArray; }
private:
	std::map<std::string, std::string> _headers;
	Method _method;
	std::string _path;
	HttpVersion _version;
	const ByteArray* _data;
};

#endif // !HTTPREQUEST_H
