/*
*living.h
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
*The main living class
*Holds functions needed for both players and mobs.
*/

#ifndef LIVING_H
#define LIVING_H
#include "mud.h"
#include "conf.h"
#include "event.h"
#include "entity.h"
#include "serializer.hpp"

class Living:public Entity
{
public:
    Living();
    virtual ~Living();
    /*
    *This is called when an object enters or leaves the game environment
    */
    virtual void EnterGame(void);
    virtual void LeaveGame(void);
    /*
    *This is updated on a set interval to keep the objects up-to-date.
    *This includes heartbeat, removing uneeded objects, etc.
    */
    virtual void Update();

    /*
    *Find out what type of living this is
    *Return: False
    *Note: Should overwrite this for player and NPC objects to return true for each object.
    */
    virtual BOOL IsNpc(void);
    virtual BOOL IsPlayer(void);

    virtual void Serialize(Serializer& ar);
    virtual void Deserialize(Serializer& ar);
};
#endif
