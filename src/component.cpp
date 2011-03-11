#include <vector>
#include "component.h"
#include "exception.h"
#include "eventargs.h"
#include "world.h"

void Component::_Init(void)
{
  _attached = false;
  _dependencies=new std::vector<std::string>();

  events.RegisterEvent("OnCreate", new Event());
  events.RegisterEvent("OnDestroy", new Event());
  events.RegisterEvent("OnAttach", new Event());
  events.RegisterEvent("OnDetach", new Event());
  events.CallEvent("OnCreate", NULL, (void*)this);
}

Component::Component(const std::string &name)
{
  _name=name;
  _Init();
}
Component::Component(void)
{
  _Init();
}

Component::~Component(void)
{
  if (_dependencies)
    {
      delete _dependencies;
      _dependencies=NULL;
    }
  events.CallEvent("OnDestroy", NULL, (void*)this);
}

std::string Component::GetName(void) const
{
  return _name;
}
void Component::SetName(const std::string &name)
{
  _name=name;
}

void Component::SetObject(Entity* obj)
{
  _object=obj;
}
Entity* Component::GetObject(void) const
{
  return _object;
}

void Component::Attach(Entity* obj)
{
  std::vector<std::string>::iterator it;
  std::vector<std::string>::iterator itEnd;

  if (_attached)
    {
      return;
    }

  SetObject(obj);

  itEnd = _dependencies->end();
  for (it = _dependencies->begin(); it != itEnd; ++it)
    {
      obj->AddComponent(world->CreateComponent((*it)));
    }

  OneArg arg(obj);
  events.CallEvent("OnAttach", &arg, (void*)this);
  _attached = true;
}
void Component::Detach()
{
  if (!_attached)
    {
      return;
    }

  SetObject(NULL);
  _attached = false;
  events.CallEvent("OnDetach", NULL, (void*) this);
}

BOOL Component::DependencyExists(const std::string &name) const
{
  std::vector<std::string>::const_iterator it;
  std::vector<std::string>::const_iterator itEnd;

  itEnd=_dependencies->end();
  for (it = _dependencies->begin(); it != itEnd; ++it)
    {
      if ((*it) == name)
        {
          return true;
        }
    }

  return false;
}
BOOL Component::AddDependency(const std::string &name)
{
  if (DependencyExists(name))
    {
      return false;
    }

  _dependencies->push_back(name);
  return true;
}
std::vector<std::string>* Component::GetDependencies(void) const
{
  return _dependencies;
}
