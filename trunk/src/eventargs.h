/*
*eventargs.h
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


#ifndef EVENT_ARGS_H
#define EVENT_ARGS_H
#include "mud.h"
#include "conf.h"
#include "entity.h"
/*
*LookArgs
*Called when a player looks at an object.
*Args: Caller (The player's object), and targ (The object being looked at).
*/
class LookArgs:public EventArgs
{
public:
    LookArgs(Player* caller, Entity* targ,std::string &desc):_caller(caller),_targ(targ),_desc(desc) {}
    ~LookArgs(void) {}

    Player* _caller;
    Entity* _targ;
    std::string &_desc;
};

class OneArg:public EventArgs
{
public:
    OneArg(void* arg):_arg(arg) { }
    void* _arg;
};

//this breaks our general style, but there's no need to declare this or include anything
//if scripts aren't enabled anyway, so we'll just move on with breaking the style, shall we?
#ifdef MODULE_SCRIPTING
class ScriptUnloadedArg:public EventArgs
{
public:
    ScriptUnloadedArg(void* arg):_arg(arg) { }
    void* _arg;
};

class ScriptLoadedArg:public EventArgs
{
public:
    ScriptLoadedArg(void* arg, Entity* obj):_arg(arg), _obj(obj) { }
    void* _arg;
    Entity* _obj;
};
#endif
#endif
