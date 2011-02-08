#include <sstream>
#include <string>
#include <list>
#include <tinyxml.h>
#include "room.h"
#include "living.h"
#include "exit.h"
#include "player.h"
#include "event.h"
#include "eventargs.h"
#include "zone.h"

Room::Room(void)
{
    events.RegisterEvent("PostLook", new Event());
    events.RegisterEvent("OnEnter", new Event());
    events.RegisterEvent("OnExit", new Event());
    events.RegisterEvent("OnLook", new Event());

    _zone=NULL;
    SetOnum(ROOM_NOWHERE);
    SetType(2);
    events.GetEvent("PostLook")->Add(ROOM_POST_LOOK);
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

void Room::Serialize(TiXmlElement* root)
{
    TiXmlElement* room = new TiXmlElement("room");
    TiXmlElement* exits = new TiXmlElement("exits");
    std::list <Exit*>::iterator it;
    std::list<Exit*>::iterator itEnd = _exits.end();

    if (_exits.size()) {
        for (it=_exits.begin(); it != itEnd; ++it) {
            (*it)->Serialize(exits);
        }
    }
    room->LinkEndChild(exits);
    Entity::Serialize(room);
    root->LinkEndChild(room);
}
void Room::Deserialize(TiXmlElement* room)
{
    TiXmlElement* exit = NULL;
    TiXmlElement* exits = NULL;
    Exit* ex = NULL;

    exits = room->FirstChild("exits")->ToElement();
    for (exit = exits->FirstChild()->ToElement(); exit; exit = exit->NextSibling()->ToElement()) {
        ex = new Exit();
        ex->Deserialize(exit);
        _exits.push_back(ex);
        ex = NULL;
    }

    Entity::Deserialize(room);
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
