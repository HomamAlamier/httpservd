#include <Web/HttpResponse.h>
#include <Core/ByteArray.h>
#include <Log/Log.h>


const char* statusStrings[] = 
{
	"200 OK",
	"201 Created",
	"202 Accepted",
	"203 Non-Authoritative Information",
	"204 No Content",
	"205 Reset Content",
	"206 Partial Content",
	"300 Multiple Choice",
	"301 Moved Permanently",
	"302 Found",
	"303 See Other",
	"304 Not Modified",
	"400 Bad Request",
	"401 Unauthorized",
	"403 Forbidden",
	"404 Not Found",
	"405 Method Not Allowed",
	"500 Internal Server Error",
	"501 Not Implemented",
	"502 Bad Gateway",
	"503 Service Unavailable"
};

std::string statusCodeToString(int code)
{
	switch (code)
	{
	case 200: return statusStrings[0];
	case 201: return statusStrings[1];
	case 202: return statusStrings[2];
	case 203: return statusStrings[3];
	case 204: return statusStrings[4];
	case 205: return statusStrings[5];
	case 206: return statusStrings[6];
	case 300: return statusStrings[7];
	case 301: return statusStrings[8];
	case 302: return statusStrings[9];
	case 303: return statusStrings[10];
	case 304: return statusStrings[11];
	case 400: return statusStrings[12];
	case 401: return statusStrings[13];
	case 403: return statusStrings[14];
	case 404: return statusStrings[15];
	case 405: return statusStrings[16];
	case 500: return statusStrings[17];
	case 501: return statusStrings[18];
	case 502: return statusStrings[19];
	case 503: return statusStrings[20];
	}
	return "";
}

std::string httpVersionToString(HttpVersion ver)
{
	switch (ver)
	{
	case NoVersion: return "NoVersion";
	case V1_1: return "HTTP/1.1";
	case V1_2: return "HTTP/1.2";
	case V2_0: return "HTTP/2.0";
	}
}


HttpResponse::HttpResponse()
	: _statusCode(0)
	, _version(HttpVersion::NoVersion)
	, _data(new ByteArray())
{

}
std::string HttpResponse::statusCodeString() const
{
	return statusCodeToString(_statusCode);
}
std::string HttpResponse::serialize() const
{
	std::string tmp;
	tmp += httpVersionToString(_version);
	tmp += " " + statusCodeToString(_statusCode);
	tmp += "\r\n";


	for (const auto& item : _headers)
	{
		tmp += item.first + ": " + item.second + "\r\n";
	}
	tmp += "Content-Length: " + std::to_string(_data->size());
	tmp += "\r\n\r\n";
	return tmp;
}
void HttpResponse::addHeader(std::string key, std::string value)
{
	if (key != "" && value != "" 
		&& key != "Content-Type")
		_headers.insert(std::pair<std::string, std::string>(key, value));
}
void HttpResponse::setData(const ByteArray& byteArray)
{
	if (_data)
		delete _data;
	_data = new ByteArray(byteArray);
}
int HttpResponse::contentLength() const
{
	return _data->size();
}