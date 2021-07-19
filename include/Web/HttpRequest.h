#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include <string>
#include <map>
#include <vector>
#include <Web/HttpGlobals.h>
class Stream;
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

	HttpRequest(const std::string& data, Stream* dataStream);

	HttpVersion httpVersion() const { return _version; }
	const std::string& path() const { return _path; }
	Stream* dataStream() const { return _dataStream; }
	const std::string& operator [](const std::string& key);
	Method method() const { return _method; }
	const std::map<std::string, std::string>& headers() const { return _headers; }
private:
	std::map<std::string, std::string> _headers;
	Method _method;
	std::string _path;
	HttpVersion _version;
	Stream* _dataStream;
};

#endif // !HTTPREQUEST_H
