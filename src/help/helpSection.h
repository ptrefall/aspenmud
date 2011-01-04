/*
*helpSection.h
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

#ifndef HELP_SECTION
#define HELP_SECTION
#include "../mud.h"
#include "../conf.h"
#include "../serializer.hpp"

#ifdef MODULE_HELP
class HelpSection
{
    UINT _id;
    std::string _name;
public:
    HelpSection(const std::string &name);
    HelpSection(void);
    ~HelpSection(void);

    UINT GetId(void) const;
    void SetId(UINT id);
    std::string GetName(void) const;
    void SetName(const std::string &name);

    void Serialize(Serializer& ar);
    void Deserialize(Serializer& ar);
};

#endif
#endif
