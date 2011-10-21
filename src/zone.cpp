#include <string>
#include <list>
#include <tinyxml.h>
#include <boost/bind.hpp>
#include <sys/stat.h>
#include "zone.h"
#include "world.h"
#include "room.h"
#include "event.h"
#include "utils.h"

static int zone_saves;

Zone::Zone()
{
  _maxRooms=100;
}
Zone::~Zone()
{
  World* world = World::GetPtr();

//we need to clean up our rooms.
  std::list<VNUM>::iterator it;
  std::list<VNUM>::iterator itEnd;
  Room* room = NULL;

  itEnd = _rnums.end();
  for (it = _rnums.begin(); it != itEnd; ++it)
    {
      room = (Room*)world->GetObject((*it));
      world->RecycleObject((Entity*)room);
    }
}

std::string Zone::GetName() const
{
  return _name;
}
void Zone::SetName(const std::string &name)
{
  _name=name;
}

int Zone::GetMaxRooms() const
{
  return _maxRooms;
}
void Zone::SetMaxRooms(int max)
{
  _maxRooms=max;
}

BOOL Zone::AddRoom(VNUM num)
{
  World* world = World::GetPtr();

  std::list<VNUM>::iterator it;

  for (it=_rnums.begin(); it!=_rnums.end(); it++)
    {
      if ((*it)==num)
        {
          return false;
        }
    }
  _rnums.push_back(num);
  ((Room*)world->GetObject(num))->SetZone(this);
  return true;
}
BOOL Zone::RemoveRoom(VNUM num)
{
  World* world = World::GetPtr();
  std::list<VNUM>::iterator it;

  for (it=_rnums.begin(); it!=_rnums.end(); it++)
    {
      if ((*it)==num)
        {
          _rnums.erase(it);
          world->RemoveObject(num);
          return true;
        }
    }
  return false;
}
void Zone::GetRooms(std::list<VNUM>* rooms)
{
  std::list<VNUM>::iterator it;
  for (it=_rnums.begin(); it!=_rnums.end(); it++)
    {
      rooms->push_back((*it));
    }
}

void Zone::Serialize(TiXmlElement* root)
{
  World* world = World::GetPtr();

  std::list<VNUM>::iterator it;
  std::list<VNUM>::iterator itEnd = _rnums.end();
  Room* room = NULL;
  TiXmlElement* rooms = new TiXmlElement("rooms");
  TiXmlElement* zone = new TiXmlElement("zone");

  if (_rnums.size())
    {
      for (it = _rnums.begin(); it != itEnd; ++it)
        {
          room=(Room*)world->GetObject((*it));
          if (room)
            {
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
  World* world = World::GetPtr();

  Room* room=NULL;
  TiXmlElement* rooms = NULL;
  TiXmlElement* iterator = NULL;
  TiXmlNode* node = NULL;
  node = zone->FirstChild("rooms");
  if (!node)
    {
      return;
    }
  rooms = node->ToElement();

  for (node = rooms->FirstChild(); node; node = node->NextSibling())
    {
      iterator = node->ToElement();
      room=new Room();
      room->Deserialize(iterator);
      world->AddObject(room->GetOnum(),room);
      AddRoom(room->GetOnum());
      room=NULL;
    }
}


BOOL InitializeZones()
{
  World* world = World::GetPtr();
  point p;
  struct stat FInfo;
  world->WriteLog("Initializing areas.");
  if ((stat(AREA_FILE,&FInfo))!=-1)
    {
      Zone::LoadZones();
    }
  else
    {
#ifndef NO_INIT_DEFAULTS
      world->WriteLog("No area file exists, and NO_INIT_DEFAULTS was enabled, exiting.");
      return false;
    }
#else
      world->WriteLog("No area found, creating default.");
//no zones and rooms exist, create a first zone/room.
      Zone*zone=new Zone();
      if (!zone)
        {
          return false;
        }
      zone->SetName("Start");
      if (!world->AddZone(zone))
        {
          return false;
        }
      Room* room=new Room();
      if (!room)
        {
          return false;
        }
      room->SetName("A blank room");
      room->SetCoord(p);
      room->SetOnum(ROOM_START);
      if (!world->AddObject(ROOM_START, room))
        {
          world->WriteLog("The start room could not be created. This is probably because ROOM_START would have overwritten the ID of a previously existing object.", ERR);
          return false;
        }
      if (!zone->AddRoom(room->GetOnum()))
        {
          return false;
        }
      if (!Zone::SaveZones())
        {
          return false;
        }
    }
#endif

  zone_saves = 0;
  world->events.AddCallback("WorldPulse",
                            boost::bind(&Zone::Autosave, _1, _2));
  world->events.AddCallback("Shutdown",
                            boost::bind(&Zone::Shutdown, _1, _2));
  world->events.AddCallback("Copyover",
                            boost::bind(&Zone::Shutdown, _1, _2));
  return true;
}

BOOL Zone::SaveZones()
{
  World* world = World::GetPtr();

  std::vector<Zone*> *zones=new std::vector<Zone*>();
  world->GetZones(zones);
  std::vector<Zone*>::iterator it;
  std::vector<Zone*>::iterator itEnd;
  TiXmlDocument doc;
  TiXmlElement* zlist = new TiXmlElement("zones");

  TiXmlDeclaration *decl = new TiXmlDeclaration("1.0", "", "");
  doc.LinkEndChild(decl);

  if (zones->size())
    {
      itEnd = zones->end();
      for (it = zones->begin(); it != itEnd; ++it)
        {
          (*it)->Serialize(zlist);
        }
    }
  delete zones;
  doc.LinkEndChild(zlist);
  doc.SaveFile(AREA_FILE);
  return true;
}
BOOL Zone::LoadZones()
{
  World* world = World::GetPtr();
  TiXmlDocument doc(AREA_FILE);
  if (!doc.LoadFile())
    {
      return false;
    }

  Zone* zone=NULL;
  TiXmlElement* zones = NULL;
  TiXmlElement* iterator = NULL;
  TiXmlNode* node = NULL;

  node = doc.FirstChild("zones");
  if (!node)
    {
      return false;
    }
  zones = node->ToElement();


  for (node = zones->FirstChild(); node; node = node->NextSibling())
    {
      iterator = node->ToElement();
      zone=new Zone();
      zone->Deserialize(iterator);
      world->AddZone(zone);
    }
  return true;
}

CEVENT(Zone, Autosave)
{
  zone_saves++;
  if (zone_saves >= 150)
    {
      Zone::SaveZones();
      zone_saves = 0;
    }
}
CEVENT(Zone, Shutdown)
{
  Zone::SaveZones();
}
