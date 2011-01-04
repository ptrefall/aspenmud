/*
*zone.cpp
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
#include <list>
#include <cstdio>
#include <sys/stat.h>
#include "zone.h"
#include "world.h"
#include "room.h"
#include "serializer.hpp"
#include "event.h"

int zone_saves;

Zone::Zone()
{
    _maxRooms=100;
}
Zone::~Zone()
{
}

std::string Zone::GetName(void) const
{
    return _name;
}
void Zone::SetName(const std::string &name)
{
    _name=name;
}

int Zone::GetMaxRooms(void) const
{
    return _maxRooms;
}
void Zone::SetMaxRooms(int max)
{
    _maxRooms=max;
}

BOOL Zone::AddRoom(VNUM num)
{
    std::list<VNUM>::iterator it;

    for (it=_rnums.begin(); it!=_rnums.end(); it++) {
        if ((*it)==num) {
            return false;
        }
    }
    _rnums.push_back(num);
    world->GetRoom(num)->SetZone(this);
    return true;
}
BOOL Zone::RemoveRoom(VNUM num)
{
    std::list<VNUM>::iterator it;

    for (it=_rnums.begin(); it!=_rnums.end(); it++) {
        if ((*it)==num) {
            _rnums.erase(it);
            world->RemoveRoom(num);
            return true;
        }
    }
    return false;
}
void Zone::GetRooms(std::list<VNUM>* rooms)
{
    std::list<VNUM>::iterator it;
    for (it=_rnums.begin(); it!=_rnums.end(); it++) {
        rooms->push_back((*it));
    }
}

void Zone::Serialize(Serializer& ar)
{
    std::list<VNUM>::iterator it;
    Room* room;
    int size=_rnums.size();

    ar << _name;
    ar << _maxRooms;
    ar << size;

//now we serialize all the rooms belonging to the zone
//We will use the vnums stored, in order to retrieve a pointer from the global room-object table.

    if (size) {
        for (it=_rnums.begin(); it!=_rnums.end(); it++) {
            room=world->GetRoom((*it));
            if (room) {
                room->Serialize(ar);
                room=NULL;
            }
        }
    }
}
void Zone::Deserialize(Serializer& ar)
{
    int size,i;
    Room* room=NULL;

    ar >> _name;
    ar >> _maxRooms;
    ar >> size;
    if (size) {
        for (i=0; i<size; i++) {
            room=new Room();
            room->Deserialize(ar);
            world->AddRoom(room->GetOnum(),room);
            AddRoom(room->GetOnum());
            room=NULL;
        }
    }
}


BOOL InitializeZones(void)
{
    struct stat FInfo;
    std::string path=AREA_FILE;
    world->WriteLog("Initializing areas.");
    if ((stat(path.c_str(),&FInfo))!=-1) {
        return LoadZones();
    } else {
        world->WriteLog("No area found, creating default.");
//no zones and rooms exist, create a first zone/room.
        Zone*zone=new Zone();
        if (!zone) {
            return false;
        }
        zone->SetName("Start");
        if (!world->AddZone(zone)) {
            return false;
        }
        Room* room=new Room();
        if (!room) {
            return false;
        }
        room->SetName("A blank room");
        if (!world->CreateRoom(room)) {
            return false;
        }
        if (!zone->AddRoom(room->GetOnum())) {
            return false;
        }
        if (!SaveZones()) {
            return false;
        }
        zone_saves = 0;
        world->AddCallback("WorldPulse", AUTOSAVE_ZONES);
        world->AddCallback("Shutdown", SHUTDOWN_ZONES);
        return true;
    }
    return true;
}

BOOL SaveZones(void)
{
    std::list<Zone*> *zones=new std::list <Zone*>();
    world->GetZones(zones);
    std::list<Zone*>::iterator it;
    FILE* output=fopen(AREA_FILE, "wb");
    Serializer* s=new Serializer(output, WRITE);
    int size=zones->size();

    (*s) << size;

    if (size) {
        for (it=zones->begin(); it!=zones->end(); it++) {
            (*it)->Serialize(*s);
        }
    }
    delete zones;
    delete s;
    return true;
}
BOOL LoadZones(void)
{
    int size,i;
    Zone* zone=NULL;
    FILE* input=fopen(AREA_FILE, "rb");
    Serializer*s =new Serializer(input, READ);
    (*s) >> size;

    if (size) {
        for (i=0; i<size; i++) {
            zone=new Zone();
            zone->Deserialize(*s);
            world->AddZone(zone);
        }
    }
    delete s;
    return true;
}

EVENT(AUTOSAVE_ZONES)
{
    zone_saves++;
    if (zone_saves >= 150) {
        SaveZones();
        zone_saves = 0;
    }
}
EVENT(SHUTDOWN_ZONES)
{
    SaveZones();
}
