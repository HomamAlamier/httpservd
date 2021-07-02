#include <Network/Socket.h>
#include <Log/Log.h>


#ifdef WINOS
	#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN
	#endif
	#include <windows.h>
	#include <winsock2.h>
	#include <ws2tcpip.h>
	#include <iphlpapi.h>
	#include <stdio.h>
	#pragma comment(lib, "Ws2_32.lib")
#endif // WINOS




static bool _os_socket_inited_ = false;
static int _socket_instance_count = 0;

IPEndPoint::IPEndPoint(const std::string& ip, int port, IPType type)
	: Object("IPEndPoint")
	, _ip(ip)
	, _port(port)
	, _type(type)

{
#ifdef LOG_LEVEL_5
	log()->Debug(DUMP_VAR(port));
	log()->Debug(DUMP_VAR(type));
	log()->Debug(DUMP_STRING(ip));
#endif // LOG_LEVEL_5
}
Socket::Socket(SocketType type, Protocol protocol) 
	: Object("Socket")
	, _endPoint(nullptr)
	, _type(type)
	, _pro(protocol)
	, _st(None)
	, _sockPtr(0)
	, _nIp(nullptr)
{
	if (!_os_socket_inited_)
	{
		#ifdef WINOS
			WSADATA data;
			int iResult = WSAStartup(MAKEWORD(2, 2), &data);
			if (iResult == 0)
			{
				#ifdef LOG_LEVEL_5
					log()->Debug("Ws2_32 Lib inited !");
					log()->Debug(data.szSystemStatus);
					log()->Debug(data.szDescription);
				#endif // LOG_LEVEL_5
			}
		#endif // WIN32
		_os_socket_inited_ = true;
	}
	_socket_instance_count++;
}
Socket::~Socket()
{
	_socket_instance_count--;
	if (_socket_instance_count == 0 && _os_socket_inited_)
	{
		_os_socket_inited_ = false;
		#ifdef WINOS
			WSACleanup();
		#endif // WINOS
	}
	#ifdef WINOS
		if (_nIp != nullptr)
		{
			freeaddrinfo(_nIp);
		}
	#endif // WINOS
}
void Socket::endPointToNative()
{
	struct addrinfo hints;
	ZeroMemory(&hints, sizeof hints);
	hints.ai_family = _endPoint->ipType();
	hints.ai_socktype = _type;
	hints.ai_protocol = _pro;
	int iResult = getaddrinfo(_endPoint->ip().c_str(), std::to_string(_endPoint->port()).c_str(), &hints, &_nIp);
	if (iResult != 0)
	{
		log()->Error("getaddrinfo failed errcode = " + std::to_string(iResult));
	}
}
void Socket::Bind(const IPEndPoint* endpoint)
{
	_endPoint = endpoint;
}
void Socket::Listen(int backlog)
{
	if (_endPoint == nullptr)
		return;
	_sockPtr = socket(_endPoint->ipType(), _type, _pro);
	endPointToNative();
}