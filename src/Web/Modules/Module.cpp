#include <Web/Modules/Module.h>
#include <Web/HttpRequest.h>
#include <Web/HttpResponse.h>
#include <Log/Log.h>
#include <Core/IO/Stream.h>
#include <string.h>
#include <Web/Modules/Api/Api.h>
#include <Core/IO/MemoryStream.h>
#include <Core/ByteArray.h>

// Shared Library Module Functions
typedef int(*module_func_init)();
typedef int(*module_func_free)();
typedef int(*module_func_can_handle)(const http_request_t*);
typedef int(*module_func_process_request)(http_request_t*, http_response_t*, const char*);
typedef int(*module_func_get_config)(config_t*);

template <class T>
struct func_result
{
    bool valid;
    T* data;
};

template <class _Func, class _Ret, class ..._Args>
func_result<_Ret> invokeFunction_args(void* func, _Args... args)
{
    if (func == nullptr)
        return {false, nullptr};
    _Ret* ret = new _Ret(_Func(func)(args...));
    return {true, ret};
}
template <class _Func, class _Ret, class ..._Args>
func_result<_Ret> invokeFunction_args_ptr(void* func, _Args... args)
{
    if (func == nullptr)
        return {false, nullptr};
    _Ret* ret = _Func(func)(args...);
    return {true, ret};
}
template <class _Func, class _Ret>
func_result<_Ret> invokeFunction(void* func)
{
    if (func == nullptr)
        return {false, nullptr};
    _Ret* ret = new _Ret(_Func(func)());
    return {true, ret};
}

#ifdef _WIN32

#else
    #include <dlfcn.h>
    void* sharedLibrary_load(const char* path)
    {
        return dlopen(path, RTLD_NOW);
    }
    void* sharedLibrary_resolveFunction(void* lib, const char* functionName)
    {
        return dlsym(lib, functionName);
    }
#endif



int stream_read_func(void* p, char* b, int s)
{
    return ((Stream*)(p))->read(b, s);
}
int stream_write_func(void* p, const char* b, int s)
{
    return ((Stream*)(p))->write(b, s);
}
int stream_get_len_func(void* p)
{
    return ((Stream*)(p))->availableBytes();
}
int stream_seek_func(void* p, int to)
{
    return ((Stream*)(p))->seek(to);
}
template <class T>
void func_result_free(func_result<T>* p)
{
    delete p->data;
}
void http_request_t_free(http_request_t* p)
{
    for(int i = 0; i < p->headers->size; ++i)
    {
        delete p->headers->data[i];
    }
    delete[] p->headers->data;
    delete p->headers;
    p->in_stream->stream_ptr = nullptr;
    delete p->in_stream;
    delete p;
}
void http_response_t_free(http_response_t* p)
{
    for(int i = 0; i < p->headers->size; ++i)
    {
        delete p->headers->data[i];
    }
    delete[] p->headers->data;
    delete p->headers;
    p->out_stream->stream_ptr = nullptr;
    delete p->out_stream;
    delete p;
}
http_request_t* HttpRequest_to_C(const HttpRequest& request)
{
    http_request_t* r = new http_request_t;
    r->request_method = request.method();
    r->http_version = request.httpVersion();
    r->path = request.path().c_str();
    r->in_stream = new stream_t;
    r->in_stream->stream_ptr = request.dataStream();
    r->in_stream->get_length = stream_get_len_func;
    r->in_stream->read = stream_read_func;
    r->in_stream->write = stream_write_func;
    r->in_stream->seek = stream_seek_func;
    
    r->headers = new headers_t;
    r->headers->data = new tuple_t*[request.headers().size()];
    r->headers->size = request.headers().size();

    int i = 0;
    for(const auto item : request.headers())
    {
        r->headers->data[i] = new tuple_t;
        r->headers->data[i]->key = item.first.c_str();
        r->headers->data[i]->value = item.second.c_str();
        ++i;    
    }

    return r;
}
http_response_t* HttpResponse_to_C(HttpResponse* resp)
{
    http_response_t* r = new http_response_t;
    r->http_version = resp->httpVesion();
    r->response_code = resp->statusCode();
    r->out_stream = new stream_t;
    r->out_stream->stream_ptr = resp->dataStream();
    r->out_stream->get_length = stream_get_len_func;
    r->out_stream->read = stream_read_func;
    r->out_stream->write = stream_write_func;
    r->out_stream->seek = stream_seek_func;

    r->headers = new headers_t;
    r->headers->data = new tuple_t*[resp->headers().size()];
    r->headers->size = resp->headers().size();

    int i = 0;
    for(const auto item : resp->headers())
    {
        r->headers->data[i] = new tuple_t;
        r->headers->data[i]->key = item.first.c_str();
        r->headers->data[i]->value = item.second.c_str();
        ++i;
    }

    return r;
}

