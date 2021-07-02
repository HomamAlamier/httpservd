#ifndef SOCKET_H
#define SOCKET_H

#include <string>
#include <Object.h>
#include <Network/IPEndPoint.h>

struct addrinfo;
namespace std
{
	class thread;
}
class Socket;
class ByteArray;

typedef unsigned long long socket_tt;
typedef void(*AcceptConnectionCallBack)(Socket*);
typedef void(*DataReceiveCallBack)(Socket*, int);

class Socket : public Object
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

	Socket(SocketType type, Protocol protocol);
	~Socket();
	void Bind(const IPEndPoint* endpoint);
	bool Listen(int backlog);
	void Close();
	Socket* Accept();
	void AcceptAsync(AcceptConnectionCallBack callback);
	int Write(const ByteArray& byteArray);
	int Read(ByteArray* byteArray, int size);
	void ReadAsync(ByteArray* buffer, DataReceiveCallBack callback);

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
	AcceptConnectionCallBack _acceptCallBack;
	std::thread* _receiveThread;
	bool _receiveThreadAlive;
	DataReceiveCallBack _receiveCallBack;
	ByteArray* _receiveBuffer;
};


#endif // !SOCKET_H