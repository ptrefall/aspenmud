/*
*option.cpp
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
#include <map>
#include "option.h"
#include "variant.h"

Option::Option()
{
    _rank = RANK_PLAYER;
    _data = Variant();
    _type = VAR_EMPTY;
    _toggle = false;
    _help = "";
    _name = "";
}
Option::Option(const std::string &name, const std::string & help, int rank, Variant value, VARIABLE_TYPE type, BOOL toggle)
{
    _name = name;
    _help = help;
    _rank = rank;
    _type = type;
    _data = value;
    _toggle = toggle;
}
Option::~Option()
{
}

Variant Option::GetData(void) const
{
    return _data;
}
void Option::SetBool(BOOL val)
{
    _data.SetInt((val==true?1:0));
}
void Option::SetInt(int val)
{
    _data.SetInt(val);
}
void Option::SetString(const std::string &val)
{
    _data.SetStr(val);
}

std::string Option::GetName(void) const
{
    return _name;
}
std::string Option::GetHelp(void) const
{
    return _help;
}
int Option::GetRank(void) const
{
    return _rank;
}
BOOL Option::CanToggle() const
{
    return _toggle;
}

static std::map<std::string, Option> options;

BOOL RegisterOption(const std::string &name, const std::string & help, int rank, Variant value, VARIABLE_TYPE type, BOOL toggle)
{
    if (options.count(name) == 0) {
        options[name] = Option(name, help, rank, value, type);
        return true;
    }

    return false;
}
BOOL OptionExists(const std::string &name)
{
    return (options.count(name)==0?false:true);
}
Option* GetGlobalOption(const std::string &name)
{
    if (options.count(name) == 0) {
        return NULL;
    }

    return &(options[name]);
}
std::map<std::string, Option>* GetGlobalOptions()
{
    return &options;
}
