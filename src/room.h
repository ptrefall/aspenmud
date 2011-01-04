/*
*room.h
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


/*
*The basic room object
*/
#ifndef ROOM_H
#define ROOM_H
#include <list>
#include <string>
#include "mud.h"
#include "conf.h"
#include "entity.h"
#include "living.h"
#include "exit.h"
#include "player.h"
#include "event.h"
#include "zone.h"
#include "serializer.hpp"

EVENT(ROOM_POST_LOOK);

class Zone;

class Room:public Entity
{
    std::list <Living*> _mobiles;
    std::list <Exit*> _exits;
    Zone* _zone;
public:
    Room(void);
    virtual ~Room(void);
    /*
    *Adds the specified exit.
    *Param: [in] a pointer to the Exit object to be added.
    *Return: True if the exit could be added, false otherwise.
    */
    virtual BOOL AddExit(Exit* exit);
    /*
    *Checks for the existance of an exit either in name, or by alias.
    *Param: [in] the name of the exit to check for.
    *Return: True if the exit exists, false otherwise.
    */
    virtual BOOL ExitExists(const std::string &name);
    /*
    *Finds the specified exit.
    *Param: [in] the name of the exit.
    *Return: a pointer to the exit object found, or NULL if none was found.
    */
    virtual Exit* GetExit(const std::string &name);
    /*
    *Returns a pointer to the list of exits.
    *Return: a pointer to a list of Exit* objects
    */
    std::list<Exit*>* GetExits(void);
    /*
    *Sets the zone object on the room.
    *Param: [in] a pointer to a zone.
    */
    void SetZone(Zone* s);
    /*
    *Gets the zone associated with the room object.
    *Return: The zone for the object, or NULL.
    */
    Zone* GetZone(void) const;
    /*
    *Sends a message to all players in the room.
    *Param: [in] the message to send.
    */
    virtual void TellAll(const std::string &message);
    virtual void TellAllBut(const std::string & message, std::list <Player*>* players);
//serialization
    virtual void Serialize(Serializer& ar);
    virtual void Deserialize(Serializer& ar);
};

#endif
