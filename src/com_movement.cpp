/*
*com_movement.cpp
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

#include <string>
#include "com_movement.h"
#include "player.h"
#include "world.h"
#include "room.h"
#include "exit.h"

void InitializeMovementCommands(void)
{
    world->WriteLog("Initializing movement commands.");
    world->commands.AddCommand(new CMDNorth());
    world->commands.AddCommand(new CMDSouth());
    world->commands.AddCommand(new CMDEast());
    world->commands.AddCommand(new CMDWest());
    world->commands.AddCommand(new CMDNortheast());
    world->commands.AddCommand(new CMDNorthwest());
    world->commands.AddCommand(new CMDSoutheast());
    world->commands.AddCommand(new CMDSouthwest());
    world->commands.AddCommand(new CMDUp());
    world->commands.AddCommand(new CMDDown());
}

BOOL DoMove(const std::string &verb, Player* mobile,std::vector<std::string> &args,int subcmd)
{
    std::string direction;
    Room* room;
    Room* dest;
    Exit* exit;

    switch (subcmd) {
    case DIR_NORTH:
        direction="north";
        break;
    case DIR_SOUTH:
        direction="south";
        break;
    case DIR_EAST:
        direction="east";
        break;
    case DIR_WEST:
        direction="west";
        break;
    case DIR_NORTHEAST:
        direction="northeast";
        break;
    case DIR_NORTHWEST:
        direction="northwest";
        break;
    case DIR_SOUTHEAST:
        direction="southeast";
        break;
    case DIR_SOUTHWEST:
        direction="southwest";
        break;
    case DIR_UP:
        direction="up";
        break;
    case DIR_DOWN:
        direction="down";
        break;
    default:
        world->WriteLog("Invalid subcmd in do_move");
        return false;
        break;
    }
    room=(Room*)mobile->GetLocation();

    if (!room) {
        mobile->Message(MSG_ERROR,"There are no exits here.");
        return false;
    }

    if (!room->ExitExists(direction)) {
        mobile->Message(MSG_ERROR,"You can't go that way!");
        return false;
    }

    exit=room->GetExit(direction);
    if (!exit->CanEnter(mobile)) {
        mobile->Message(MSG_ERROR,"Something prevents you from going that direction.");
        return false;
    }

//call the event to signal that the player is moving
    room->events.CallEvent("OnExit", NULL, (void*)mobile);
    dest = world->GetRoom(exit->GetTo());
    mobile->MoveTo(dest);
    dest->events.CallEvent("OnEnter", NULL, (void*)mobile);

    mobile->Message(MSG_INFO, world->GetRoom(exit->GetTo())->DoLook(mobile));
    return true;
}

CMDNorth::CMDNorth()
{
    SetName("north");
    AddAlias("n");
    SetSubcmd(DIR_NORTH);
}
BOOL CMDNorth::Execute(const std::string &verb, Player* mobile,std::vector<std::string> &args,int subcmd)
{
    return DoMove(verb, mobile, args, subcmd);
}

CMDSouth::CMDSouth()
{
    SetName("south");
    AddAlias("s");
    SetSubcmd(DIR_SOUTH);
}
BOOL CMDSouth::Execute(const std::string &verb, Player* mobile,std::vector<std::string> &args,int subcmd)
{
    return DoMove(verb, mobile, args, subcmd);
}

CMDEast::CMDEast()
{
    SetName("east");
    AddAlias("e");
    SetSubcmd(DIR_EAST);
}
BOOL CMDEast::Execute(const std::string &verb, Player* mobile,std::vector<std::string> &args,int subcmd)
{
    return DoMove(verb, mobile, args, subcmd);
}

CMDWest::CMDWest()
{
    SetName("west");
    AddAlias("w");
    SetSubcmd(DIR_WEST);
}
BOOL CMDWest::Execute(const std::string &verb, Player* mobile,std::vector<std::string> &args,int subcmd)
{
    return DoMove(verb, mobile, args, subcmd);
}

CMDNortheast::CMDNortheast()
{
    SetName("northeast");
    AddAlias("ne");
    SetSubcmd(DIR_NORTHEAST);
}
BOOL CMDNortheast::Execute(const std::string &verb, Player* mobile,std::vector<std::string> &args,int subcmd)
{
    return DoMove(verb, mobile, args, subcmd);
}

CMDSoutheast::CMDSoutheast()
{
    SetName("southeast");
    AddAlias("se");
    SetSubcmd(DIR_SOUTHEAST);
}
BOOL CMDSoutheast::Execute(const std::string &verb, Player* mobile,std::vector<std::string> &args,int subcmd)
{
    return DoMove(verb, mobile, args, subcmd);
}

CMDSouthwest::CMDSouthwest()
{
    SetName("southwest");
    AddAlias("sw");
    SetSubcmd(DIR_SOUTHWEST);
}
BOOL CMDSouthwest::Execute(const std::string &verb, Player* mobile,std::vector<std::string> &args,int subcmd)
{
    return DoMove(verb, mobile, args, subcmd);
}

CMDNorthwest::CMDNorthwest()
{
    SetName("northwest");
    AddAlias("nw");
    SetSubcmd(DIR_NORTHWEST);
}
BOOL CMDNorthwest::Execute(const std::string &verb, Player* mobile,std::vector<std::string> &args,int subcmd)
{
    return DoMove(verb, mobile, args, subcmd);
}

CMDUp::CMDUp()
{
    SetName("up");
    AddAlias("u");
    SetSubcmd(DIR_UP);
}
BOOL CMDUp::Execute(const std::string &verb, Player* mobile,std::vector<std::string> &args,int subcmd)
{
    return DoMove(verb, mobile, args, subcmd);
}

CMDDown::CMDDown()
{
    SetName("down");
    AddAlias("d");
    SetSubcmd(DIR_DOWN);
}
BOOL CMDDown::Execute(const std::string &verb, Player* mobile,std::vector<std::string> &args,int subcmd)
{
    return DoMove(verb, mobile, args, subcmd);
}
