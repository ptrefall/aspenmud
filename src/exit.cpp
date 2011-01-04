/*
*exit.cpp
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
#include "exit.h"
#include "living.h"
#include "serializer.hpp"

Exit::Exit(VNUM from, VNUM to)
{
    _to=to;
    _from=from;
}
Exit::Exit(void)
{
    _from=EXIT_NOWHERE;
    _to=EXIT_NOWHERE;
}
Exit::~Exit(void)
{
}

VNUM Exit::GetFrom(void) const
{
    return _from;
}
void Exit::SetFrom(VNUM from)
{
    _from=from;
}

VNUM Exit::GetTo(void) const
{
    return _to;
}
void Exit::SetTo(VNUM to)
{
    _to=to;
}

std::string Exit::GetName(void) const
{
    return _name;
}
void Exit::SetName(const std::string &name)
{
    _name=name;
}

std::string Exit::GetAlias(void) const
{
    return _alias;
}
void Exit::SetAlias(const std::string &alias)
{
    _alias=alias;
}

BOOL Exit::CanEnter(Living* mobile)
{
    return true;
}

void Exit::Serialize(Serializer& ar)
{
    ar << _to;
    ar << _from;
    ar << _name;
    ar << _alias;
}
void Exit::Deserialize(Serializer& ar)
{
    ar >> _to;
    ar >> _from;
    ar >> _name;
    ar >> _alias;
}
