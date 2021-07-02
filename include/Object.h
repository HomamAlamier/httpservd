#ifndef OBJECT_H
#define OBJECT_H
#include <string>
#include <Globals.h>
#include <Log/Log.h>
class Object
{
public:
	Object(const char* classname);
	virtual std::string toString() const;
	Log* log() { return _logInstance; }
	const char* className() { return _className; }
private:
	Log* _logInstance;
	const char* _className;
};

#endif // OBJECT_H
