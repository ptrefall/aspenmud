#include <string>
#include <list>
#include <map>
#include <tinyxml.h>
#include <boost/bind.hpp>
#include <boost/bind/protect.hpp>
#include "entity.h"
#include "world.h"
#include "room.h"
#include "eventargs.h"
#include "event.h"
#include "eventManager.h"
#include "utils.h"
#include "command.h"
#include "olc.h"
#include "editor.h"
#ifdef MODULE_SCRIPTING
#include "scripts/scripts.h"
#endif

Entity::Entity(void)
{
  _name="A blank object";
  _desc="You see nothing special.";
  _short = "A generic object lies here.";
  _script="";
  _location=NULL;
  _onum=0;
  _components=new std::vector<Component*>();
  _aliases = new std::vector<std::string>();
  variables.SetObject(this);
#ifdef OLC
  _olcs = new std::vector<struct OLC_DATA*>();
  AddOlc("name", "Please enter the name of the object", STRING,
         boost::bind(OlcString, _1, _2, _3,
                     boost::protect(boost::bind(&Entity::GetName, this)),
                     boost::protect(boost::bind(&Entity::SetName, this, _1))));
  AddOlc("description", "Editing object description", EDITOR,
         boost::bind(OlcEditor, _1, _2, _3,
                     boost::protect(boost::bind(&Entity::GetDescription, this)),
                     boost::protect(boost::bind(&Entity::SetDescription, this, _1))));
  AddOlc("script", "Editing object script", EDITOR,
         boost::bind(OlcEditor, _1, _2, _3,
                     boost::protect(boost::bind(&Entity::GetScript, this)),
                     boost::protect(boost::bind(&Entity::SetScript, this, _1))));
#endif
  _persistent = true;

  events.RegisterEvent("PostLook", new Event());
  events.RegisterEvent("PreLook",new Event());
}
Entity::~Entity(void)
{
  std::vector<Component*>::iterator it, itEnd;
  itEnd = _components->end();
  for (it=_components->begin(); it != itEnd; ++it)
    {
      delete (*it);
    }
  delete _components;

  delete _aliases;

#ifdef OLC
  std::vector<struct OLC_DATA*>::iterator oit, oitEnd;
  oitEnd = _olcs->end();
  for (oit=_olcs->begin(); oit != oitEnd; ++oit)
    {
      delete (*oit);
    }
  delete _olcs;
#endif
}

std::string Entity::GetName(void) const
{
  return _name;
}
void Entity::SetName(const std::string &s)
{
  _name=s;
}

std::string Entity::GetShort() const
{
  return _short;
}
void Entity::SetShort(const std::string &s)
{
  _short = s;
}

std::string Entity::GetDescription(void) const
{
  return _desc;
}
void Entity::SetDescription(const std::string &s)
{
  _desc=s;
}

std::string Entity::GetPlural()
{
  return _plural;
}
void Entity::SetPlural(const std::string &s)
{
  _plural = s;
}

std::string Entity::GetScript(void) const
{
  return _script;
}
void Entity::SetScript(const std::string &script)
{
  _script=script;
}

BOOL Entity::GetPersistent() const
{
  return _persistent;
}
void Entity::SetPersistent(BOOL s)
{
  _persistent = s;
}

Entity* Entity::GetLocation(void) const
{
  return _location;
}
void Entity::SetLocation(Entity* location)
{
  _location=location;
}

std::list<Entity*>* Entity::GetContents()
{
  return &_contents;
}

VNUM Entity::GetOnum(void) const
{
  return _onum;
}
void Entity::SetOnum(VNUM num)
{
  _onum=num;
}

