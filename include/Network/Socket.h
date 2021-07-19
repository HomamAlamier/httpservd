#ifndef SOCKET_H
#define SOCKET_H

#include <string>
#include <string>
#include <Network/IPEndPoint.h>

struct addrinfo;


namespace std
{
	class thread;
}
class Socket;
class ByteArray;
typedef unsigned long long socket_tt;

class SocketCallBacks
{
	friend Socket;
	friend void _async_accept_invoke_callback_(int, Socket*, Socket*);
	friend void _async_accept_receive_callback_(int, Socket*, ByteArray*, void*, int);
protected:
	virtual void acceptConnectionCallBack(Socket*) = 0;
	virtual bool dataReceiveCallBack(Socket*,ByteArray*, void*, int) = 0;
};


class Socket 
{
	friend void _async_accept_handler_();
	friend void _async_receive_handler_();
	friend void _async_accept_invoke_callback_(int, Socket*, Socket*);
	friend void _async_accept_receive_callback_(int, Socket*, ByteArray*, void*, int);
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
		Connected,
		Accepting
	};

	const IPEndPoint* ipEndPoint() const { return _endPoint; }
	SocketType socketType() const { return _type; }
	Protocol protocol() const { return _pro; }
	SocketStatus status() const { return _st; }
	int dataAvailable() const;


	Socket(SocketType type, Protocol protocol);
	~Socket();
	void Bind(const IPEndPoint* endpoint);
	bool Listen(int backlog);
	bool Connect(const IPEndPoint& ipEndPoint);
	void close();
	Socket* Accept();
	void AcceptAsync();
	int Write(const ByteArray& byteArray);
	int Read(ByteArray* byteArray, int size);
	void ReadAsync(ByteArray* buffer, void* data = nullptr);
	void setSocketCallBacks(SocketCallBacks* ptr);
	void destory();

	bool destroyed() const { return _destroyed; }
private:
	bool endPointToNative();;
	void acceptConnection();
	void receiveData();
	socket_tt _sockPtr;
	const IPEndPoint* _endPoint;
	SocketType _type;
	Protocol _pro;
	SocketStatus _st;
	addrinfo* _nIp;
	bool _running;
	std::thread* _acceptThread;
	bool _acceptThreadAlive;
	std::thread* _receiveThread;
	bool _receiveThreadAlive;
	ByteArray* _receiveBuffer;
	void* _receiveDataPtr;
	SocketCallBacks* _callbacksPtr;
	int _connection_ms;
	bool _destroyed;
};


#endif // !SOCKET_H
