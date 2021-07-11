#include <Web/HttpServerFileManager.h>
#include <Log/Log.h>

HttpServerFileManager::HttpServerFileManager(Directory* rootDir, Directory* cwd)
	: _root(rootDir)
	, _cwd(cwd)
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
	std::string p = path.substr(0, path.find_last_of('/') + 1);
	p = _root->path() + p;

	log_WriteLine<HttpServerFileManager>(Debug, "Checking permissions for " + p);


	std::string tmp1 = _root->path()[_root->path().size() - 1] == '/' ? _root->path().substr(0, _root->path().size() - 1) : _root->path();
	std::string tmp2 = p[p.size() - 1] == '/' ? p.substr(0, p.size() - 1) : p;
	if (tmp1 == tmp2)
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
		return _cwd->path() + "/" + _root->path() + path;
	}
	return "";
}