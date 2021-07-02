#ifndef SOCKET_H
#define SOCKET_H

#include <string>

typedef unsigned long long socket_tt;

#include <Globals.h>
#include <Object.h>
struct addrinfo;
class IPEndPoint : Object
{
public:
	enum IPType 
	{
		IPv4 = 2,
		IPv6 = 23
	};
	int port() const { return _port;  }
	IPType ipType() const { return _type; }
	const std::string& ip() const { return _ip; }

	IPEndPoint(const std::string& ip, int port, IPType type);
private:
	int _port;
	IPType _type;
	std::string _ip;
};

class Socket : Object
{
public:
	enum SocketType
	{
		Stream = 1, 
		Datagram, 
		Raw
	};
	enum Protocol
	{
		TCP = 6, 
		UDP = 17, 
		ICMP = 1, 
		ICMPv6 = 58
	};
	enum SocketStatus
	{
		None,
		Listening,
		Connecting,
		Connected
	};

	const IPEndPoint* ipEndPoint() const { return _endPoint; }
	SocketType socketType() const { return _type; }
	Protocol protocol() const { return _pro; }
	SocketStatus status() const { return _st; }

	Socket(SocketType type, Protocol protocol);
	~Socket();
	void Bind(const IPEndPoint* endpoint);
	void Listen(int backlog);
private:
	void endPointToNative();
	socket_tt _sockPtr;
	const IPEndPoint* _endPoint;
	SocketType _type;
	Protocol _pro;
	SocketStatus _st;
	addrinfo* _nIp;
};


#endif // !SOCKET_H
