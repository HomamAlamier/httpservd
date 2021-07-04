#ifndef DIRECTORY_H
#define DIRECTORY_H

#include <map>
#include <string>
class Directory
{
public:
	enum DirectoryOperation
	{
		All,
		List,
		ReadAll
	};
	Directory(const std::string& dir);

	bool isEmpty() const { return _dir == ""; }
	const std::string& path() const { return _dir; }
	bool can(DirectoryOperation oper) const;


	void addPermission(DirectoryOperation oper, bool value);
private:
	std::string _dir;
	std::map<DirectoryOperation, bool> _perms;
};

#endif // !DIRECTORY_H
