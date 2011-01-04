/*
*stats.cpp
*
*   Copyright 2010 Tyler Littlefield.
*
*   Licensed under the Apache License, Version 2.0 (the "License");
*   you may not use this file except in compliance with the License.
*   You may obtain a copy of the License at
*
*       http://www.apache.org/licenses/LICENSE-2.0
*
*   Unless required by applicable law or agreed to in writing, software
*   distributed under the License is distributed on an "AS IS" BASIS,
*   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*   See the License for the specific language governing permissions and
*   limitations under the License.
*/

#include "stats.h"
#include "../event.h"
#include "../player.h"
#include "../entity.h"
#include "../world.h"
#include "../eventargs.h"

Stats::Stats(void)
{
    SetName("stats");
    RegisterEvent("StatChanged", new Event());
    GetEvent("OnAttach")->Add(STATS_ATTACHED);
}

COMCREATE(STATS_CREATE)
{
    return new Stats();
}
COMINIT(STATS_INIT)
{
    world->GetEvent("PlayerCreated")->Add(STATS_ADD);
    world->GetEvent("PlayerConnect")->Add(STATS_ADD);
}

EVENT(STATS_ATTACHED)
{
    OneArg* arg = (OneArg*)args;
    Entity* obj = (Entity*) arg->_arg;

    obj->AddVar("str", 1);
    obj->AddVar("con", 1);
    obj->AddVar("dex", 1);
    obj->AddVar("int", 1);
    obj->AddVar("wis", 1);
}

EVENT(STATS_ADD) //called for new player and player logins.
{
    Player* mob=(Player*)caller;
    mob->AddComponent(world->CreateComponent("stats"));
}
