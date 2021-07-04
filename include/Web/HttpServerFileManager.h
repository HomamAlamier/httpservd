#ifndef HTTPSERVER_FILEMANAGER_H
#define HTTPSERVER_FILEMANAGER_H


#include <string>
#include <vector>
#include <Core/IO/Directory.h>
class HttpServerFileManager
{
public:
	HttpServerFileManager(Directory* rootDir);

	void addDirectory(Directory* dir);
	void removeDirectory(const std::string& dir);
	bool canAccess(const std::string& path, Directory::DirectoryOperation oper);
	std::string get(const std::string& path, Directory::DirectoryOperation oper);

	void setRoot(Directory* root);
private:
	Directory* _root;
	std::vector<Directory*> _dirs;
};


#endif // !HTTPSERVER_FILEMANAGER_H


