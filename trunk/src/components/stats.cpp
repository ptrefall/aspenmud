#include "stats.h"
#include "../event.h"
#include "../player.h"
#include "../entity.h"
#include "../world.h"
#include "../eventargs.h"

Stats::Stats(void)
{
  SetName("stats");
  events.RegisterEvent("StatChanged", new Event());
  events.AddCallback("OnAttach", STATS_ATTACHED);
}

COMCREATE(STATS_CREATE)
{
  return new Stats();
}
COMINIT(STATS_INIT)
{
  World* world = World::GetPtr();
  world->events.GetEvent("PlayerCreated")->Add(STATS_ADD);
  world->events.GetEvent("PlayerConnect")->Add(STATS_ADD);
}

EVENT(STATS_ATTACHED)
{
  OneArg* arg = (OneArg*)args;
  Entity* obj = (Entity*) arg->_arg;

  obj->variables.AddProperty("str", Variant(1));
  obj->variables.AddProperty("con", Variant(1));
  obj->variables.AddProperty("dex", Variant(1));
  obj->variables.AddProperty("int", Variant(1));
  obj->variables.AddProperty("wis", Variant(1));
}

EVENT(STATS_ADD) //called for new player and player logins.
{
  World* world = World::GetPtr();

  Player* mob=(Player*)caller;
  mob->AddComponent(world->CreateComponent("stats"));
}
