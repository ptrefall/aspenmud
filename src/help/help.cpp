#include "help.h"
#include "../mud.h"
#include "../conf.h"
#include "../world.h"
#include "../player.h"
#include "../utils.h"
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <tinyxml.h>

HelpEntry::HelpEntry(const std::string &name, const std::string &data, FLAG access, HELP_ENTRY_TYPE type):_name(name), _data(data), _access(access), _type(type)
{
  _lastModified = time(NULL);
}
HelpEntry::HelpEntry()
{
  _lastModified = time(NULL);
}
HelpEntry::~HelpEntry()
{
}

std::string HelpEntry::GetName() const
{
  return _name;
}
void HelpEntry::SetName(const std::string &name)
{
  _name = name;
}

std::string HelpEntry::GetData() const
{
  return _data;
}
void HelpEntry::SetData(const std::string &data)
{
  _data = data;
}

HELP_ENTRY_TYPE HelpEntry::GetType() const
{
  return _type;
}
void HelpEntry::SetType(HELP_ENTRY_TYPE type)
{
  _type = type;
}

FLAG HelpEntry::GetAccess() const
{
  return _access;
}
void HelpEntry::SetAccess(FLAG access)
{
  _access = access;
}

void HelpEntry::Serialize(TiXmlElement* root)
{
  TiXmlElement* entry = new TiXmlElement("entry");
  entry->SetAttribute("name", _name.c_str());
  entry->SetAttribute("data", _data.c_str());
  entry->SetAttribute("type", _type);
  entry->SetAttribute("access", _access);
  entry->SetAttribute("lastModified", _lastModified);
  root->LinkEndChild(entry);
}
void HelpEntry::Deserialize(TiXmlElement* entry)
{
  int tmp = 0;

  _name = entry->Attribute("name");
  _data = entry->Attribute("data");
  entry->Attribute("access", &_access);
  tmp = (int)_type;
  entry->Attribute("type", &tmp);
  tmp = (int)_lastModified;
  entry->Attribute("lastModified", &tmp);
}

HelpTable::HelpTable()
{
  _entries = new std::vector<HelpEntry*>();
}
HelpTable::~HelpTable()
{
  std::vector<HelpEntry*>::iterator it, itEnd;

  itEnd = _entries->end();
  for (it = _entries->begin(); it != itEnd; ++it)
    {
      delete (*it);
    }

  delete _entries;
}

void HelpTable::Load()
{
  TiXmlDocument doc(HELP_FILE);
  TiXmlElement* root = NULL;
  TiXmlElement* element = NULL;
  TiXmlNode* node = NULL;
  HelpEntry* entry = NULL;
  if (!doc.LoadFile())
    {
      world->WriteLog("Could not find help files.", ERR);
      return;
    }

  node = doc.FirstChild("help");
  if (!node)
    {
      world->WriteLog("The help files were loaded, but the top element could not be located. Help will not load.", ERR);
      return;
    }
  root = node->ToElement();

  node=root->FirstChild("entry");
  if (!node)
    {
      return;
    }
  element = node->ToElement();
  for (node = element->FirstChild(); node; node = node->NextSibling())
    {
      entry = new HelpEntry();
      entry->Deserialize(element);
      _entries->push_back(entry);
    }
}

void HelpTable::Save()
{
  TiXmlDocument doc;
  TiXmlDeclaration *decl = new TiXmlDeclaration("1.0", "", "");
  doc.LinkEndChild(decl);
  TiXmlElement* root = new TiXmlElement("help");
  std::vector<HelpEntry*>::iterator it, itEnd;

  itEnd = _entries->end();
  for (it = _entries->begin(); it != itEnd; ++it)
    {
      (*it)->Serialize(root);
    }

  doc.SaveFile(HELP_FILE);
}
