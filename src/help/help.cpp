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

#ifdef MODULE_HELP
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
  World* world = World::GetPtr();
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

BOOL HelpTable::AddEntry(HelpEntry* entry)
{
  if (!entry)
    {
      return false;
    }

  std::vector<HelpEntry*>::iterator it, itEnd;

  itEnd = _entries->end();
  for (it = _entries->begin(); it != itEnd; ++it)
    {
      if ((*it)->GetName() == entry->GetName())
        {
          return false;
        }
    }

  _entries->push_back(entry);
  return true;
}
BOOL HelpTable::RemoveEntry(const std::string &name)
{
  std::vector<HelpEntry*>::iterator it, itEnd;

  itEnd = _entries->end();
  for (it = _entries->begin(); it != itEnd; ++it)
    {
      if ((*it)->GetName() == name)
        {
          _entries->erase(it);
          delete (*it);
          return true;
        }
    }

  return false;
}
BOOL HelpTable::EntryExists(const std::string &name)
{
  std::vector<HelpEntry*>::iterator it, itEnd;

  itEnd = _entries->end();
  for (it = _entries->begin(); it != itEnd; ++it)
    {
      if ((*it)->GetName() == name)
        {
          return true;
        }
    }

  return false;
}

BOOL HelpTable::ShowEntry(const std::string &name, Player* mobile)
{
  std::vector<HelpEntry*>::iterator it, itEnd;

  itEnd = _entries->end();
  for (it = _entries->begin(); it != itEnd; ++it)
    {
      if ((*it)->GetName() == name)
        {
          mobile->Message(MSG_INFO, (*it)->GetData());
          return true;
        }
    }

  return false;
}

CMDHelp::CMDHelp()
{
  SetName("help");
  AddAlias("h");
  SetType(normal);
}
BOOL CMDHelp::Execute(const std::string &verb, Player* mobile,std::vector<std::string> &args,int subcmd)
{
  World* world = World::GetPtr();
  HelpTable* table = NULL;

  if (!args.size())
    {
      mobile->Message(MSG_ERROR, "Syntax: help <topic>.\nYou can also use help <index> to get a listing of help categories.");
      return false;
    }

  table = (HelpTable*)world->GetProperty("help");
  if (args.size() == 1)
    {
      if (!table->ShowEntry(args[0], mobile))
        {
          mobile->Message(MSG_ERROR, "That topic does not exist.");
          return false;
        }
    }
  else
    {
      std::string arg;
      arg = Explode(args);
      if (!table->ShowEntry(arg, mobile))
        {
          mobile->Message(MSG_ERROR, "That topic does not exist.");
          return false;
        }
    }

  return true;
}

CMDAddTopic::CMDAddTopic()
{
  SetName("addtopic");
  AddAlias("addt");
  SetAccess(RANK_ADMIN);
}
BOOL CMDAddTopic::Execute(const std::string &verb, Player* mobile,std::vector<std::string> &args,int subcmd)
{
  World* world = World::GetPtr();

  HelpEntry* entry;
  std::string arg = Explode(args);
  HelpTable* table = (HelpTable*)world->GetProperty("table");
  if (table->EntryExists(arg))
    {
      mobile->Message(MSG_ERROR, "That topic already exists.");
      return false;
    }

  entry = new HelpEntry();
  entry->SetName(arg);
  entry->SetData("A newly created help file, with no contents. Perhaps you should tell "+mobile->GetName()+" to give this help file some content.");
  if (!table->AddEntry(entry))
    {
      mobile->Message(MSG_ERROR, "Could not add the entry to the help table.");
      delete entry;
      return false;
    }

  table->AddEntry(entry);
  mobile->Message(MSG_INFO, "Entry added successfully. You will need to write contents in the entry, by using the hedit command.");
  return true;
}

CMDHedit::CMDHedit()
{
  SetName("hedit");
  AddAlias("hed");
  SetAccess(RANK_ADMIN);
}
BOOL CMDHedit::Execute(const std::string &verb, Player* mobile,std::vector<std::string> &args,int subcmd)
{
  return true;
}
#endif

void InitializeHelp()
{
#ifdef MODULE_HELP
  World* world = World::GetPtr();

  world->WriteLog("Initializing help.");
  HelpTable* table = new HelpTable();
  if (!table)
    {
      world->WriteLog("Error creating help table.", CRIT);
    }
  world->AddProperty("help", (void*)table);
  world->commands.AddCommand(new CMDHelp());
  world->commands.AddCommand(new CMDAddTopic());
#endif
}
