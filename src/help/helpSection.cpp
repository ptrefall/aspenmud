/*
*helpSection.cpp
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

#include "helpSection.h"
#include "../serializer.hpp"

#ifdef MODULE_HELP
HelpSection::HelpSection(const std::string &name):_name(name)
{
}
HelpSection::HelpSection(void)
{
}

HelpSection::~HelpSection(void)
{
}

UINT HelpSection::GetId(void) const
{
    return _id;
}
void HelpSection::SetId(UINT id)
{
    _id=id;
}

std::string HelpSection::GetName(void) const
{
    return _name;
}
void HelpSection::SetName(const std::string &name)
{
    _name=name;
}

void HelpSection::Serialize(Serializer& ar)
{
    ar << _id;
    ar << _name;
}
void HelpSection::Deserialize(Serializer& ar)
{
    ar >> _id;
    ar >> _name;
}
#endif
