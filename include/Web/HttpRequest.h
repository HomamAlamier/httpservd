#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include <string>
#include <map>
#include <vector>
#include <Object.h>
#include <Web/HttpGlobals.h>
class HttpRequest : public Object
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
	HttpRequest(const std::string& data);
	const std::string& operator [](const std::string& key);
	Method method() const { return _method; }
	HttpVersion httpVersion() const { return _version; }
	const std::string& path() const { return _path; }
private:
	std::map<std::string, std::string> _headers;
	Method _method;
	std::string _path;
	HttpVersion _version;
};

#endif // !HTTPREQUEST_H
