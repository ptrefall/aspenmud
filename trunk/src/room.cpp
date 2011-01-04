/*
*room.cpp
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


#include <sstream>
#include <string>
#include <list>
#include "room.h"
#include "living.h"
#include "exit.h"
#include "player.h"
#include "event.h"
#include "eventargs.h"
#include "zone.h"
#include "serializer.hpp"

Room::Room(void)
{
    RegisterEvent("PostLook", new Event());
    RegisterEvent("OnEnter", new Event());
    RegisterEvent("OnExit", new Event());
    RegisterEvent("OnLook", new Event());

    _zone=NULL;
    SetOnum(ROOM_NOWHERE);
    SetType(2);
    GetEvent("PostLook")->Add(ROOM_POST_LOOK);
}
Room::~Room(void)
{
    std::list <Exit*>::iterator it;

//delete exits:
    if (_exits.size()) {
        for (it=_exits.begin(); it!=_exits.end(); it++) {
            delete (*it);
        }
    }
}

BOOL Room::AddExit(Exit* exit)
{
    std::list<Exit*>::iterator it;
    if (exit==NULL) {
        return false;
    }

    for (it=_exits.begin(); it!=_exits.end(); it++) {
        if (exit==(*it)) {
            return false;
        }
    }

    if (ExitExists(exit->GetName())) {
        return false;
    }

    _exits.push_back(exit);
    return true;
}

BOOL Room::ExitExists(const std::string &name)
{
    std::list<Exit*>::iterator it;
    for (it=_exits.begin(); it!=_exits.end(); it++) {
        if (((*it)->GetName()==name)||((*it)->GetAlias()==name)) {
            return true;
        }
    }
    return false;
}

Exit* Room::GetExit(const std::string &name)
{
    std::list<Exit*>::iterator it;

    for (it=_exits.begin(); it!=_exits.end(); it++) {
        if ((*it)->GetName()==name) {
            return (*it);
        }
    }
    return NULL;
}
std::list<Exit*>* Room::GetExits(void)
{
    return &_exits;
}

void Room::SetZone(Zone* s)
{
    _zone=s;
}
Zone* Room::GetZone(void) const
{
    return _zone;
}

void Room::TellAll(const std::string &message)
{
    std::list<Living*>::iterator it;
    std::list<Living*>::iterator itEnd;

    itEnd = _mobiles.end();
    for (it = _mobiles.begin(); it != itEnd; ++it) {
        if ((*it)->IsPlayer()) {
            ((Player*)(*it))->Message(MSG_INFO,message);
        }
    }
}
void Room::TellAllBut(const std::string &message, std::list <Player*>* players)
{
    std::list<Player*> left;
    std::list<Player*>::iterator pit;
    std::list<Player*>::iterator pitEnd;
    std::list <Living*>::iterator lit;
    std::list <Living*>::iterator litEnd;
    BOOL found = false;

    pitEnd = players->end();
    litEnd = _mobiles.end();

    for (lit = _mobiles.begin(); lit != litEnd; ++lit) {
        for (pit = players->begin(); pit != pitEnd; ++pit) {
            if ((*lit) == (*pit)) {
                found = true;
                break;
            }
        }
        if (!found) {
            left.push_back((Player*)(*lit));
        }
        found = false;
    }

    pitEnd = left.end();
    for (pit = left.begin(); pit != pitEnd; ++pit) {
        (*pit)->Message(MSG_INFO, message);
    }
}

void Room::Serialize(Serializer& ar)
{
    std::list <Exit*>::iterator it;
    int size=_exits.size();

    Entity::Serialize(ar);

    ar << size;
    if (size) {
        for (it=_exits.begin(); it!=_exits.end(); it++) {
            (*it)->Serialize(ar);
        }
    }
}
void Room::Deserialize(Serializer& ar)
{
    int size,i;
    Exit* exit=NULL;

    Entity::Deserialize(ar);

    ar >> size;

    if (size) {
        for (i=0; i<size; i++) {
            exit=new Exit();
            exit->Deserialize(ar);
            _exits.push_back(exit);
            exit=NULL;
        }
    }
}


//events
EVENT(ROOM_POST_LOOK)
{
    std::list<Exit*>::iterator it;
    std::stringstream st;
    LookArgs* largs=(LookArgs*)args;
    Room* targ=(Room*)largs->_targ;

    if (!targ->GetExits()->size()) {
        largs->_desc+="You see no obvious exits.";
    } else {
        st << "Obvious exits: [";
        for (it=targ->GetExits()->begin(); it!=targ->GetExits()->end(); it++) {
            st << (*it)->GetName();
        }
        st << "].";
        largs->_desc+=st.str();
    }
}
