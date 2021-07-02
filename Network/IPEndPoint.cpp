#include <Network/IPEndPoint.h>

IPEndPoint::IPEndPoint(const std::string& ip, int port, IPType type)
	: Object("IPEndPoint")
	, _ip(ip)
	, _port(port)
	, _type(type)

{
#ifdef LOG_LEVEL_5
	log()->Debug(DUMP_VAR(port));
	log()->Debug(DUMP_VAR(type));
	log()->Debug(DUMP_STRING(ip));
#endif // LOG_LEVEL_5
}