/*
*com_builder.cpp
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

#include <list>
#include <sstream>
#include <string>
#include "com_builder.h"
#include "world.h"
#include "player.h"
#include "zone.h"
#include "room.h"
#include "utils.h"

void InitializeBuilderCommands()
{
    world->WriteLog("Initializing builder commands.");
    world->AddCommand(new CMDZlist());
    world->AddCommand(new CMDRlist());
    world->AddCommand(new CMDDig());
}

//zlist
CMDZlist::CMDZlist()
{
    SetAccess(RANK_BUILDER);
    SetName("zlist");
}
BOOL CMDZlist::Execute(const std::string &verb, Player* mobile,std::vector<std::string> &args,int subcmd)
{
    std::stringstream st;
    std::list<Zone*> *zones=new std::list <Zone*>();
    std::list<Zone*>::iterator it;
    std::list <Zone*>::iterator itEnd;

    world->GetZones(zones);

    if (zones->size()) {
        st << "Showing " << zones->size() << (zones->size()==1?"zone":"zones") << ".\n";
        itEnd = zones->end();
        for (it = zones->begin(); it != itEnd; ++it) {
            st << ((*it)->GetName()) << "\n";
        }
    } else {
        st << "No zones found.";
    }
    mobile->Message(MSG_LIST,st.str());
    delete zones;

    return true;
}

//rlist
CMDRlist::CMDRlist()
{
    SetName("rlist");
    SetAccess(RANK_BUILDER);
}
BOOL CMDRlist::Execute(const std::string &verb, Player* mobile,std::vector<std::string> &args,int subcmd)
{
    if (!args.size()) {
        mobile->Message(MSG_ERROR,"You need to provide the name of a zone to list rooms for.");
        return false;
    }

    std::stringstream st;
    std::list <VNUM>* nums=new std::list<VNUM>();
    std::list <VNUM>::iterator it;
    std::list<VNUM>::iterator itEnd;
    Lower(args[0]);
    Zone* zone=world->GetZone(args[0]);
    Room* room=NULL;

    if (!zone) {
        mobile->Message(MSG_ERROR,"That zone doesn't exist.");
        delete nums;
        return false;
    }

    zone->GetRooms(nums);
    st << "Found " << (int)nums->size() << ((int)nums->size()==0?"room":"rooms") << ".\n";

    itEnd=nums->end();
    for (it = nums->begin(); it != itEnd; ++it) {
        room=world->GetRoom((*it));
        if (room) {
            st << "[" << room->GetOnum() << "] " << room->GetName() << "\n";
        }
        room=NULL;
    }
    mobile->Message(MSG_LIST,st.str());
    return true;
}

//dig
CMDDig::CMDDig()
{
    SetName("dig");
    SetAccess(RANK_BUILDER);
}
BOOL CMDDig::Execute(const std::string &verb, Player* mobile,std::vector<std::string> &args,int subcmd)
{
    if (!args.size()) {
        mobile->Message(MSG_ERROR,"Syntax: dig <direction>\nCreates another room in the direction you specified.");
        return false;
    }

    Room* room;
    Room* location;
    Zone* zone;
    Exit* orig;
    Exit* other;
    std::string dir=SwapExit(args[0]);
    location=(Room*)mobile->GetLocation();

    if (!location) {
        mobile->Message(MSG_ERROR, "You can't dig here.");
        return false;
    }
    room=new Room();
    world->CreateRoom(room);
    zone=location->GetZone();
    if (zone) {
        room->SetZone(zone);
    }
    zone->AddRoom(room->GetOnum());

    orig=new Exit(location->GetOnum(),room->GetOnum());
    orig->SetName(args[0]);
    location->AddExit(orig);
    if (dir!="") {
        other=new Exit(room->GetOnum(),location->GetOnum());
        room->AddExit(other);
        other->SetName(dir);
    }
    SaveZones();
    return true;
}
