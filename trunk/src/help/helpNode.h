/*
*helpNode.h
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

#ifndef HELP_NODE_H
#define HELP_NODE_H
#include <list>
#include <map>
#include "../mud.h"
#include "../conf.h"
#include "../serializer.hpp"

#ifdef MODULE_HELP
class HelpNode
{
    UINT _id; //the id of the help file.
    UINT _sid; //the section ID this entry belongs to.
    std::string _name; //the name of the help file.
    std::string _data; //the data
    FLAG _rank; //minimal rank aloud to access this file.
    std::list <UINT> _seeAlso; //a list of ids to other helpfiles.
public:
    HelpNode(const std::string &name,const std::string &data):_name(name),_data(data) {
        _rank=RANK_PLAYER;
    }
    HelpNode(void);
    ~HelpNode();
    /*
    *Getters and setters.
    *These either take the value to set (setter), or return the value (getter).
    */
    void SetId(UINT id);
    UINT GetId(void) const;
    void SetSid(UINT sid);
    UINT GetSid(void) const;
    void SetName(const std::string &name);
    std::string GetName(void) const;
    void SetData(const std::string &data);
    std::string GetData(void) const;
    void SetRank(FLAG rank);
    FLAG GetRank(void) const;
    /*
    *Adds a See Also reference to the list.
    *Param: [in] the ID to add.
    */
    BOOL AddSeeAlso(UINT id);
    /*
    *Retrieves a list of see also references.
    *Param: [out] a pointer to a list where the references can be stored.
    */
    void GetSeeAlso(std::list<UINT>* out);

//serialization
    void Serialize(Serializer& ar);
    void Deserialize(Serializer& ar);
};
#endif
#endif
