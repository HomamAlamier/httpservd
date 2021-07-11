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

const ByteArray http_response_404("<html><head><h1>404 Not Found !</h1></head></html>\r\n", 53);
const ByteArray http_response_403("<html><head><h1>403 Access Denied !</h1></head></html>\r\n", 57);
const ByteArray http_response_200("<html><head><h1>Success !</h1></head></html>\r\n", 47);


HttpServer::HttpServer(int port, Directory* cwd, const std::string& hostIp)
	: _socket(new Socket(Socket::Stream, Socket::TCP))
	, _buffer(new ByteArray(1))
	, _endPoint(new IPEndPoint(hostIp, port, IPEndPoint::IPv4))
	, _fileManager(new HttpServerFileManager(new Directory("www"), cwd))
{
	_socket->Bind(_endPoint);
	_socket->setSocketCallBacks(this);
}
void HttpServer::Start()
{
	_socket->Listen(5);
	_socket->AcceptAsync();
}
void HttpServer::acceptConnectionCallBack(Socket* socket)
{
	socket->setSocketCallBacks(this);
	socket->ReadAsync(_buffer);
}
void HttpServer::dataReceiveCallBack(Socket* socket, ByteArray* buffer, void* data, int count)
{
	if (count > 0)
	{
		log_WriteLine<HttpServer>(Debug, "From " + socket->ipEndPoint()->toString());
		log_WriteLine<HttpServer>(Debug, "Received data length = " + std::to_string(count));


		int newLineCount = 0;
		ByteArray a(1);
		ByteArray rawData;
		bool isRawData = false;
		std::string headers;
		headers += buffer->operator[](0);
		while (socket->dataAvailable() > 0)
		{
			socket->Read(&a, 1);
			if (newLineCount == 4)
				isRawData = true;

			if (a[0] == '\r' || a[0] == '\n')
			{
				newLineCount++;
				headers += a[0];
				continue;
			}
			else
			{
				newLineCount = 0;
			}


			if (isRawData)
				rawData << a[0];
			else
			{
				headers += a[0];
			}
		}
		auto resp = processIncomingRequest(HttpRequest(headers, &rawData));

		log_WriteLine<HttpServer>(Debug, "Response : " + resp.statusCodeString());

		socket->Write(resp.serialize());
		socket->Write(*resp.data());

	}

	socket->close();
	socket->destory();
}
HttpResponse HttpServer::processIncomingRequest(const HttpRequest& request)
{
	//std::string realPath = stringReplace(request.path(), "/", "\\");
	std::string realPath = request.path();
	if (realPath == "/")
	{
		realPath += "index.html";
	}

	log_WriteLine<HttpServer>(Debug, "Requesting path = " + request.path());

	HttpResponse resp;
	resp.setHttpVersion(V1_1);
	realPath = _fileManager->get(realPath, Directory::ReadAll);
	if (realPath == "")
	{
		log_WriteLine<HttpServer>(Debug, "No Access to this directory !");
		resp.setStatusCode(403);
		resp.setData(http_response_403);
		return resp;
	}



	//realPath = realPath.erase(0, 1);
	std::fstream stream(realPath, std::ios::in);
	if (stream.fail())
	{
		log_WriteLine<HttpServer>(Error, "Cannot open file : " + realPath);
		resp.setStatusCode(404);
		resp.setData(http_response_404);
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
		resp.setStatusCode(200);
		resp.setData(data);
	}
	return resp;
}
void HttpServer::setRootDir(Directory* rootDir)
{
	_fileManager->setRoot(rootDir);
}