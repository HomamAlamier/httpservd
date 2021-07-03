#include <Web/HttpServer.h>
#include <Core/ByteArray.h>
#include <Network/Socket.h>
#include <Web/HttpRequest.h>
#include <Web/HttpResponse.h>
const ByteArray http_response_200("<html><head><h1>Success !</h1></head></html>\r\n\0", 109);



HttpServer::HttpServer(int port)
	: Object("HttpServer")
	, _socket(new Socket(Socket::Stream, Socket::TCP))
	, _buffer(new ByteArray(1024))
	, _endPoint(new IPEndPoint("0.0.0.0", port, IPEndPoint::IPv4))
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
void HttpServer::dataReceiveCallBack(Socket* socket, const ByteArray* buffer, int count)
{
	if (count > 0)
	{
		log()->Debug("From " + socket->ipEndPoint()->toString());
		log()->Debug("Received data length = " + std::to_string(count));
		HttpRequest req(std::string(buffer->dataPtr(), count));
		if (req.path() == "/")
		{
			HttpResponse resp;
			resp.setStatusCode(200);
			resp.setHttpVersion(V1_1);
			resp.addHeader("Content-Type", "text/html");
			std::string r = resp.serialize();
			r += http_response_200.toStdString();
			ByteArray a(r.data(), r.size());
			socket->Write(a);
		}
		socket->Close();
	}
}