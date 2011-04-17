#include <map>
#include "ComponentFactory.h"
#include "component.h"

ComponentFactory::ComponentFactory(void)
{
  _components=new std::map<std::string,COMCREATECB>();
}
ComponentFactory::~ComponentFactory(void)
{
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

  (*_components)[name]=cb;
  return true;
}

Component* ComponentFactory::Create(const std::string &name)
{
  if (HasComponent(name))
    {
      return ((*_components)[name])();
    }

  return NULL;
}
