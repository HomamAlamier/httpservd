#include <Web/HttpServerFileManager.h>
#include <Log/Log.h>

HttpServerFileManager::HttpServerFileManager(Directory* rootDir)
	: _root(rootDir)
{
	_dirs.push_back(rootDir);
}

void HttpServerFileManager::setRoot(Directory* root)
{
	if (root == nullptr)
		return;

	removeDirectory(_root->path());
	_root = root;
}


void HttpServerFileManager::addDirectory(Directory* dir)
{
	if (dir != nullptr && !dir->isEmpty())
		_dirs.push_back(dir);
}
void HttpServerFileManager::removeDirectory(const std::string& dir)
{
	for (int i = 0; i < _dirs.size(); ++i)
	{
		if (_dirs[i]->path() == dir)
		{
			_dirs.erase(_dirs.begin() + i);
			return;
		}
	}
}
bool HttpServerFileManager::canAccess(const std::string& path, Directory::DirectoryOperation oper)
{
	std::string p = path.substr(0, path.find_last_of('\\') + 1);
	p = _root->path() + p;

	log_WriteLine<HttpServerFileManager>(log_dbg, "Checking permissions for " + p);

	if (_root->path() == path)
	{
		return _root->can(oper);
	}

	for (int i = 0; i < _dirs.size(); ++i)
	{
		const auto* dir = _dirs[i];
		if (dir->path() == p)
			return dir->can(oper);
	}
	return false;
}
std::string HttpServerFileManager::get(const std::string& path, Directory::DirectoryOperation oper)
{
	if (canAccess(path, oper))
	{
		return _root->path() + path;
	}
	return "";
}