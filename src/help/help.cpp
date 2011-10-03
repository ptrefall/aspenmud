#include "../mud.h"
#include "../conf.h"
#include "../world.h"
#include "../player.h"
#include "../utils.h"
#include "../inputHandlers.h"
#include "../olc.h"
#include "../editor.h"
#include "../event.h"
#include "../eventargs.h"
#include "help.h"
#include "HelpEntry.h"
#include "HelpTable.h"
#include <string>
#include <boost/bind.hpp>

#ifdef MODULE_HELP

/*
*This structure holds the name and the player to be passed to the edit menu function.
*/
struct HelpEditMenuData
{
  std::string name;
  Player* mobile;
};
//
struct SeeAlsoMenuData
{
  HelpEntry* entry;
  Player* mobile;
};

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

CMDHedit::CMDHedit()
{
  SetName("hedit");
  AddAlias("hed");
  SetAccess(RANK_ADMIN);
}
BOOL CMDHedit::Execute(const std::string &verb, Player* mobile,std::vector<std::string> &args,int subcmd)
{
//no args were provided, show the default editor:
  if (!args.size())
    {
      Menu* m = new Menu(mobile);
      m->AddItem("Add topic", boost::bind(&CMDHedit::MainMenu, this, _1, _2, _3, _4), (void*)mobile, 1);
      m->AddItem("Remove topic", boost::bind(&CMDHedit::MainMenu, this, _1, _2, _3, _4), (void*)mobile, 2);
      m->AddItem("Edit topic", boost::bind(&CMDHedit::MainMenu, this, _1, _2, _3, _4), (void*)mobile, 3);
      m->Attach();
      return true;
    }
  else
    {
      if (args.size() == 1)
        {
          EditTopic(mobile, args[0]);
          return true;
        }
      else
        {
          std::string arg = Explode(args);
          EditTopic(mobile, arg);
          return true;
        }
    }
}

CMENU(CMDHedit, MainMenu)
{
  switch(subitem)
    {
    case 1: //AddTopic
      mob->Message(MSG_INFO, "Please enter the name of the topic you want to create, or @abort to abort.");
      LineHandler::CreateHandler(mob->GetSocket(), boost::bind(&CMDHedit::CreateTopic, this, _1, _2));
      return;
    case 2: //Remove topic
      mob->Message(MSG_INFO, "Please enter the name of the topic you want to remove, or @abort to abort.");
      LineHandler::CreateHandler(mob->GetSocket(), boost::bind(&CMDHedit::RemoveTopic, this, _1, _2));
      return;
    case 3:
      mob->Message(MSG_INFO, "Please enter the name of the topic you want to edit, or @abort to abort.");
      LineHandler::CreateHandler(mob->GetSocket(), boost::bind(&CMDHedit::TopicName, this, _1, _2));
      return;
    }
}

