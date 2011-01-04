/*
*entity.cpp
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
#include <map>
#include "entity.h"
#include "world.h"
#include "room.h"
#include "eventargs.h"
#include "utils.h"
#include "command.h"
#include "olc.h"
#include "serializer.hpp"
#ifdef OLC
OLC_INPUT(olc_entity_name);
#endif

Entity::Entity(void)
{
    _name="A blank object";
    _desc="You see nothing special.";
    _script="--scripting starts here\n";
    _location=NULL;
    _type=1;
    _onum=0;
    _components=new std::list <Component*>();
    _vars=new std::map <std::string,Variant>();
#ifdef OLC
    _olcs = new std::vector<struct OLC_DATA*>();
    AddOlc("name", "Please enter the name of the object", STRING, olc_entity_name);
#endif

    RegisterEvent("PostLook", new Event());
    RegisterEvent("PreLook",new Event());
}
Entity::~Entity(void)
{
    std::list <Component*>::iterator it;
#ifdef OLC
    std::vector<struct OLC_DATA*>::iterator oit;
#endif
    for (it=_components->begin(); it!=_components->end(); it++) {
        delete (*it);
    }
    delete _components;
    delete _vars;
#ifdef OLC
    for (oit=_olcs->begin(); oit!=_olcs->end(); oit++) {
        delete (*oit);
    }
    delete _olcs;
#endif
}

std::string Entity::GetName(void) const
{
    return _name;
}
void Entity::SetName(const std::string &s)
{
    _name=s;
}

std::string Entity::GetDescription(void) const
{
    return _desc;
}
void Entity::SetDescription(const std::string &s)
{
    _desc=s;
}

std::string Entity::GetScript(void) const
{
    return _script;
}
void Entity::SetScript(const std::string &script)
{
    _script=script;
}

Entity* Entity::GetLocation(void) const
{
    return _location;
}
void Entity::SetLocation(Entity* location)
{
    _location=location;
}

int Entity::GetType(void) const
{
    return _type;
}
void Entity::SetType(int type)
{
    _type=type;
}

VNUM Entity::GetOnum(void) const
{
    return _onum;
}
void Entity::SetOnum(VNUM num)
{
    _onum=num;
}

void Entity::Serialize(Serializer& ar)
{
    int i=0;
    int size=_components->size();
    std::string key;
    std::list <Component*>::iterator it;
    std::map<std::string, Variant>::iterator it2;

    ar << _name;
    ar << _desc;
    ar << _script;

    ar << size;
    if (size) {
        for (it=_components->begin(); it!=_components->end(); it++) {
            key=(*it)->GetName();
            ar << key;
        }
    }

    size=_vars->size();
//go through and make sure we're not serializing persistent vars.
    /*
        if (size) {
            for (it2=_vars->begin(); it2!=_vars->end(); it2++) {
                if (((*it2).second.Typeof()==VAR_EMPTY)||(!(*it2).second.GetPersistents())) {
                    size--;
                }
            }
        }
    */

    ar << size;
    if (size) {
        for (it2=_vars->begin(); it2!=_vars->end(); it2++) {
            ar << (*it2).first;
            (*it2).second.Serialize(ar);
        }
    }

    ar << _onum;
    ar << _type;

    if (_location) {
        i=_location->GetOnum();
        ar << i;
        i=0;
    } else {
        ar << i;
    }

    SerializeObjectList<Entity>(ar, _contents);
}
void Entity::Deserialize(Serializer& ar)
{
    int loc;
    int size,i;
    std::string name;
    Variant var;

    ar >> _name;
    ar >> _desc;
    ar >> _script;

    ar >> size;
    if (size) {
        for (i=0; i<size; i++) {
            ar >> name;
            AddComponent(world->CreateComponent(name));
        }
    }

    ar >> size;
    if (size) {
        for (i=0; i<size; i++) {
            ar >> name;
            var.Deserialize(ar);
            (*_vars)[name]=var;
        }
    }

    ar >> _onum;
    ar >> _type;
    ar >> loc;
    if (!loc) {
        _location=NULL;
    } else {
        if (_type==1) {
//it's an object
            _location=world->GetObject(loc);
        } else if (_type==2) {
//it's a room
            _location=world->GetRoom(loc);
        }
    }

    DeserializeObjectList<Entity>(ar, _contents);
}

std::string Entity::DoLook(Player* mobile)
{
    std::string str;

    LookArgs* args=new LookArgs(mobile,this,str);
    CallEvent("PreLook", args, (void*)mobile);
    str+=Capitalize(this->GetName())+"\n";
    str+=this->GetDescription()+"\n";
    CallEvent("PostLook", args, (void*)mobile);
    delete args;
    return str;
}

