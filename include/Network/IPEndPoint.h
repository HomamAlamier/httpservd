#ifndef IPENDPOINT_H
#define IPENDPOINT_H

#include <Object.h>
#include <string>

class IPEndPoint : public Object
{
public:
	enum IPType
	{
		IPv4 = 2,
		IPv6 = 23
	};
	int port() const { return _port; }
	IPType ipType() const { return _type; }
	const std::string& ip() const { return _ip; }

	IPEndPoint(const std::string& ip, int port, IPType type);
	std::string toString() const override;
private:
	int _port;
	IPType _type;
	std::string _ip;
};

#endif // !IPENDPOINT_H
