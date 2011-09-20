#include "../mud.h"
#include "../conf.h"
#include "../world.h"
#include "../player.h"
#include "../utils.h"
#include "../inputHandlers.h"
#include "help.h"
#include "HelpEntry.h"
#include "HelpTable.h"
#include <string>
#include <boost/bind.hpp>

#ifdef MODULE_HELP
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
  Menu* m = new Menu(mobile);
  m->AddItem("Add topic", boost::bind(&CMDHedit::MainMenu, this, _1, _2, _3, _4), (void*)mobile, 1);
  m->AddItem("Remove topic", boost::bind(&CMDHedit::MainMenu, this, _1, _2, _3, _4), (void*)mobile, 2);
  m->AddItem("Edit topic", boost::bind(&CMDHedit::MainMenu, this, _1, _2, _3, _4), (void*)mobile, 3);
  if (!m->Attach())
    {
      mobile->Message(MSG_ERROR, "Error!");
    }
  return true;
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
