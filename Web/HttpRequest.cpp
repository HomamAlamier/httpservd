#include <Web/HttpRequest.h>
#include <Core/StringUtils.h>
HttpRequest::HttpRequest(const std::string& data)
	: Object("HttpRequest")
	, _method(NoMethod)
	, _version(NoVersion)
{
	auto vec = stringSplit(data, "\r\n");

	auto fstLine = stringSplit(vec[0], " ");
	if (fstLine[0] == "GET")
	{
		_method = Get;
	}
	else if (fstLine[0] == "POST")
	{
		_method = Post;
	}
	else if (fstLine[0] == "PUT")
	{
		_method = Put;
	}
	else if (fstLine[0] == "CONNECT")
	{
		_method = Connect;
	}
	else if (fstLine[0] == "DELETE")
	{
		_method = Delete;
	}

	_path = fstLine[1];

	if (fstLine[2] == "HTTP/1.1")
	{
		_version = V1_1;
	}
	else if (fstLine[2] == "HTTP/1.2")
	{
		_version = V1_2;
	}
	else if (fstLine[2] == "HTTP/2.0")
	{
		_version = V2_0;
	}


	if (vec.size() > 0)
	{
		for (int i = 1; i < vec.size(); ++i)
		{
			const std::string& item = vec[i];
			if (item.size() == 0)
				continue;
			int st = item.find(':');
			std::string key = item.substr(0, st);
			std::string value = item.substr(st + 2);
			_headers.insert(std::pair<std::string, std::string>(key, value));
		}
	}
}
const std::string& HttpRequest::operator [](const std::string& key)
{
	return _headers[key];
}