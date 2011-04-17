#include <list>
#include <cstring>
#include "com_wiz.h"
#include "command.h"
#include "player.h"
#include "world.h"
#include "utils.h"

void InitializeWizCommands()
{
  World* world = World::GetPtr();

  world->WriteLog("Initializing wizard commands.");
  world->commands.AddCommand(new CMDCopyover());
  world->commands.AddCommand(new CMDMkgod());
  world->commands.AddCommand(new CMDShutdown());
  world->commands.AddCommand(new CMDMkbuilder());
}

CMDCopyover::CMDCopyover()
{
  SetName("copyover");
  SetAccess(RANK_GOD);
}
BOOL CMDCopyover::Execute(const std::string &verb, Player* mobile,std::vector<std::string> &args,int subcmd)
{
  World* world = World::GetPtr();

  world->Copyover(mobile);
  return true;
}

CMDMkgod::CMDMkgod()
{
  SetName("mkgod");
  SetAccess(RANK_GOD);
}
BOOL CMDMkgod::Execute(const std::string &verb, Player* mobile,std::vector<std::string> &args,int subcmd)
{
  World* world = World::GetPtr();
  Player* target = NULL;

  if (!args.size())
    {
      mobile->Message(MSG_ERROR,"You must specify the person that you would like to make a god.\n");
      return false;
    }

  target=world->FindPlayer(args[0]);

  if (target==mobile)
    {
      mobile->Message(MSG_ERROR,"You may not make yourself a god.");
      return false;
    }
  if (target==NULL)
    {
      mobile->Message(MSG_ERROR,"That person couldn't be found.");
      return false;
    }
  if (BitIsSet(target->GetRank(),RANK_GOD))
    {
      mobile->Message(MSG_ERROR,"That person is already a god.");
      return false;
    }

  target->SetRank(BitSet(mobile->GetRank(),RANK_GOD));
  target->Message(MSG_INFO,"You suddenly feel more godlike.");
  return true;
}

CMDMkbuilder::CMDMkbuilder()
{
  SetName("mkbuilder");
  SetAccess(RANK_GOD);
}
BOOL CMDMkbuilder::Execute(const std::string &verb, Player* mobile,std::vector<std::string> &args,int subcmd)
{
  World* world = World::GetPtr();
  Player* target = NULL;

  if (!args.size())
    {
      mobile->Message(MSG_ERROR,"You must specify the person that you would like to make a builder.\n");
      return false;
    }

  target=world->FindPlayer(args[0]);
  if (target==NULL)
    {
      mobile->Message(MSG_ERROR,"That person couldn't be found.");
      return false;
    }
  if (BitIsSet(target->GetRank(),RANK_BUILDER))
    {
      mobile->Message(MSG_ERROR,"That person is already a builder.");
      return false;
    }

  target->SetRank(BitSet(mobile->GetRank(),RANK_BUILDER));
  target->Message(MSG_INFO,"You were made a builder.");

  return true;
}
CMDShutdown::CMDShutdown()
{
  SetName("shutdown");
  SetAccess(RANK_ADMIN);
}
BOOL CMDShutdown::Execute(const std::string &verb, Player* mobile,std::vector<std::string> &args,int subcmd)
{
  World* world = World::GetPtr();
  world->Shutdown();
  return true;
}
