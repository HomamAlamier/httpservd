#include <Network/IPEndPoint.h>
#include <Globals.h>
#include <Log/Log.h>


IPEndPoint::IPEndPoint(const std::string& ip, int port, IPType type)
	: _ip(ip)
	, _port(port)
	, _type(type)

{
#ifdef LOG_LEVEL_5
	log_WriteLine<IPEndPoint>(log_dbg, DUMP_VAR(port));
	log_WriteLine<IPEndPoint>(log_dbg, DUMP_VAR(type));
	log_WriteLine<IPEndPoint>(log_dbg, DUMP_STRING(ip));
#endif // LOG_LEVEL_5
}
std::string IPEndPoint::toString() const
{
	return _ip + ":" + std::to_string(_port);
}