void Entity::Serialize(TiXmlElement* root)
{
  TiXmlElement* ent = new TiXmlElement("entity");
  TiXmlElement *components = new TiXmlElement("components");
  TiXmlElement* aliases = new TiXmlElement("aliases");
  TiXmlElement* alias = NULL;
  TiXmlElement* component = NULL;
  TiXmlElement* contents = new TiXmlElement("contents");
  TiXmlElement* properties = new TiXmlElement("properties");
  std::vector<std::string>::iterator ait, aitEnd;
  std::vector <Component*>::iterator it, itEnd;
  itEnd = _components->end();
  std::list<Entity*>::iterator cit, citEnd;
  citEnd=_contents.end();

  if (_contents.size())
    {
      if (_persistent)
        {
          for (cit = _contents.begin(); cit != citEnd; ++cit)
            {
              if (!(*cit)->IsLiving())
                {
                  (*cit)->Serialize(ent);
                }
            }
        }
    }
  ent->LinkEndChild(contents);

  if (_components->size())
    {
      for (it=_components->begin(); it != itEnd; ++it)
        {
          component = new TiXmlElement("component");
          component->SetAttribute("name", (*it)->GetName().c_str());
          (*it)->Serialize(component);
          components->LinkEndChild(component);
        }
    }
  ent->LinkEndChild(components);

  if (_aliases->size())
    {
      aitEnd = _aliases->end();
      for (ait = _aliases->begin(); ait != aitEnd; ++ait)
        {
          alias = new TiXmlElement("alias");
          alias->SetAttribute("name", (*ait).c_str());
          aliases->LinkEndChild(alias);
        }
    }
  ent->LinkEndChild(aliases);

  variables.Serialize(properties);
  ent->LinkEndChild(properties);

  ent->SetAttribute("name", _name.c_str());
  ent->SetAttribute("desc", _desc.c_str());
  ent->SetAttribute("script", _script.c_str());
  ent->SetAttribute("plural", _plural.c_str());
  ent->SetAttribute("onum", _onum);
  ent->SetAttribute("location", (_location?_location->GetOnum():0));

  root->LinkEndChild(ent);
}
void Entity::Deserialize(TiXmlElement* root)
{
  World* world = World::GetPtr();

  TiXmlElement* components = NULL;
  TiXmlElement* component = NULL;
  TiXmlElement* alias = NULL;
  TiXmlElement*aliases = NULL;
  TiXmlElement* obj = NULL;
  TiXmlElement* contents = NULL;
  TiXmlElement* properties = NULL;
  TiXmlNode* node = NULL;
  int loc;
  Component* com = NULL;

//contents
  node = root->FirstChild("contents");
  if (node)
    {
      contents = node->ToElement();
      for (node = contents->FirstChild(); node; node = node->NextSibling())
        {
          obj = node->ToElement();
          Entity* object = new Entity();
          object->Deserialize(obj);
          object->SetLocation(this);
          _contents.push_back(object);
        }
    }

  node = root->FirstChild("components");
  if (node)
    {
      components = node->ToElement();
      for (node = components->FirstChild(); node; node = node->NextSibling())
        {
          component = node->ToElement();
          com = world->CreateComponent(component->Attribute("name"));
          com->Deserialize(component);
          AddComponent(com);
        }
    }

  node = root->FirstChild("aliases");
  if (node)
    {
      aliases = node->ToElement();
      for (node = aliases->FirstChild(); node; node = node->NextSibling())
        {
          alias=node->ToElement();
          AddAlias(alias->Attribute("name"));
        }
    }

  node = root->FirstChild("properties");
  if (node)
    {
      properties = node->ToElement();
      variables.Deserialize(properties);
    }

  _name = root->Attribute("name");
  _desc = root->Attribute("desc");
  _script = root->Attribute("script");
  _plural = root->Attribute("plural");
  root->Attribute("onum", (unsigned int*)&_onum);
  root->Attribute("location", &loc);
//we need to add ourselves to the object registry.
  world->AddObject(_onum, this);
  if (!loc)
    {
      _location=NULL;
    }
  else
    {
      _location=world->GetObject(loc);
    }

//now we execute the script on the object.
#ifdef MODULE_SCRIPTING
  Script* script = (Script*)world->GetProperty("script");
  script->Execute(this, GetScript());
#endif

//and now we notify everything that an object was loaded:
  world->events.CallEvent("ObjectLoaded", NULL, this);
}

std::string Entity::DoLook(Player* mobile)
{
  std::string str;

  LookArgs* args=new LookArgs(mobile,this,str);
  events.CallEvent("PreLook", args, (void*)mobile);
  str+=Capitalize(this->GetName())+"\n";
  str+=this->GetDescription()+"\n";
  events.CallEvent("PostLook", args, (void*)mobile);
  delete args;
  return str;
}

BOOL Entity::CanReceive(Entity* obj) const
{
  return true;
}

