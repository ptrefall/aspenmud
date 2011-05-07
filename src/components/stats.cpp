#include <boost/bind.hpp>
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
  events.AddCallback("OnAttach", boost::bind(&Stats::attached, this, _1, _2));
}

CCOMCREATE(Stats, Create)
{
  return new Stats();
}
CCOMINIT(Stats, Initialize)
{
  World* world = World::GetPtr();
  world->events.GetEvent("PlayerCreated")->Add(boost::bind(&Stats::Added, _1, _2));
  world->events.GetEvent("PlayerConnect")->Add(boost::bind(&Stats::Added, _1, _2));
}

CEVENT(Stats, attached)
{
  variables.AddProperty("str", Variant(1));
  variables.AddProperty("con", Variant(1));
  variables.AddProperty("dex", Variant(1));
  variables.AddProperty("int", Variant(1));
  variables.AddProperty("wis", Variant(1));
}

CEVENT(Stats, Added) //called for new player and player logins.
{
  World* world = World::GetPtr();

  Player* mob=(Player*)caller;
  mob->AddComponent(world->CreateComponent("stats"));
}
