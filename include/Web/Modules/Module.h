#ifndef MODULE_H
#define MODULE_H

#include <string>

class HttpRequest;
class HttpResponse;
class Module
{
public:
    enum ResultConfig 
    {
        Return, Forward, IgnoreIfEmpty
    };
    enum ProcessPriority
    {
        First, Last
    };
    Module(const std::string& path);
    ~Module();
    bool load();
    int init();
    bool canHandle(const HttpRequest& request);
    void process(const HttpRequest& request, HttpResponse* response);

    const std::string& path() const { return _modPath; }
    const std::string& name() const { return _modName; }
    ResultConfig resultConfig() const { return _resultConfig; }
    ProcessPriority processPriority() const { return _processPriority; }

private:
    std::string _modPath;
    std::string _modName;
    ResultConfig _resultConfig;
    ProcessPriority _processPriority;
    bool _loaded;
    bool _inited;
    void* _ptr;
    void* _funcInit;
    void* _funcFree;
    void* _funcCanHandle;
    void* _funcProcessRequest;
    void* _funcGetConfig;
};

#endif // !MODULE_H
