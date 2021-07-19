#ifndef MODULES_MANAGER_H
#define MODULES_MANAGER_H

#include <vector>
class Module;
class HttpRequest;
class ModulesManager
{
public:
    ModulesManager();
    ~ModulesManager();
    void addModule(Module* module);
    void loadAll();
    const std::vector<Module*>& modules() const { return _mods; }

    int modCount() const { return _mods.size(); }
private:
    std::vector<Module*> _mods;
};

#endif // !MODULES_MANAGER_H