BOOL Entity::CanReceive(Entity* obj) const
{
    return true;
}

BOOL Entity::MoveTo(Entity* targ)
{
    if (targ->CanReceive(this)) {
        _location=targ;
        return true;
    }
    return false;
}


BOOL Entity::AddComponent(Component* component)
{
    if (component==NULL) {
        return false;
    }

    if (HasComponent(component->GetName())) {
        return false;
    }
    _components->push_back(component);
    component->Attach(this);
    return true;
}
BOOL Entity::RemoveComponent(Component* component)
{
    if (!HasComponent(component->GetName())) {
        return false;
    }
    _components->remove(component);
    component->Detach();
    return true;
}

bool Entity::HasComponent(const std::string &name)
{
    std::list <Component*>::iterator it;

    for (it=_components->begin(); it!=_components->end(); it++) {
        if ((*it)->GetName()==name) {
            return true;
        }
    }
    return false;
}

void Entity::Attach(Entity* obj)
{
    std::list <Component*>::iterator it;
    for (it=_components->begin(); it!=_components->end(); it++) {
        (*it)->Attach(obj);
    }
}

BOOL Entity::RemoveVar(const std::string &name)
{
    if (VarExists(name)) {
        _vars->erase(name);
        return true;
    }
    return false;
}
/*
*Checks to see if the specified variable exists
*Param: [in] the name of the variable.
*Return: true if the variable exists, false otherwise.
*/
BOOL Entity::VarExists(const std::string &name) const
{
    return (_vars->count(name));
}

void Entity::SetPersistents(const std::string &name, BOOL s)
{
    if (!VarExists(name)) {
        throw(VarNotFoundException(std::string("Variable ")+name+std::string(" doesn't exist. ")+WHERE()));
    }

    (*_vars)[name].SetPersistents(s);
}
BOOL Entity::GetPersistents(const std::string &name) const
{
    if (!VarExists(name)) {
        throw(VarNotFoundException(std::string("Variable ")+name+std::string(" doesn't exist. ")+WHERE()));
    }

    return (*_vars)[name].GetPersistents();
}

int Entity::GetInt(const std::string &name) const
{
    if (!VarExists(name)) {
        throw(VarNotFoundException(std::string("Variable ")+name+std::string(" doesn't exist. ")+WHERE()));
    }

    return (*_vars)[name].GetInt();
}
UINT Entity::GetUint(const std::string &name) const
{
    if (!VarExists(name)) {
        throw(VarNotFoundException(std::string("Variable ")+name+std::string(" doesn't exist. ")+WHERE()));
    }

    return (*_vars)[name].GetUint();
}
unsigned short Entity::GetUshort(const std::string &name) const
{
    if (!VarExists(name)) {
        throw(VarNotFoundException(std::string("Variable ")+name+std::string(" doesn't exist. ")+WHERE()));
    }

    return (*_vars)[name].GetUshort();
}
short Entity::GetShort(const std::string &name) const
{
    if (!VarExists(name)) {
        throw(VarNotFoundException(std::string("Variable ")+name+std::string(" doesn't exist. ")+WHERE()));
    }

    return (*_vars)[name].GetShort();
}
char Entity::GetByte(const std::string &name) const
{
    if (!VarExists(name)) {
        throw(VarNotFoundException(std::string("Variable ")+name+std::string(" doesn't exist. ")+WHERE()));
    }

    return (*_vars)[name].GetByte();
}
unsigned char Entity::GetUbyte(const std::string &name) const
{
    if (!VarExists(name)) {
        throw(VarNotFoundException(std::string("Variable ")+name+std::string(" doesn't exist. ")+WHERE()));
    }

    return (*_vars)[name].GetUbyte();
}
float Entity::GetFloat(const std::string &name) const
{
    if (!VarExists(name)) {
        throw(VarNotFoundException(std::string("Variable ")+name+std::string(" doesn't exist. ")+WHERE()));
    }

    return (*_vars)[name].GetFloat();
}
std::string Entity::GetString(const std::string &name) const
{
    if (!VarExists(name)) {
        throw(VarNotFoundException(std::string("Variable ")+name+std::string(" doesn't exist. ")+WHERE()));
    }

    return (*_vars)[name].GetStr();
}

