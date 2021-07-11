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
protected:
	virtual void acceptConnectionCallBack(Socket*) = 0;
	virtual void dataReceiveCallBack(Socket*,ByteArray*, void*, int) = 0;
};


class Socket 
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
	void close();
	Socket* Accept();
	void AcceptAsync();
	int Write(const ByteArray& byteArray);
	int Read(ByteArray* byteArray, int size);
	void ReadAsync(ByteArray* buffer, void* data = nullptr);
	void setSocketCallBacks(SocketCallBacks* ptr);
	void destory();
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
};


#endif // !SOCKET_H
