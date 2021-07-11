#include <Core/IO/Directory.h>
#include <Log/Log.h>

Directory::Directory(const std::string& dir)
	: _dir(dir)
	, _perms()
{
}
void Directory::addPermission(DirectoryOperation oper, bool value)
{
	bool f = (_perms.find(oper) != _perms.end());
	if (f)
	{
		_perms[oper] = value;
	}
	else
	{
		_perms.insert(std::pair<DirectoryOperation, bool>(oper, value));
	}
}
bool Directory::can(DirectoryOperation oper) const
{
	bool f = (_perms.find(oper) != _perms.end());
	if (f)
		return _perms.at(oper);
	else
		return false;
}