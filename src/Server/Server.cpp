
#include "Server.h"
#include <Log/Log.h>
#include <chrono>
#include <thread>
#include <Web/HttpServer.h>
#include <Core/IO/Directory.h>
#include <filesystem>
#include <Web/Modules/Module.h>
#include <Core/IO/MemoryStream.h>
#include <Core/ByteArray.h>
#include <Web/HttpRequest.h>
#include <Web/HttpResponse.h>
struct Main{};
int main()
{
    ByteArray ba;
	MemoryStream ms(&ba, Stream::ReadWrite);
	ms.write("HelloWorld", 11);
    HttpRequest r("GET /redirect HTTP/1.1\r\n", &ms);



	auto cwd = std::filesystem::current_path().string();
	log_WriteLine<Main>(Debug, "Current Dir = " + cwd);

	HttpServer server(8089, new Directory(cwd));

	Directory dir("www");
	dir.addPermission(Directory::ReadAll, true);
	server.setRootDir(&dir);
	server.addModule(new Module(cwd + "/libHelloModule.so"));
	server.Start();
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
	
	return 0;
}
