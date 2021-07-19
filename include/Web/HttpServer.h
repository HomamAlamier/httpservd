#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <Network/Socket.h>


class ByteArray;
class HttpRequest;
class HttpResponse;
class HttpServerFileManager;
class Directory;
class ModulesManager;
class Module;
class HttpServer : public SocketCallBacks
{

public: 
	HttpServer(int port, Directory* cwd, const std::string& hostIp = "0.0.0.0");
	void Start();
	void setRootDir(Directory* rootDir);
	void addModule(Module* mod);
protected:
	void acceptConnectionCallBack(Socket*) override;
	bool dataReceiveCallBack(Socket*, ByteArray*, void*, int) override;
private:
	HttpResponse* processIncomingRequest(const HttpRequest& request);
	Socket* _socket;
	ByteArray* _buffer;
	IPEndPoint* _endPoint;
	HttpServerFileManager* _fileManager;
	ModulesManager* _modMan;
};


#endif // !HTTPSERVER_H
