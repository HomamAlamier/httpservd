#ifndef HTTPRESPONSE_H
#define HTTPRESPONSE_H


#include <string>
#include <Web/HttpGlobals.h>
#include <map>

class ByteArray;
class HttpResponse
{
public:
	HttpResponse();


	std::string serialize() const;
	int statusCode() const { return _statusCode; }
	HttpVersion httpVesion() const { return _version; }
	std::string statusCodeString() const;
	ByteArray* data() { return _data; }
	int contentLength() const;


	void setStatusCode(int code) { _statusCode = code; }
	void setHttpVersion(HttpVersion version) { _version = version; }
	void addHeader(std::string key, std::string value);
	void setData(const ByteArray& byteArray);
private:
	int _statusCode;
	std::string _statusString;
	HttpVersion _version;
	std::map<std::string, std::string> _headers;
	ByteArray* _data;
};

#endif // !HTTPRESPONSE_H
