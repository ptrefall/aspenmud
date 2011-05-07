/*
*This is a bit of a messy way to set this up, but it works.
*Include the header file for every component here, then register it.
*/
#include <boost/bind.hpp>
#include "world.h"
#include "component.h"
//start components include
#include "components/stats.h"

void CreateComponents(void)
{
  World* world = World::GetPtr();

  world->WriteLog("Initializing components.");
  world->RegisterComponent("stats",
                           boost::bind(&Stats::Create),
                           boost::bind(&Stats::Initialize));
}
