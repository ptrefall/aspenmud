#include <string>
#include <list>
#include <tinyxml.h>
#include <cstdio>
#include <sys/stat.h>
#include "zone.h"
#include "world.h"
#include "room.h"
#include "event.h"

static int zone_saves;

Zone::Zone()
{
    _maxRooms=100;
}
Zone::~Zone()
{
//we need to clean up our rooms.
    std::list<VNUM>::iterator it;
    std::list<VNUM>::iterator itEnd;
    Room* room = NULL;

    itEnd = _rnums.end();
    for (it = _rnums.begin(); it != itEnd; ++it) {
        room = world->GetRoom((*it));
        world->RemoveRoom((*it));
        delete room;
    }
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

void Zone::Serialize(TiXmlElement* root)
{
    std::list<VNUM>::iterator it;
    std::list<VNUM>::iterator itEnd = _rnums.end();
    Room* room = NULL;
    TiXmlElement* rooms = new TiXmlElement("rooms");
    TiXmlElement* zone = new TiXmlElement("zone");

    if (_rnums.size()) {
        for (it = _rnums.begin(); it != itEnd; ++it) {
            room=world->GetRoom((*it));
            if (room) {
                room->Serialize(rooms);
                room=NULL;
            }
        }
    }
    zone->LinkEndChild(rooms);

    zone->SetAttribute("name", _name.c_str());
    zone->SetAttribute("maxRooms", _maxRooms);
    root->LinkEndChild(zone);
}
void Zone::Deserialize(TiXmlElement* zone)
{
    Room* room=NULL;
    TiXmlElement* rooms = NULL;
    TiXmlElement* iterator = NULL;
    TiXmlNode* node = NULL;
    node = zone->FirstChild("rooms");
    if (!node) {
        return;
    }
    rooms = node->ToElement();

    for (node = rooms->FirstChild(); node; node = node->NextSibling()) {
        iterator = node->ToElement();
        room=new Room();
        room->Deserialize(iterator);
        world->AddRoom(room->GetOnum(),room);
        AddRoom(room->GetOnum());
        room=NULL;
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
        world->events.AddCallback("WorldPulse", AUTOSAVE_ZONES);
        world->events.AddCallback("Shutdown", SHUTDOWN_ZONES);
        return true;
    }
    return true;
}

BOOL SaveZones(void)
{
    std::list<Zone*> *zones=new std::list <Zone*>();
    world->GetZones(zones);
    std::list<Zone*>::iterator it;
    std::list<Zone*>::iterator itEnd;
    TiXmlDocument doc;
    TiXmlElement* zlist = new TiXmlElement("zones");

    TiXmlDeclaration *decl = new TiXmlDeclaration("1.0", "", "");
    doc.LinkEndChild(decl);

    if (zones->size()) {
        itEnd = zones->end();
        for (it = zones->begin(); it != itEnd; ++it) {
            (*it)->Serialize(zlist);
        }
    }
    delete zones;
    doc.LinkEndChild(zlist);
    doc.SaveFile(AREA_FILE);
    return true;
}
BOOL LoadZones(void)
{
    TiXmlDocument doc(AREA_FILE);
    if (!doc.LoadFile()) {
        return false;
    }

    Zone* zone=NULL;
    TiXmlElement* zones = NULL;
    TiXmlElement* iterator = NULL;
    TiXmlNode* node = NULL;

    node = doc.FirstChild("zones");
    if (!node) {
        return false;
    }
    zones = node->ToElement();


    for (node = zones->FirstChild(); node; node = node->NextSibling()) {
        iterator = node->ToElement();
        zone=new Zone();
        zone->Deserialize(iterator);
        world->AddZone(zone);
    }
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
