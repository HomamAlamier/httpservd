
#include "Server.h"
#include <Log/Log.h>
#include <Network/Socket.h>
int main()
{
	IPEndPoint endpoint("127.0.0.1", 8080, IPEndPoint::IPv4);
	Socket sock(Socket::Stream, Socket::TCP);
	sock.Bind(&endpoint);
	sock.Listen(5);
	return 0;
}