BOOL Entity::MoveTo(Entity* targ)
{
  if (targ->CanReceive(this))
    {
      if (_location)
        {
          _location->ObjectLeave(this);
        }
      _location=targ;
      targ->ObjectEnter(this);
      return true;
    }
  return false;
}
void Entity::ObjectEnter(Entity* obj)
{
  _contents.push_back(obj);
}
void Entity::ObjectLeave(Entity* obj)
{
  std::list<Entity*>::iterator it, itEnd;

  itEnd = _contents.end();
  for (it = _contents.begin(); it != itEnd; ++it)
    {
      if ((*it) == obj)
        {
          it = _contents.erase(it);
        }
    }
}

BOOL Entity::AddComponent(Component* component)
{
  if (component==NULL)
    {
      return false;
    }

  if (HasComponent(component->GetName()))
    {
      return false;
    }
  _components->push_back(component);
  component->Attach(this);
  return true;
}
BOOL Entity::RemoveComponent(Component* component)
{
  std::vector<Component*>::iterator it, itEnd;

  if (!HasComponent(component->GetName()))
    {
      return false;
    }

  itEnd = _components->end();
  for (it = _components->begin(); it != itEnd; ++it)
    {
      if ((*it) == component)
        {
          component->Detach();
          _components->erase(it);
          return true;
        }
    }

  return false; //should never happen.
}
bool Entity::HasComponent(const std::string &name)
{
  std::vector <Component*>::iterator it, itEnd;

  if (_components->size())
    {
      itEnd = _components->end();
      for (it = _components->begin(); it != itEnd; ++it)
        {
          if ((*it)->GetName()==name)
            {
              return true;
            }
        }
    }

  return false;
}
Component* Entity::GetComponent(const std::string &name)
{
  std::vector <Component*>::iterator it, itEnd;

  if (_components->size())
    {
      itEnd = _components->end();
      for (it = _components->begin(); it != itEnd; ++it)
        {
          if ((*it)->GetName()==name)
            {
              return (*it);
            }
        }
    }

  return NULL;
}
void Entity::Attach(Entity* obj)
{
  std::vector <Component*>::iterator it, itEnd;
  if (_components->size())
    {
      itEnd = _components->end();
      for (it = _components->begin(); it != itEnd; ++it)
        {
          (*it)->Attach(obj);
        }
    }
}

BOOL Entity::AddAlias(const std::string &alias)
{
  if (AliasExists(alias) && alias != "")
    {
      return false;
    }

  _aliases->push_back(alias);
  return true;
}
BOOL Entity::AliasExists(const std::string & name)
{
  std::vector<std::string>::iterator it, itEnd;

  if (_aliases->size())
    {
      itEnd = _aliases->end();
      for (it = _aliases->begin(); it != itEnd; ++it)
        {
          if ((*it) == name)
            {
              return true;
            }
        }
    }

  return false;
}
std::vector<std::string>* Entity::GetAliases()
{
  return _aliases;
}

BOOL Entity::IsNpc() const
{
  return false;
}
BOOL Entity::IsPlayer() const
{
  return false;
}
BOOL Entity::IsLiving() const
{
  if (IsPlayer() || IsNpc())
    {
      return true;
    }

  return false;
}
BOOL Entity::IsRoom() const
{
  return false;
}

#ifdef OLC
void Entity::AddOlc(const std::string &name, const std::string &prompt, OLC_INPUT_TYPE type, OLCFUNC callback)
{
  struct OLC_DATA* data = NULL;
  if ((name == "") || (callback == NULL))
    {
      return;
    }

  data = new OLC_DATA();
  data->name = name;
  data->prompt = prompt;
  data-> type = type;
  data->callback = callback;
  _olcs->push_back(data);
}
BOOL Entity::RemoveOlc(const std::string &name)
{
  std::vector<struct OLC_DATA*>::iterator it, itEnd;

  itEnd = _olcs->end();
  for (it = _olcs->begin(); it != itEnd; ++it)
    {
      if ((*it)->name == name)
        {
          _olcs->erase(it);
          return true;
        }
    }

  return false;
}
void Entity::ListOlcs(std::vector<struct OLC_DATA*> &out)
{
  std::vector<struct OLC_DATA*>::iterator it;
  std::vector<struct OLC_DATA*>::iterator end = _olcs->end();

  for (it = _olcs->begin(); it != end; it++)
    {
      out.push_back((*it));
    }
}
OLC_DATA* Entity::GetOlcByIndex(int index)
{
  index--;

  if (((int)_olcs->size() < index+1) || (index < 0 ))
    {
      return NULL;
    }

  return (_olcs->at(index));
}
#endif
