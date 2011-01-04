/*
*helpDb.cpp
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
#include "helpDb.h"
#include "helpNode.h"
#include "helpSection.h"
#include "../player.h"
#include "../world.h"
#include "../utils.h"
#include "../serializer.hpp"

#ifdef MODULE_HELP
HelpDb::HelpDb()
{
    _sections=new std::map<UINT,HelpSection*>();
    _idTable=new std::map<UINT,HelpNode*>();
    _nameTable=new std::map<std::string,HelpNode*>();

    _sid=1;
    _hid=1;
}
HelpDb::~HelpDb(void)
{
//we need to iterate through one of the HelpNode tables
//and the Section table and delete the entries

    std::map<UINT,HelpSection*>::iterator st;
    std::map<UINT,HelpSection*>::iterator stEnd;
    std::map<UINT,HelpNode*>::iterator ht;
    std::map<UINT,HelpNode*>::iterator htEnd;

//first we iterate through section table
    for (st=_sections->begin(); st!=_sections->end(); st++) {
        delete (*st).second;
    }

//and now the help table
    for (ht=_idTable->begin(); ht!=_idTable->end(); ht++) {
        delete (*ht).second;
    }

//and now we delete the tables
    delete _sections;
    _sections=NULL;
    delete _idTable;
    _idTable=NULL;
    delete _nameTable;
    _nameTable=NULL;
}

HelpNode* HelpDb::GetHelpObj(const std::string &name) const
{
    if (!_nameTable->count(name)) {
        return NULL;
    }
    return (*_nameTable)[name];
}

BOOL HelpDb::AddHelpObj(HelpNode* obj)
{

    if (!obj) {
        return false;
    }

    if ((_nameTable->count(obj->GetName()))||(_idTable->count(obj->GetId()))) {
        return false;
    }

    (*_idTable)[obj->GetId()]=obj;
    (*_nameTable)[obj->GetName()]=obj;
    return true;
}

UINT HelpDb::GetSectionByName(const std::string &name)
{
    std::map<UINT,HelpSection*>::iterator it;
    std::map<UINT,HelpSection*>::iterator itEnd = _sections->end();

    for (it=_sections->begin(); it != itEnd; it++) {
        if ((*it).second->GetName()==name) {
            return (*it).second->GetId();
        }
    }
    return -1;
}

HelpSection* HelpDb::GetSection(UINT id) const
{
    if (!_sections->count(id)) {
        return NULL;
    }

    return (*_sections)[id];
}

BOOL HelpDb::AddHelpSection(HelpSection* obj)
{
    if (!obj) {
        return false;
    }

    _hid+=1;
    obj->SetId(_hid);
    (*_sections)[_hid]=obj;
    return true;
}
int HelpDb::GetSectionCount(void) const
{
    return (_sections->size());
}

BOOL HelpDb::EntryExists(const std::string &name) const
{
    return (_nameTable->count(name)==0)?0:1;
}

void HelpDb::GetHelp(const std::string &name,Player* mob)
{
    if (!EntryExists(name)) {
        mob->Message(MSG_ERROR,"That help file doesn't exist.");
    }

    HelpNode* node;
    std::list <UINT> sa;
    std::list<std::string> el;
    std::list<UINT>::iterator it;
    HelpNode *san;

    node=(*_nameTable)[name];

    mob->Write(Capitalize(node->GetName()));
    mob->Write(Repete("-",80));
    mob->Write(node->GetData());
    mob->Write(Repete("-",80));

    node->GetSeeAlso(&sa);
    for (it=sa.begin(); it!=sa.end(); it++) {
        san=(*_idTable)[(*it)];
        el.push_back(san->GetName());
    }

    if (!el.size()) {
        mob->Write("See Also: none.");
    } else {
        mob->Write("See Also: "+EnglishList(&el)+".");
    }
}

void HelpDb::ShowSectionTopics(UINT sid,Player* mob)
{
    if (!_sections->count(sid)) {
        mob->Message(MSG_ERROR,"That topic doesn't exist.");
        return;
    }

    HelpSection* section=NULL;
    HelpNode* node=NULL;
    std::list <std::string> el;
    std::map <UINT,HelpNode*>::iterator it;

    section=(*_sections)[sid];
    mob->Write("Showing topics for "+section->GetName()+".");
    mob->Write(Repete("-",80));

    for (it=_idTable->begin(); it!=_idTable->end(); it++) {
        node=(*it).second;
        if (node->GetSid()==sid) {
            el.push_back(node->GetName());
        }
    }
    mob->Write(EnglishList(&el));
}

void HelpDb::ShowSections(Player* mob)
{
    std::map <UINT,HelpSection*>::iterator it;
    int i=0;
    for (it=_sections->begin(); it!=_sections->end(); it++) {
        mob->Message(MSG_LIST,(*it).second->GetName());
        i+=1;
    }
    mob->Write(Repete("-",80));
}

BOOL HelpDb::Load(const std::string &path)
{
    if (!FileExists(path)) {
        return false;
    }
    FILE* input=fopen(path.c_str(), "rb");
    Serializer*s = new Serializer(input, READ);
    Deserialize(*s);
    delete s;
    return true;
}
void HelpDb::Save(const std::string &path)
{
    FILE* output = fopen(path.c_str(), "wb");
    Serializer*s =new Serializer(output, WRITE);
    Serialize(*s);
    delete s;
}

void HelpDb::Serialize(Serializer& ar)
{
    int size;

    std::map<UINT,HelpSection*>::iterator st;
    std::map<UINT,HelpNode*>::iterator ht;

    size=_sections->size();
    ar << size;
    if (size!=0) {
        for (st=_sections->begin(); st!=_sections->end(); st++) {
            (*st).second->Serialize(ar);
        }
    }
    size=_nameTable->size();
    ar << size;
    if (size!=0) {
        for (ht=_idTable->begin(); ht!=_idTable->end(); ht++) {
            (*ht).second->Serialize(ar);
        }
    }
}
void HelpDb::Deserialize(Serializer& ar)
{
    int size,i;
    std::string value;

    HelpSection* section=NULL;
    HelpNode* node=NULL;

    ar >> size;
    if (size!=0) {
        for (i=0; i<size; i++) {
            section=new HelpSection();
            section->Deserialize(ar);
            (*_sections)[section->GetId()]=section;

            if (_sid<section->GetId()) {
                _sid=section->GetId();
            }
        }
    }

    ar >> size;
    if (size!=0) {
        for (i=0; i<size; i++) {
            node=new HelpNode();
            node->Deserialize(ar);

            (*_nameTable)[node->GetName()]=node;
            (*_idTable)[node->GetId()]=node;

            if (_hid<node->GetId()) {
                _hid=node->GetId();
            }
        }
    }

}
#endif
