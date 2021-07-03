
#include "Server.h"
#include <Log/Log.h>
#include <chrono>
#include <thread>
#include <Web/HttpServer.h>



int main()
{
	HttpServer server(8080);
	server.Start();
	while (true)	
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	return 0;
}