Module::Module(const std::string& path)
    : _modPath(path)
    , _loaded(false)
    , _inited(false)
    , _funcInit(nullptr)
    , _funcFree(nullptr)
    , _funcCanHandle(nullptr)
    , _funcProcessRequest(nullptr)
{
}
Module::~Module()
{
    if (_inited && _funcFree)
    {
        invokeFunction<module_func_free, int>(_funcFree);
    }
    if (_loaded)
    {
        dlclose(_ptr);
    }
}
bool Module::load()
{
    if (_loaded)
        return false;
    log_WriteLine<Module>(Debug, "Loading module " + _modPath);
    _ptr = sharedLibrary_load(_modPath.c_str());
    if (!_ptr)
    {
        log_WriteLine<Module>(Error, "Loading module " + _modPath + " failed ! (" + std::string(strerror(errno)) + ")");
        _loaded = false;
        return false;
    }
    _loaded = true;
    return true;
}

int Module::init()
{
    if (!_loaded || _inited)
        return -1;
    log_WriteLine<Module>(Debug, "Initialize module " + _modPath);
    _funcInit = sharedLibrary_resolveFunction(_ptr, "init");
    _funcFree = sharedLibrary_resolveFunction(_ptr, "free");
    _funcCanHandle = sharedLibrary_resolveFunction(_ptr, "can_handle");
    _funcProcessRequest = sharedLibrary_resolveFunction(_ptr, "process_request");
    _funcGetConfig = sharedLibrary_resolveFunction(_ptr, "get_config");
    auto ret = invokeFunction<module_func_init, int>(_funcInit);
    if (!ret.valid)
    {
        func_result_free<int>(&ret);
        log_WriteLine<Module>(Error, "Initialize module " + _modPath + " failed !");
        return -1;
    }

    int retx = *ret.data;
    func_result_free<int>(&ret);
    if (_funcGetConfig)
    {
        config_t conf;
        auto res = invokeFunction_args<module_func_get_config, int, config_t*>(_funcGetConfig, &conf);
        if (res.valid && *res.data == 0)
        {
            _modName = std::string(conf.name);
            _resultConfig = ResultConfig(conf.result_config);
            _processPriority = ProcessPriority(conf.process_priority);
        }
        func_result_free<int>(&res);
    }
    _inited = true;
    return retx;
}

bool Module::canHandle(const HttpRequest& request)
{
    if (!_inited)
        return -1;
    auto r = HttpRequest_to_C(request);
    auto ret = invokeFunction_args<module_func_can_handle, int, const http_request_t*>(_funcCanHandle, r);
    http_request_t_free(r);
    if (!ret.valid || (*ret.data) == 0)
    {
        func_result_free<int>(&ret);
        return false;
    }
    func_result_free<int>(&ret);
    return true;
}

// Make the module process a request 
void Module::process(const HttpRequest& request, HttpResponse* response)
{
    if (!_inited)
        return;
    auto r =    HttpRequest_to_C(request);
    auto resp = HttpResponse_to_C(response);

    auto result = invokeFunction_args<module_func_process_request, int, http_request_t*, http_response_t*, const char*>(_funcProcessRequest, r, resp, request.path().c_str());
    
    if (!result.valid || (*result.data) < 0)
    {
        http_response_t_free(resp);
        http_request_t_free(r);
        return;
    }
    func_result_free<int>(&result);
    
    response->setHttpVersion(HttpVersion(resp->http_version));
    response->setStatusCode(resp->response_code);
    
    if (resp->headers->size != response->headers().size())
    {
        response->clearHeaders();
        for(int i = 0; i < resp->headers->size; ++i)
        {
            tuple_t* item = resp->headers->data[i];
            if (item->key != nullptr && item->value != nullptr)
                response->addHeader(item->key, item->value);
        }
    }
    http_response_t_free(resp);
    http_request_t_free(r);
}