void CMDHedit::CreateTopic(Socket* sock, const std::string &name)
{
  Player* mobile = sock->GetPlayer();
  World* world = World::GetPtr();
  HelpTable* table = (HelpTable*)world->GetProperty("help");
  HelpEntry* entry = NULL;

  if (name == "@abort")
    {
      mobile->Message(MSG_INFO, "Aborting.");
      sock->ClearInput();
      return;
    }

  entry = new HelpEntry(name, "This help file does not yet have any content.", RANK_PLAYER, h_general);
  if (!entry)
    {
      mobile->Message(MSG_ERROR, "Could not create help entry: memory error.");
      sock->ClearInput();
      return;
    }

  if (!table->AddEntry(entry))
    {
      mobile->Message(MSG_ERROR, "Could not add the specified entry to the help table.\nCheck to see if the entry already exists.");
      sock->ClearInput();
      delete entry;
      return;
    }

  mobile->Message(MSG_INFO, "Entry successfully added.");
  sock->ClearInput();
}
void CMDHedit::RemoveTopic(Socket* sock, const std::string &name)
{
  Player* mobile = sock->GetPlayer();
  World* world = World::GetPtr();
  HelpTable* table = (HelpTable*)world->GetProperty("help");

  if (name == "@abort")
    {
      mobile->Message(MSG_INFO, "Aborting.");
      sock->ClearInput();
      return;
    }

  if (!table->RemoveEntry(name))
    {
      mobile->Message(MSG_ERROR, "The specified entry does not exist.");
    }
  else
    {
      mobile->Message(MSG_INFO, "The specified entry was successfully removed.");
    }

  sock->ClearInput();
}
void CMDHedit::TopicName(Socket* sock, const std::string &name)
{
  Player* mobile = sock->GetPlayer();

  if (name == "@abort")
    {
      mobile->Message(MSG_INFO, "Aborting.");
      sock->ClearInput();
      return;
    }

  EditTopic(mobile, name);
}
void CMDHedit::EditTopic(Player* mobile, const std::string &name)
{
  World* world = World::GetPtr();
  HelpTable* table = (HelpTable*)world->GetProperty("help");
  Menu* m = NULL;
  HelpEditMenuData* data = NULL;

  if (!table->EntryExists(name))
    {
      mobile->Message(MSG_ERROR, "That topic was not found.");
      mobile->GetSocket()->ClearInput();
      return;
    }

  data = new HelpEditMenuData();
  data->name = name;
  data->mobile = mobile;
  m = new Menu(mobile, data);
  m->AtUnattach(boost::bind(&CMDHedit::EditMenuCleanup, this, _1));
  m->AddItem("Edit access", boost::bind(&CMDHedit::EditMenu, this, _1, _2, _3, _4), (void*)data, 1);
  m->AddItem("Edit contents", boost::bind(&CMDHedit::EditMenu, this, _1, _2, _3, _4), (void*)data, 2);
  m->AddItem("Add see also", boost::bind(&CMDHedit::EditMenu, this, _1, _2, _3, _4), (void*)data, 3);
  m->AddItem("Remove see also", boost::bind(&CMDHedit::EditMenu, this, _1, _2, _3, _4), (void*)data, 4);
  m->Attach();
}

void CMDHedit::EditMenuCleanup(Menu* m)
{
  HelpEditMenuData* data = (HelpEditMenuData*)m->GetArg();
  delete data;
  m->SetArg(NULL);
}
void CMDHedit::SeeAlsoMenuCleanup(Menu* m)
{
  SeeAlsoMenuData* data = (SeeAlsoMenuData*)m->GetArg();
  delete data;
  m->SetArg(NULL);
}

CMENU(CMDHedit, EditMenu)
{
  int i = 0;
  std::vector<std::string> names;
  std::vector<std::string>::iterator it, itEnd;
  HelpEditMenuData *data = NULL;
  SeeAlsoMenuData* sdata = NULL;
  World* world = World::GetPtr();
  HelpTable* table = (HelpTable*)world->GetProperty("help");
  HelpEntry* entry = NULL;

  switch(subitem)
    {
    case 1: //edit access
    {
      OlcPlayerFlag(NULL, mob, NULL, boost::bind(&HelpEntry::GetAccess, entry), boost::bind(&HelpEntry::UpdateAccess, entry, _1));
      break;
    }
    case 2: //edit contents:
    {
      data = (HelpEditMenuData*)menu->GetArg();
      entry = table->GetEntry(data->name);
      if (!entry)
        {
          mob->Message(MSG_ERROR, "Could not edit that topic.");
          break;
        }

      Editor* ed = new Editor();
      ed->SetArg((void*)entry);
      ed->events.AddCallback("load", boost::bind(&CMDHedit::EditorLoad, this, _1, _2));
      ed->events.AddCallback("save", boost::bind(&CMDHedit::EditorSave, this, _1, _2));
      ed->EnterEditor(mob);
      break;
    }
    case 3: //add seealso
    {
      mob->Message(MSG_INFO, "Please enter the name of the topic you want to add a see also to, or @abort to abort.");
      LineHandler::CreateHandler(mob->GetSocket(), boost::bind(&CMDHedit::AddSeealso, this, _1, _2, menu->GetArg()));
      break;
    }
    case 4: //Remove see also
    {
      Menu* m = new Menu(mob);
      data = (HelpEditMenuData*)menu->GetArg();
      entry = table->GetEntry(data->name);
      if (!entry)
        {
          mob->Message(MSG_ERROR, "There was an error; that help entry does not exist.");
          menu->ShowMenu();
          return;
        }

      if (!entry->SeeAlsoToList(&names))
        {
          mob->Message(MSG_ERROR, "Could not retrieve see also entries for the specified help entry.");
          menu->ShowMenu();
          return;
        }

      sdata = new SeeAlsoMenuData();
      sdata->entry = entry;
      sdata->mobile = mob;
      m->SetArg((void*)sdata);
      m->AtUnattach(boost::bind(&CMDHedit::SeeAlsoMenuCleanup, this, _1));

      itEnd = names.end();
      for (it = names.begin(), i = 0; it != itEnd; ++it, ++i)
        {
          m->AddItem((*it), boost::bind(&CMDHedit::RemoveSeeAlso, this, _1, _2, _3, _4), (void*)mob, i);
        }
      m->Attach();
    }
    }
}

