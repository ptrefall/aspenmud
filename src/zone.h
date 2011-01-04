/*
*zone.h
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


#ifndef ZONE_H
#define ZONE_H
#include "mud.h"
#include "conf.h"
#include "event.h"
#include "room.h"
#include "serializer.hpp"

///Zones are used to separate areas.
/**
*Zones are used to allow a builder to define an area, which will contain rooms, (a list of Room objects).
*This is useful for determining where a mob can wander (not allowing it outside it's zone), etc.
*/
class Zone
{
    std::string _name;
    std::list <VNUM> _rnums;
    int _maxRooms;
public:
    Zone();
    ~Zone();
    /**
    *Retrieves the set name of the zone.
    *\return The name of the zone, or "" if a name hasn't been set.
    */
    std::string GetName(void) const;
    /**
    *Sets the name of the zone object.
    *\param name The name of the zone.
    */
    void SetName(const std::string &name);

    /**
    *Retrieves the number of rooms that this zone can contain.
    *\return The number of max rooms.
    *\note The number of max rooms returned may or may not be higher than the actual amount of rooms in the zone.
    */
    int GetMaxRooms(void) const;
    /**
    *Sets the number of max rooms that the zone can hold.
    *\param max [in] The number of rooms this zone should be able to hold.
    *\note The number of max rooms set may or may not be higher than the actual amount of rooms in the zone. The function will only
    *set the number of rooms and prevent adding of rooms if the limit has been reached.
    */
    void SetMaxRooms(int max);

    /**
    *Adds the specified room.
    *\param num [in] the VNUM of the room to add.
    *\return True if the room was added successfully, false otherwise.
    *\note The zone pointer on the Room object is set to the zone to which the room was added.
    */
    BOOL AddRoom(VNUM num);
    BOOL RemoveRoom(VNUM num);
    void GetRooms(std::list<VNUM>* rooms);

    virtual void Serialize(Serializer& ar);
    virtual void Deserialize(Serializer& ar);
};

BOOL InitializeZones(void);
BOOL SaveZones(void);
BOOL LoadZones();

EVENT(AUTOSAVE_ZONES);
EVENT(SHUTDOWN_ZONES);
#endif
