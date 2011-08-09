#include "../mud.h"
#include "../conf.h"
#include "../world.h"
#include "../player.h"
#include "../utils.h"
#include "help.h"
#include "HelpEntry.h"
#include "HelpTable.h"
#include <string>

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
  world->commands.AddCommand(new CMDAddTopic());
#endif

  return true;
}
