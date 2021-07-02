#include <Object.h>
Object::Object(const char* classname)
	: _className(classname)
{
	_logInstance = new Log(classname);
#ifdef LOG_DEBUG 
#ifdef LOG_LEVEL_5
	_logInstance->Debug("Creating instance...");
#endif
#endif // LOG_DEBUG
}
std::string Object::toString() const
{
	return std::string(_className) + "()";
}