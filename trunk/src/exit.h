/*
*exit.h
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
*A basic exit object;
*Can be inherited to create doors, etc.
*/
#ifndef EXIT_H
#define EXIT_H
#include <string>
#include "mud.h"
#include "conf.h"
#include "living.h"
#include "serializer.hpp"

class Exit
{
    VNUM _to; //the vnum that the exit points to.
    VNUM _from; //where the exit is located.
    std::string _name; //the name of the exit
    std::string _alias; //an alias for the exit.
public:
    Exit(VNUM from,VNUM to);
    Exit(void);
    virtual ~Exit(void);

//getters and setters:
    virtual VNUM GetFrom(void) const;
    virtual void SetFrom(VNUM from);
    virtual VNUM GetTo(void) const;
    virtual void SetTo(VNUM to);
    virtual std::string GetName(void) const;
    virtual void SetName(const std::string &name);
    virtual std::string GetAlias(void) const;
    virtual void SetAlias(const std::string &alias);

    /*
    *Checks to see if the npc/player can enter through the exit.
    *Return: true if the player/npc can enter, false otherwise.
    */
    virtual BOOL CanEnter(Living* mobile);

//serialization
    virtual void Serialize(Serializer& ar);
    virtual void Deserialize(Serializer& ar);
};

#endif