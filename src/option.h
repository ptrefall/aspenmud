/*
*option.h
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


/*
*A setup for holding options for players.
*We register each option here, then if the player has something different set we check that.
*This both saves us time and adds for more flexability.
*/
#ifndef OPTION_H
#define OPTION_H
#include <string>
#include <map>
#include "mud.h"
#include "conf.h"
#include "variant.h"

class Option
{
    int _rank;
    Variant _data; //the value.
    VARIABLE_TYPE _type; //the type that the value should hold.
    BOOL _toggle;
    std::string _help; //a brief description of the option.
    std::string _name; //the name of the option.
public:
    Option();
    Option(const std::string &name, const std::string & help, int rank, Variant value, VARIABLE_TYPE type, BOOL toggle=false);
    ~Option();
    template <class T>
    inline T GetValue() const {
        return _data.Get<T>();
    }
    Variant GetData(void) const;
    void SetBool(BOOL val);
    void SetInt(int val);
    void SetString(const std::string &val);
    std::string GetName(void) const;
    std::string GetHelp(void) const;
    int GetRank(void) const;
    BOOL CanToggle() const;
};

BOOL RegisterOption(const std::string &name, const std::string & help, int rank, Variant value, VARIABLE_TYPE type, BOOL toggle = false);
BOOL OptionExists(const std::string &name);
Option* GetGlobalOption(const std::string name);
std::map<std::string, Option>* GetGlobalOptions();
#endif
