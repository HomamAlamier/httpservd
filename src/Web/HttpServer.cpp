#include <Web/HttpServer.h>
#include <Core/ByteArray.h>
#include <Network/Socket.h>
#include <Web/HttpRequest.h>
#include <Web/HttpResponse.h>
#include <Core/StringUtils.h>
#include <Web/HttpServerFileManager.h>
#include <iostream>
#include <fstream>
#include <Log/Log.h>
#include <Network/SocketStream.h>
#include <Core/IO/MemoryStream.h>
#include <Web/Modules/ModulesManager.h>
#include <Web/Modules/Module.h>


const ByteArray http_response_404("<html><head><h1>404 Not Found !</h1></head></html>\r\n", 53);
const ByteArray http_response_403("<html><head><h1>403 Access Denied !</h1></head></html>\r\n", 57);
const ByteArray http_response_200("<html><head><h1>Success !</h1></head></html>\r\n", 47);


HttpServer::HttpServer(int port, Directory* cwd, const std::string& hostIp)
	: _socket(new Socket(Socket::Stream, Socket::TCP))
	, _buffer(new ByteArray(1))
	, _endPoint(new IPEndPoint(hostIp, port, IPEndPoint::IPv4))
	, _fileManager(new HttpServerFileManager(new Directory("www"), cwd))
	, _modMan(new ModulesManager)
{
	_socket->Bind(_endPoint);
	_socket->setSocketCallBacks(this);
}
void HttpServer::Start()
{
	_modMan->loadAll();
	_socket->Listen(128);
	_socket->AcceptAsync();
}
void HttpServer::acceptConnectionCallBack(Socket* socket)
{
	if (socket == nullptr)
		return;
	log_WriteLine<HttpServer>(Debug, "Connection accepted from " + socket->ipEndPoint()->toString());
	socket->setSocketCallBacks(this);
	socket->ReadAsync(_buffer);
}
bool HttpServer::dataReceiveCallBack(Socket* socket, ByteArray* buffer, void* data, int count)
{
	if (count > 0)
	{
		log_WriteLine<HttpServer>(Debug, "From " + socket->ipEndPoint()->toString());
		log_WriteLine<HttpServer>(Debug, "Received data length = " + std::to_string(count));


		int newLineCount = 0;
		ByteArray* a = new ByteArray(1);
		ByteArray* rawData = new ByteArray;
		bool isRawData = false;
		std::string headers;
		headers += buffer->operator[](0);
		while (socket->dataAvailable() > 0 && newLineCount != 4)
		{
			socket->Read(a, 1);


			if (a->operator[](0) == '\r' || a->operator[](0) == '\n')
			{
				newLineCount++;
				headers += a->operator[](0);
				continue;
			}
			else
			{
				newLineCount = 0;
			}


			if (isRawData)
				rawData->append(a->dataPtr(), 1);
			else
			{
				headers += a->operator[](0);
			}
		}
		auto ss = new SocketStream(socket, Stream::ReadOnly);
		auto resp = processIncomingRequest(HttpRequest(headers, ss));
		delete ss;
		log_WriteLine<HttpServer>(Debug, "Response : " + resp->statusCodeString());

		socket->Write(resp->serialize());
		socket->Write(*resp->dataStream()->dataPtr());

		delete rawData;
		delete a;
		delete resp;
	}
	socket->destory();
	return false;
}
HttpResponse* HttpServer::processIncomingRequest(const HttpRequest& request)
{
	//std::string realPath = stringReplace(request.path(), "/", "\\");

	unsigned rd = request.dataStream()->availableBytes();
	if (rd > 0)
	{
		ByteArray ba;
		if (request.dataStream()->read(&ba, rd) > 0)
		{
			log_WriteLine<HttpServer>(Debug, ba.toStdString());
		}
	}


	std::string realPath = request.path();
	if (realPath == "/")
	{
		realPath += "index.html";
	}

	log_WriteLine<HttpServer>(Debug, "Requesting path = " + request.path());
	
	if (_modMan->modCount() > 0)
	{
		HttpResponse* response = new HttpResponse(new MemoryStream(Stream::ReadWrite));
		for(Module* mod : _modMan->modules())
		{
			if (mod->canHandle(request))
			{
				mod->process(request, response);
				if (mod->resultConfig() == Module::Return)
				{
					return response;
				}
			}
		}
	}
	HttpResponse* resp = new HttpResponse(new MemoryStream(Stream::ReadWrite));
	resp->setHttpVersion(V1_1);
	realPath = _fileManager->get(realPath, Directory::ReadAll);
	if (realPath == "")
	{
		log_WriteLine<HttpServer>(Debug, "No Access to this directory !");
		resp->setStatusCode(403);
		resp->dataStream()->write(&http_response_403);
		return resp;
	}



	//realPath = realPath.erase(0, 1);
	std::fstream stream(realPath, std::ios::in);
	if (stream.fail())
	{
		log_WriteLine<HttpServer>(Error, "Cannot open file : " + realPath);
		resp->setStatusCode(404);
		resp->dataStream()->write(&http_response_404);
	}
	else
	{
		log_WriteLine<HttpServer>(Info, "Reading file : " + realPath);
		ByteArray data;
		while (!stream.eof())
		{
			char buf[1024];
			int count = stream.read(buf, sizeof buf).gcount();
			data.append(buf, count);
		}
		stream.close();
		resp->setStatusCode(200);
		resp->dataStream()->write(&data);
	}
	return resp;
}
void HttpServer::setRootDir(Directory* rootDir)
{
	_fileManager->setRoot(rootDir);
}
void HttpServer::addModule(Module* mod)
{
	_modMan->addModule(mod);
}