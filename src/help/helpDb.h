/*
*helpDb.h
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

#ifndef HELP_DB_H
#define HELP_DB_H
#include <string>
#include <list>
#include <map>
#include "../mud.h"
#include "../conf.h"
#include "../player.h"
#include "../serializer.hpp"
#include "helpNode.h"
#include "helpSection.h"
#ifdef MODULE_HELP
class HelpDb
{
    std::map <UINT,HelpSection*> *_sections;
    std::map <UINT,HelpNode*> *_idTable;
    std::map <std::string,HelpNode*> *_nameTable;
    UINT _hid;
    UINT _sid;
public:
    HelpDb();
    ~HelpDb();

    HelpNode* GetHelpObj(const std::string &name) const;
    BOOL AddHelpObj(HelpNode* obj);

    UINT GetSectionByName(const std::string &name);
    HelpSection* GetSection(UINT id) const;
    BOOL AddHelpSection(HelpSection* obj);
    BOOL DeleteHelpSection(UINT id);
    int GetSectionCount(void) const;

    BOOL EntryExists(const std::string &name) const;
    void GetHelp(const std::string &name,Player* mob);
    void ShowSectionTopics(UINT sid,Player* mob);
    void ShowSections(Player* mob);

    BOOL Load(const std::string &path);
    void Save(const std::string &path);
    void Serialize(Serializer& ar);
    void Deserialize(Serializer& ar);
};
#endif
#endif
