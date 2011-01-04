/*
*helpNode.cpp
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
#include "helpNode.h"
#include "../serializer.hpp"

#ifdef MODULE_HELP
HelpNode::HelpNode()
{
    _rank=RANK_PLAYER;
}

HelpNode::~HelpNode()
{
}

void HelpNode::SetId(UINT id)
{
    _id=id;
}
UINT HelpNode::GetId(void) const
{
    return _id;
}

void HelpNode::SetSid(UINT sid)
{
    _sid=sid;
}
UINT HelpNode::GetSid(void) const
{
    return _sid;
}

void HelpNode::SetName(const std::string &name)
{
    _name=name;
}
std::string HelpNode::GetName(void) const
{
    return _name;
}

void HelpNode::SetData(const std::string &data)
{
    _data=data;
}
std::string HelpNode::GetData(void) const
{
    return _data;
}

void HelpNode::SetRank(FLAG rank)
{
    _rank=rank;
}
FLAG HelpNode::GetRank(void) const
{
    return _rank;
}

BOOL HelpNode::AddSeeAlso(UINT id)
{
    std::list<UINT>::iterator it;

    for (it=_seeAlso.begin(); it!=_seeAlso.end(); it++) {
        if ((*it)==id) {
            return false;
        }
    }

    _seeAlso.push_back(id);
    return true;
}

void HelpNode::GetSeeAlso(std::list <UINT>* out)
{
    std::list<UINT>::iterator it;

    out->clear();
    for (it=_seeAlso.begin(); it!=_seeAlso.end(); it++) {
        out->push_back((*it));
    }
}

void HelpNode::Serialize(Serializer& ar)
{
    std::list<UINT>::iterator it;
    int size;
    unsigned int tmp;
    tmp=_id;
    ar << _id;
    ar << _sid;
    ar << _name;
    ar << _data;
    ar << _rank;
    size=_seeAlso.size();
    ar << size;

    for (it=_seeAlso.begin(); it!=_seeAlso.end(); it++) {
        ar << (*it);
    }
}
void HelpNode::Deserialize(Serializer& ar)
{
    int i,size;
    UINT tmp;

    ar >> _id;
    ar >> _sid;
    ar >> _name;
    ar >> _data;
    ar >> _rank;
    ar >> size;
    for (i=0; i<size; i++) {
        ar >> tmp;
        _seeAlso.push_back(tmp);
    }
}
#endif