CMENU(CMDHedit, RemoveSeeAlso)
{
  Socket* sock = mob->GetSocket();
  SeeAlsoMenuData* data = (SeeAlsoMenuData*)menu->GetArg();
  HelpEntry* sa = NULL;

  sa = data->entry->GetSeeAlsoByIndex((unsigned int)subitem);
  if (sa == NULL)
    {
      mob->Message(MSG_ERROR, "That entry does not exist.");
      sock->ClearInput();
      return;
    }

  if (!data->entry->RemoveSeeAlso(sa->GetName()))
    {
      mob->Message(MSG_ERROR, "Erorr, could not remove that see also entry.");
    }
  else
    {
      mob->Message(MSG_INFO, "See also entry removed successfully.");
    }

  sock->ClearInput();
}

CEVENT(CMDHedit, EditorLoad)
{
  EditorLoadedArgs *arg = (EditorLoadedArgs*)args;
  Editor* ed = arg->editor;
  HelpEntry* entry = (HelpEntry*)ed->GetArg();
  std::vector<std::string> lines;
  std::vector<std::string>::iterator it, itEnd;

  SplitToVector(entry->GetData(), &lines);

  itEnd = lines.end();
  for (it = lines.begin(); it != itEnd; ++it)
    {
      ed->Add((*it), true);
    }
}
CEVENT(CMDHedit, EditorSave)
{
  EditorLoadedArgs *arg = (EditorLoadedArgs*)args;
  Editor* ed = arg->editor;
  HelpEntry* entry = (HelpEntry*)ed->GetArg();
  std::vector<std::string> *lines = ed->GetLines();

  entry->SetData(Explode(*lines, "\n"));
}

void CMDHedit::AddSeealso(Socket* sock, std::string input, void* data)
{
  Player* mobile = sock->GetPlayer();
  World* world = World::GetPtr();
  HelpTable *table = (HelpTable*)world->GetProperty("help");
  HelpEditMenuData* mdata = (HelpEditMenuData*)data;
  HelpEntry* entry = table->GetEntry(mdata->name);
  HelpEntry* sa = table->GetEntry(input);

  if (!sa)
    {
      mobile->Message(MSG_ERROR, "Could not add the specified see also entry.");
    }
  else
    {
      if (!entry->AddSeeAlso(sa))
        {
          mobile->Message(MSG_ERROR, "Could not add the specified see also entry.");
        }
      else
        {
          mobile->Message(MSG_INFO, "See also entry added successfully.");
        }
    }

  sock->ClearInput();
}
#endif

BOOL InitializeHelp()
{
#ifdef MODULE_HELP
  World* world = World::GetPtr();

  world->WriteLog("Initializing help.");
  HelpTable* table = new HelpTable();
  if (!table)
    {
      world->WriteLog("Error creating help table.", CRIT);
      return false;
    }
  world->AddProperty("help", (void*)table);
  world->commands.AddCommand(new CMDHelp());
  world->commands.AddCommand(new CMDHedit());
#endif

  return true;
}
