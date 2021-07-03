#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <Object.h>
#include <Network/Socket.h>


class ByteArray;
class HttpServer : public SocketCallBacks, public Object
{

public: 
	HttpServer(int port);
	void Start();
protected:
	void acceptConnectionCallBack(Socket*) override;
	void dataReceiveCallBack(Socket*, const ByteArray*, int) override;
private:
	Socket* _socket;
	ByteArray* _buffer;
	IPEndPoint* _endPoint;
};


#endif // !HTTPSERVER_H
