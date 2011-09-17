#include <map>
#include "ComponentFactory.h"
#include "component.h"

ComponentFactory::ComponentFactory(void)
{
  _components = new std::map <std::string, componentEntry*>();
  _id = 1;
}
ComponentFactory::~ComponentFactory(void)
{
  std::map <std::string, componentEntry*>::iterator it, itEnd;

  itEnd = _components->end();
  for (it = _components->begin(); it != itEnd; ++it)
    {
      delete (*it).second;
    }

  delete _components;
}
BOOL ComponentFactory::HasComponent(const std::string &name)
{
  return (_components->count(name)==0?0:1);
}

BOOL ComponentFactory::RegisterComponent(const std::string &name,COMCREATECB cb)
{
  if (HasComponent(name))
    {
      return false;
    }
  componentEntry* entry = new componentEntry();
  entry->cb = cb;
  entry->id = _id;
  _id++;
  (*_components)[name]=entry;
  return true;
}

Component* ComponentFactory::Create(const std::string &name)
{
  if (HasComponent(name))
    {
      return ((*_components)[name])->cb();
    }

  return NULL;
}

unsigned int ComponentFactory::GetId(const std::string &name)
{
  if (!HasComponent(name))
    {
      return 0;
    }

  return ((*_components)[name])->id;
}
