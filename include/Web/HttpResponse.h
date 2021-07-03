#ifndef HTTPRESPONSE_H
#define HTTPRESPONSE_H


#include <Object.h>
#include <string>
#include <Web/HttpGlobals.h>
#include <map>


class HttpResponse : public Object
{
public:
	HttpResponse();
	void setStatusCode(int code) { _statusCode = code; }
	void setHttpVersion(HttpVersion version) { _version = version; }
	std::string serialize() const;
	int statusCode() const { return _statusCode; }
	HttpVersion httpVesion() const { return _version; }
	const std::string& statusCodeString() const;
	void addHeader(std::string key, std::string value);
private:
	int _statusCode;
	std::string _statusString;
	HttpVersion _version;
	std::map<std::string, std::string> _headers;
};

#endif // !HTTPRESPONSE_H
