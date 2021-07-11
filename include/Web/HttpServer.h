#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <Network/Socket.h>


class ByteArray;
class HttpRequest;
class HttpResponse;
class HttpServerFileManager;
class Directory;
class HttpServer : public SocketCallBacks
{

public: 
	HttpServer(int port, Directory* cwd, const std::string& hostIp = "0.0.0.0");
	void Start();
	void setRootDir(Directory* rootDir);
protected:
	void acceptConnectionCallBack(Socket*) override;
	void dataReceiveCallBack(Socket*, ByteArray*, void*, int) override;
private:
	HttpResponse processIncomingRequest(const HttpRequest& request);
	Socket* _socket;
	ByteArray* _buffer;
	IPEndPoint* _endPoint;
	HttpServerFileManager* _fileManager;
};


#endif // !HTTPSERVER_H
