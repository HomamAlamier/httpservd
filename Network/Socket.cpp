#include <Network/Socket.h>
#include <Log/Log.h>
#include <Globals.h>
#include <thread>
#include <Core/ByteArray.h>
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


	char* lastErr()
	{
		return strerror(WSAGetLastError());
	}
#endif // WINOS

void* get_in_addr(struct sockaddr* sa)
{
	if (sa->sa_family == AF_INET)
		return &(((struct sockaddr_in*)sa)->sin_addr);
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


static bool _os_socket_inited_ = false;
static int _socket_instance_count = 0;


Socket::Socket(SocketType type, Protocol protocol) 
	: Object("Socket")
	, _endPoint(nullptr)
	, _type(type)
	, _pro(protocol)
	, _st(None)
	, _sockPtr(0)
	, _nIp(nullptr)
	, _running(false)
	, _acceptThread(nullptr)
	, _acceptThreadAlive(false)
	, _receiveThread(nullptr)
	, _receiveThreadAlive(false)
	, _receiveBuffer(nullptr)
	, _callbacksPtr(nullptr)
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
	Close();
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
		if (_sockPtr != 0)
		{
			closesocket(_sockPtr);
		}
	#endif // WINOS

}
bool Socket::endPointToNative()
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
		return false;
	}
	return true;
}
void Socket::Bind(const IPEndPoint* endpoint)
{
	_endPoint = endpoint;
}
bool Socket::Listen(int backlog)
{
	if (_endPoint == nullptr)
		return false;

	if (_sockPtr != 0)
		closesocket(_sockPtr);

	_sockPtr = socket(_endPoint->ipType(), _type, _pro);

	if (!endPointToNative())
		return false;

	log()->Debug("Binding...");
	if (bind(_sockPtr, _nIp->ai_addr, int(_nIp->ai_addrlen)) == SOCKET_ERROR)
	{
		log()->Error("Socket bind failed !");
		log()->Error(lastErr());
		return false;
	}

	log()->Debug("Listening...");
	if (listen(_sockPtr, backlog) == SOCKET_ERROR)
	{
		log()->Error("Socket listen failed !");
		log()->Error(lastErr());
		return false;
	}
	_st = Listening;
	_running = true;
	return true;
}
void Socket::Close()
{
	_running = false;
	_st = None;

	if (_sockPtr != 0)
	{
		#ifdef WINOS
			closesocket(_sockPtr);
		#endif // WINOS
		_sockPtr = 0;
	}
	if (_nIp != nullptr)
	{
		freeaddrinfo(_nIp);
		_nIp = nullptr;
	}

}
Socket* Socket::Accept()
{
	socket_tt client = 0;
	struct sockaddr addr;
	int sz_addr = sizeof addr;
	if ((client = accept(_sockPtr, &addr, &sz_addr)) != 0)
	{

		char buf[INET6_ADDRSTRLEN];
		void* addr_in = get_in_addr(&addr);
		inet_ntop(_endPoint->ipType(), addr_in, buf, sizeof buf);
		log()->Debug(std::string("Accepting connection from ") + buf + "...");

		int port = (_endPoint->ipType() == IPEndPoint::IPv4) 
			? ((struct sockaddr_in*)addr_in)->sin_port 
			: ((struct sockaddr_in6*)addr_in)->sin6_port;

		Socket* s = new Socket(_type, _pro);
		s->_sockPtr = client;
		s->_endPoint = new IPEndPoint(buf, port, _endPoint->ipType());
		s->_st = Socket::Connected;
		return s;
	}
	else
	{
		log()->Error("Error accepting connection !");
		return nullptr;
	}
}
void Socket::AcceptAsync()
{
	if (_acceptThread != nullptr || _callbacksPtr == nullptr)
		return;
	_running = true;
	_acceptThread = new std::thread(&Socket::acceptConnection, this);
}
void Socket::acceptConnection()
{
	_acceptThreadAlive = true;
	while (_running)
	{
		Socket* s = Accept();
		_callbacksPtr->acceptConnectionCallBack(s);
	}
	_acceptThreadAlive = false;
}
int Socket::Write(const ByteArray& byteArray)
{
	if (_sockPtr == 0 || _st != Connected)
		return 0;
	int count = send(_sockPtr, byteArray.dataPtr(), byteArray.size(), 0);
	return count;
}
int Socket::Read(ByteArray* byteArray, int size)
{
	if (_sockPtr == 0 || _st != Connected || byteArray == nullptr)
		return 0;
	int count = recv(_sockPtr, byteArray->dataPtr(), size, 0);
	return count;
}
void Socket::ReadAsync(ByteArray* buffer)
{
	if (_receiveThread != nullptr || _callbacksPtr == nullptr)
		return;
	_receiveBuffer = buffer;
	_running = true;
	_receiveThread = new std::thread(&Socket::receiveData, this);
}
void Socket::receiveData()
{
	_receiveThreadAlive = true;
	while (_running)
	{
		int c = Read(_receiveBuffer, _receiveBuffer->size());
		if (c > 0 && _running)
		{
			_callbacksPtr->dataReceiveCallBack(this, _receiveBuffer, c);
		}
	}
	_receiveThreadAlive = false;
}
void Socket::setSocketCallBacks(SocketCallBacks* ptr)
{
	if (ptr != nullptr)
	{
		_callbacksPtr = ptr;
	}
}