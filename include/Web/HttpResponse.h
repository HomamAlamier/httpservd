#ifndef HTTPRESPONSE_H
#define HTTPRESPONSE_H


#include <string>
#include <Web/HttpGlobals.h>
#include <map>

class ByteArray;
class Stream;
class MemoryStream;
class HttpResponse
{
public:
	HttpResponse(MemoryStream* stream);
	~HttpResponse();

	std::string serialize() const;
	int statusCode() const { return _statusCode; }
	HttpVersion httpVesion() const { return _version; }
	std::string statusCodeString() const;
	int contentLength() const;
	MemoryStream* dataStream() { return _stream; }

	void setStatusCode(int code) { _statusCode = code; }
	void setHttpVersion(HttpVersion version) { _version = version; }
	void addHeader(std::string key, std::string value);
	void clearHeaders() { _headers.clear(); }
	const std::map<std::string, std::string>& headers() const { return _headers; }
private:
	int _statusCode;
	std::string _statusString;
	HttpVersion _version;
	std::map<std::string, std::string> _headers;
	MemoryStream* _stream;
};

#endif // !HTTPRESPONSE_H
