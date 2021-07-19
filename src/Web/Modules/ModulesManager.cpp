#include <Web/Modules/ModulesManager.h>
#include <Web/Modules/Module.h>

ModulesManager::ModulesManager()
{

}
ModulesManager::~ModulesManager()
{
    for(Module* mod : _mods)
    {
        delete mod;
    }
}
void ModulesManager::addModule(Module* mod)
{
    if (mod != nullptr)
        _mods.push_back(mod);
}
void ModulesManager::loadAll()
{
    for(Module* mod : _mods)
    {
        mod->load();
        mod->init();
    }
}