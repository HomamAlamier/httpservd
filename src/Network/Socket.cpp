#include <Network/Socket.h>
#include <Globals.h>
#include <thread>
#include <Core/ByteArray.h>
#include <Log/Log.h>
#include <vector>
#include <Core/Threading/Task.h>
#include <Core/Threading/TaskPool.h>
#include <mutex>
#ifdef _WIN32
	#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN
	#endif
	#include <windows.h>
	#include <winsock2.h>
	#include <ws2tcpip.h>
	#include <iphlpapi.h>
	#include <stdio.h>
	#pragma comment(lib, "Ws2_32.lib")

	typedef int socklen_t;
	char* lastErr()
	{
		return strerror(WSAGetLastError());
	}
#else
	#include <stdio.h>
	#include <stdlib.h>
	#include <unistd.h>
	#include <memory.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <netdb.h>
	#include <arpa/inet.h>
	#include <sys/ioctl.h>

	#define SOCKET_ERROR -1 
	void ZeroMemory(void* ptr, size_t size)
	{
		memset(ptr, 0, size);
	}
	int closesocket(socket_tt sock)
	{
		return close(sock);
	}
	char* lastErr()
	{
		return strerror(errno);
	}
#endif // _WIN32

void* get_in_addr(struct sockaddr* sa)
{
	if (sa->sa_family == AF_INET)
		return &(((struct sockaddr_in*)sa)->sin_addr);
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

const int _worker_count_ = 4;

static std::vector<Socket*> _accept_queue_;
static std::vector<Socket*> _receive_queue_;
static std::vector<ByteArray*> _receive_buffer_queue_;
static std::mutex _accept_queue_mutex_;
static std::mutex _receive_queue_mutex_;
static std::thread* _receive_workers_[_worker_count_];
static std::thread* _accept_workers_[_worker_count_];
static Socket* _receive_worker_context_[_worker_count_];
static Socket* _accept_worker_context_[_worker_count_];
static bool _receive_worker_running_[_worker_count_];
static bool _accept_worker_running_[_worker_count_];

void _async_accept_socket_(Socket* sock)
{
	_accept_queue_mutex_.lock();
	_accept_queue_.push_back(sock);
	_accept_queue_mutex_.unlock();
}
void _async_receive_socket_(Socket* sock)
{
	_receive_queue_mutex_.lock();
	_receive_queue_.push_back(sock);
	_receive_queue_mutex_.unlock();
}
void _async_accept_invoke_callback_(int pid, Socket* parent, Socket* child)
{
	_accept_worker_running_[pid] = true;
	parent->_callbacksPtr->acceptConnectionCallBack(child);
	_accept_worker_running_[pid] = false;
}
void _async_accept_receive_callback_(int pid, Socket* parent, ByteArray* buffer, void* data, int size)
{
	_receive_worker_running_[pid] = true;
	parent->_callbacksPtr->dataReceiveCallBack(parent, buffer, data, size);
	_receive_worker_running_[pid] = false;
}
int _async_accept_free_worker_()
{
	for(int i = 0; i < _worker_count_; ++i)
	{
		if (_accept_workers_[i] == nullptr)
		{
			return i;
		}
		else if (!_accept_workers_[i]->joinable())
		{
			delete _accept_workers_[i];
			return i;
		}
	}
	while(true)
	{
		for(int i = 0; i < _worker_count_; ++i)
		{
			if (!_accept_worker_running_[i])
			{
				_accept_workers_[i]->join();
				delete _accept_workers_[i];
				return i;
			}
		}
	}
	return -1;
}
int _async_receive_free_worker_()
{
	for(int i = 0; i < _worker_count_; ++i)
	{
		if (_receive_workers_[i] == nullptr)
		{
			return i;
		}
		else if (!_receive_workers_[i]->joinable())
		{
			delete _receive_workers_[i];
			return i;
		}
	}
	while(true)
	{
		for(int i = 0; i < _worker_count_; ++i)
		{
			if (!_receive_worker_running_[i])
			{
				_receive_workers_[i]->join();
				delete _receive_workers_[i];
				return i;
			}
		}
	}
	return -1;
}
void _async_accept_handler_()
{
	while (true)
	{
		_accept_queue_mutex_.lock();
		for(int i = 0; i < _accept_queue_.size(); ++i)
		{
			auto sock = _accept_queue_[i];
			auto s = sock->Accept();
			int free_worker = _async_accept_free_worker_();
			_accept_workers_[free_worker] = new std::thread(_async_accept_invoke_callback_, i, sock, s);

			if (sock->status() != Socket::Listening || sock->destroyed())
			{
				sock->destory();
				_accept_queue_.erase(_accept_queue_.begin() + i);
				break;
			}
		}
		_accept_queue_mutex_.unlock();
		std::this_thread::sleep_for(std::chrono::microseconds(50));
	}
}
void _async_receive_handler_()
{
	while (true)
	{
		_receive_queue_mutex_.lock();
		for(int i = 0; i < _receive_queue_.size(); ++i)
		{
			auto sock = _receive_queue_[i];

			bool inWork = false;
			for(int i = 0; i < _worker_count_; ++i)
			{
				auto t = _receive_workers_[i];
				if (t != nullptr && _receive_worker_context_[i] == sock && t->joinable())
				{
					inWork = true;
					break;
				}
			}
			if (inWork)
				continue;

			auto size = sock->Read(sock->_receiveBuffer, sock->_receiveBuffer->size());
			int free_worker = _async_receive_free_worker_();
			_receive_workers_[free_worker] = new std::thread(_async_accept_receive_callback_, i, sock, sock->_receiveBuffer, sock->_receiveDataPtr, size);

			if (sock->status() != Socket::Connected || sock->destroyed())
			{
				sock->destory();
				_receive_queue_.erase(_receive_queue_.begin() + i);
				break;
			}
		}
		_receive_queue_mutex_.unlock();
		std::this_thread::sleep_for(std::chrono::microseconds(50));
	}
	
}
static std::thread _async_accept_worker_(_async_accept_handler_);
static std::thread _async_receive_worker_(_async_receive_handler_);


static bool _os_socket_inited_ = false;
static int _socket_instance_count = 0;

bool _chk_socket_error_(socket_tt s)
{
	int error_code;
	socklen_t error_code_size = sizeof(error_code);
	getsockopt(s, SOL_SOCKET, SO_ERROR, &error_code, &error_code_size);
	if (error_code != 0)
	{
		return true;
	}
	else return false;
}


Socket::Socket(SocketType type, Protocol protocol) 
	: _endPoint(nullptr)
	, _type(type)
	, _pro(protocol)
	, _st(None)
	, _sockPtr(0)
	, _nIp(nullptr)
	, _running(false)
	, _acceptThread(nullptr)
	, _acceptThreadAlive(false)
	, _receiveThread(nullptr)
	, _receiveThreadAlive(false)
	, _receiveDataPtr(nullptr)
	, _callbacksPtr(nullptr)
	, _connection_ms(0)
	, _destroyed(false)
{
	if (!_os_socket_inited_)
	{
		#ifdef _WIN32
			WSADATA data;
			int iResult = WSAStartup(MAKEWORD(2, 2), &data);
			if (iResult == 0)
			{
				#ifdef LOG_LEVEL_5
					log_WriteLine<Socket>(Debug, "Ws2_32 Lib inited !");
					log_WriteLine<Socket>(Debug, data.szSystemStatus);
					log_WriteLine<Socket>(Debug, data.szDescription);
				#endif // LOG_LEVEL_5
			}
		#endif // WIN32
		_os_socket_inited_ = true;
	}
	_socket_instance_count++;
}
Socket::~Socket()
{
	destory();
}
bool Socket::endPointToNative()
{
	if (_nIp != nullptr)
	{
		freeaddrinfo(_nIp);
	}
	struct addrinfo hints;
	ZeroMemory(&hints, sizeof hints);
	hints.ai_family = _endPoint->ipType();
	hints.ai_socktype = _type;
	hints.ai_protocol = _pro;
	int iResult = getaddrinfo(_endPoint->ip().c_str(), std::to_string(_endPoint->port()).c_str(), &hints, &_nIp);
	if (iResult != 0)
	{
		log_WriteLine<Socket>(Error, "getaddrinfo failed errcode = " + std::to_string(iResult));
		return false;
	}
	return true;
}
void Socket::Bind(const IPEndPoint* endpoint)
{
	_endPoint = endpoint;
}
bool Socket::Listen(int backlog)
{
	if (_endPoint == nullptr)
		return false;

	if (_sockPtr != 0)
		closesocket(_sockPtr);

	_sockPtr = socket(_endPoint->ipType(), _type, _pro);
	int t = 1;
	if (setsockopt(_sockPtr, SOL_SOCKET, SO_REUSEADDR, &t, sizeof(t)) < 0)
		log_WriteLine<Socket>(Error, "setsockopt(SO_REUSEADDR) failed");
	if (setsockopt(_sockPtr, SOL_SOCKET, SO_REUSEPORT, &t, sizeof(t)) < 0)
		log_WriteLine<Socket>(Error, "setsockopt(SO_REUSEPORT) failed");
	#ifdef _WIN32
		DWORD tv = 3 * 1000;
	#else
		struct timeval tv;
		tv.tv_sec = 3;
		tv.tv_usec = 0;
	#endif // DEBUG
	if (setsockopt(_sockPtr, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv))
		log_WriteLine<Socket>(Error, "setsockopt(SO_RCVTIMEO) failed");
		
	if (!endPointToNative())
		return false;

	log_WriteLine<Socket>(Debug, "Binding...");
	if (bind(_sockPtr, _nIp->ai_addr, int(_nIp->ai_addrlen)) == SOCKET_ERROR)
	{
		log_WriteLine<Socket>(Error, "Socket bind failed !");
		log_WriteLine<Socket>(Error, lastErr());
		return false;
	}

	log_WriteLine<Socket>(Debug, "Listening...");
	if (listen(_sockPtr, backlog) == SOCKET_ERROR)
	{
		log_WriteLine<Socket>(Error, "Socket listen failed !");
		log_WriteLine<Socket>(Error, lastErr());
		return false;
	}
	_st = Listening;
	_running = true;
	return true;
}
bool Socket::Connect(const IPEndPoint& ipEndPoint)
{
	if (_sockPtr != 0)
		closesocket(_sockPtr);

		
	struct sockaddr_in addr;
	addr.sin_family = int(ipEndPoint.ipType());
	addr.sin_port = htons(ipEndPoint.port());

	if (inet_pton(ipEndPoint.ipType(), ipEndPoint.ip().c_str(), &addr.sin_addr) <= 0)
	{
		log_WriteLine<Socket>(Error, "Connect: inet_pton failed !");
		log_WriteLine<Socket>(Error, lastErr());
		return false;
	}


	_sockPtr = socket(ipEndPoint.ipType(), _type, _pro);

	

	log_WriteLine<Socket>(Debug, "Connecting...");
	if (connect(_sockPtr, (struct sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR)
	{
		log_WriteLine<Socket>(Error, "Socket connect failed !");
		log_WriteLine<Socket>(Error, lastErr());
		return false;
	}
	_endPoint = new IPEndPoint(ipEndPoint);

	_st = Connected;
	_running = true;
	return true;
}
void Socket::close()
{
	_running = false;
	_st = None;

	if (_sockPtr != 0)
	{
		log_WriteLine<Socket>(Debug, "Closing socket (" + std::to_string(_sockPtr) + ")");
		closesocket(_sockPtr);
		_sockPtr = 0;
	}
	if (_nIp != nullptr)
	{
		freeaddrinfo(_nIp);
		_nIp = nullptr;
	}

}
Socket* Socket::Accept()
{
	socket_tt client = 0;
	struct sockaddr addr;
	socklen_t sz_addr = sizeof addr;
	if ((client = accept(_sockPtr, &addr, &sz_addr)) != 0)
	{

		char buf[INET6_ADDRSTRLEN];
		void* addr_in = get_in_addr(&addr);
		inet_ntop(_endPoint->ipType(), addr_in, buf, sizeof buf);
		log_WriteLine<Socket>(Debug, std::string("Accepting connection from ") + buf + "...");

		int port = (_endPoint->ipType() == IPEndPoint::IPv4) 
			? ((struct sockaddr_in*)addr_in)->sin_port 
			: ((struct sockaddr_in6*)addr_in)->sin6_port;

		Socket* s = new Socket(_type, _pro);
		s->_sockPtr = client;
		s->_endPoint = new IPEndPoint(buf, port, _endPoint->ipType());
		s->_st = Socket::Connected;
		return s;
	}
	else
	{
		log_WriteLine<Socket>(Error, "Error accepting connection !");
		return nullptr;
	}
}
void Socket::AcceptAsync()
{
	if (_acceptThread != nullptr || _callbacksPtr == nullptr)
		return;
	_async_accept_socket_(this);
}
void Socket::acceptConnection()
{
	/*_acceptThreadAlive = true;
	while (_running)
	{
		Socket* s = Accept();
		_callbacksPtr->acceptConnectionCallBack(s);
	}
	_acceptThreadAlive = false;*/
}
int Socket::Write(const ByteArray& byteArray)
{
	if (_sockPtr == 0 || _st != Connected)
		return 0;
	int count = send(_sockPtr, byteArray.dataPtr(), byteArray.size(), MSG_NOSIGNAL);
	if (_chk_socket_error_(_sockPtr))
	{
		close();
		return 0;
	}
	return count;
}
int Socket::Read(ByteArray* byteArray, int size)
{
	if (_sockPtr == 0 || _st != Connected || byteArray == nullptr)
		return 0;

	if (byteArray->size() == 0)
	{
		byteArray->allocate(size);
	}
	int count = recv(_sockPtr, byteArray->dataPtr(), size, MSG_NOSIGNAL);
	if (_chk_socket_error_(_sockPtr))
	{
		close();
		return 0;
	}
	return count;
}
void Socket::ReadAsync(ByteArray* buffer, void* data)
{
	if (_receiveThread != nullptr || _callbacksPtr == nullptr)
		return;
	_receiveBuffer = buffer;
	_receiveDataPtr = data;
	//_running = true;
	_async_receive_socket_(this);
}
void Socket::receiveData()
{
	/*_receiveThreadAlive = true;
	size_t x = registerThread(_receiveThread);
	while (_running && _sockPtr > 0 && _st == Connected && _connection_ms < 3000)
	{
		int c = 0;
		if (dataAvailable() > 0)
			c = Read(_receiveBuffer, _receiveBuffer->size());
		if (c > 0 && _running)
		{

			task_pool_add(new Task(std::bind(&Socket::receiveData, this, this, _receiveBuffer, _receiveDataPtr, c)));
			_connection_ms = 0;
		}
		std::this_thread::sleep_for(std::chrono::microseconds(10));
		_connection_ms += 10;
	}
	if (_connection_ms == 3000)
	{
		close();
		destory();
	}
	_receiveThreadAlive = false;
	setState(x, 0);*/
}
void Socket::setSocketCallBacks(SocketCallBacks* ptr)
{
	if (ptr != nullptr)
	{
		_callbacksPtr = ptr;
	}
}
int Socket::dataAvailable() const
{
	if (_st != Connected || _destroyed)
		return 0;
	#ifdef _WIN32
		u_long count = 0;
		ioctlsocket(_sockPtr, FIONREAD, &count);
		return int(count);
	#else
		u_long count = 0;
		ioctl(_sockPtr, FIONREAD, &count);
		return int(count);
	#endif // _WIN32

}
void Socket::destory()
{
	if (_destroyed)
		return;
	log_WriteLine<Socket>(Debug, "~");
	_socket_instance_count--;
	close();
	if (_socket_instance_count == 0 && _os_socket_inited_)
	{
		_os_socket_inited_ = false;
		#ifdef _WIN32
			WSACleanup();
		#endif // _WIN32
	}
	_destroyed = true;
}