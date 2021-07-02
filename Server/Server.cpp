
#include "Server.h"
#include <Log/Log.h>
#include <Network/Socket.h>
#include <chrono>
#include <thread>
#include <Core/ByteArray.h>
const ByteArray http_response_200("HTTP/1.1 200 OK\r\nServer: libServer\r\nContent-Type: text/html;\r\n\r\n<html><head><h1>Success !</h1></head></html>\r\n\0", 109);

Log ll("Main");
ByteArray ba(1024);


void receive(Socket* s, int rcount)
{
	if (rcount > 0)
	{
		ll.Debug("From " + s->ipEndPoint()->ip());
		ll.Debug("Received data length = " + std::to_string(rcount));
		ll.Debug(std::string(ba.dataPtr(), rcount));
		s->Write(http_response_200);
	}
	s->Close();
}

void accept(Socket* s)
{
	s->ReadAsync(&ba, receive);
}

int main()
{
	IPEndPoint endpoint("127.0.0.1", 8080, IPEndPoint::IPv4);
	Socket sock(Socket::Stream, Socket::TCP);
	sock.Bind(&endpoint);
	sock.Listen(5);
	sock.AcceptAsync(accept);
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	return 0;
}