void Entity::SetInt(const std::string &name, int s)
{
    if (!VarExists(name)) {
        throw(VarNotFoundException(std::string("Variable ")+name+std::string(" doesn't exist. ")+WHERE()));
    }

    (*_vars)[name].SetInt(s);
}
void Entity::SetUint(const std::string &name, unsigned int s)
{
    if (!VarExists(name)) {
        throw(VarNotFoundException(std::string("Variable ")+name+std::string(" doesn't exist. ")+WHERE()));
    }

    (*_vars)[name].SetUint(s);
}
void Entity::SetUshort(const std::string &name, unsigned short s)
{
    if (!VarExists(name)) {
        throw(VarNotFoundException(std::string("Variable ")+name+std::string(" doesn't exist. ")+WHERE()));
    }

    (*_vars)[name].SetUshort(s);
}
void Entity::SetShort(const std::string &name, short s)
{
    if (!VarExists(name)) {
        throw(VarNotFoundException(std::string("Variable ")+name+std::string(" doesn't exist. ")+WHERE()));
    }

    (*_vars)[name].SetShort(s);
}
void Entity::SetUbyte(const std::string &name, unsigned char s)
{
    if (!VarExists(name)) {
        throw(VarNotFoundException(std::string("Variable ")+name+std::string(" doesn't exist. ")+WHERE()));
    }

    (*_vars)[name].SetUbyte(s);
}
void Entity::SetByte(const std::string &name, char s)
{
    if (!VarExists(name)) {
        throw(VarNotFoundException(std::string("Variable ")+name+std::string(" doesn't exist. ")+WHERE()));
    }

    (*_vars)[name].SetByte(s);
}
void Entity::SetFloat(const std::string &name, float s)
{
    if (!VarExists(name)) {
        throw(VarNotFoundException(std::string("Variable ")+name+std::string(" doesn't exist. ")+WHERE()));
    }

    (*_vars)[name].SetFloat(s);
}
void Entity::SetDouble(const std::string &name, double s)
{
    if (!VarExists(name)) {
        throw(VarNotFoundException(std::string("Variable ")+name+std::string(" doesn't exist. ")+WHERE()));
    }

    (*_vars)[name].SetDouble(s);
}
void Entity::SetStr(const std::string &name, const std::string &s)
{
    if (!VarExists(name)) {
        throw(VarNotFoundException(std::string("Variable ")+name+std::string(" doesn't exist. ")+WHERE()));
    }

    (*_vars)[name].SetStr(s);
}
void Entity::SetStr(const std::string &name, const char* s)
{
    if (!VarExists(name)) {
        throw(VarNotFoundException(std::string("Variable ")+name+std::string(" doesn't exist. ")+WHERE()));
    }

    (*_vars)[name].SetStr(s);
}

Variant& Entity::operator [](const std::string &name) const
{
    if (!VarExists(name)) {
        throw(VarNotFoundException(std::string("Variable ")+name+std::string(" doesn't exist. ")+WHERE()));
    }

    return (*_vars)[name];
}
Variant& Entity::operator [](const char* name) const
{
    if (!VarExists(name)) {
        throw(VarNotFoundException(std::string("Variable ")+name+std::string(" doesn't exist. ")+WHERE()));
    }

    return (*_vars)[name];
}

#ifdef OLC
void Entity::AddOlc(const std::string &name, const std::string &prompt, OLC_INPUT_TYPE type, FP_INPUT callback)
{
    struct OLC_DATA* data = NULL;
    if ((name == "") || (callback == NULL)) {
        return;
    }

    data = new OLC_DATA();
    data->name = name;
    data->prompt = prompt;
    data-> type = type;
    data->callback = callback;
    _olcs->push_back(data);
}
BOOL Entity::RemoveOlc(const std::string &name)
{
    std::vector<struct OLC_DATA*>::iterator oit;
    for (oit=_olcs->begin(); oit != _olcs->end(); oit++) {
        if ((*oit)->name == name) {
            _olcs->erase(oit);
            return true;
        }
    }

    return false;
}
void Entity::ListOlcs(std::vector<struct OLC_DATA*> &out)
{
    std::vector<struct OLC_DATA*>::iterator it;
    std::vector<struct OLC_DATA*>::iterator end = _olcs->end();
    for (it = _olcs->begin(); it != end; it++) {
        out.push_back((*it));
    }
}
OLC_DATA* Entity::GetOlcByIndex(int index)
{
    index--;

    if (((int)_olcs->size() > index+1) || (index < 0 )) {
        return NULL;
    }

    return (_olcs->at(index));
}

OLC_INPUT(olc_entity_name)
{
    if (input->Typeof() != VAR_STR) {
        mob->Message(MSG_ERROR, "Invalid input.");
        return;
    }

//we have a string, lets set that to the name.
    ed->SetName(input->GetStr());
}
#endif
