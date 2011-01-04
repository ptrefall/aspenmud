/*
*scripts.cpp
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
#include <sstream>
#include "scripts.h"
#include "../mud.h"
#include "../conf.h"
#include "../world.h"
#include "../command.h"
#include "../event.h"
#include "../eventargs.h"
#include "scr_world.h"
#include "scr_player.h"

#ifdef MODULE_SCRIPTING
Script::Script()
{
    state = lua_open();
    luaL_openlibs(state);
    OneArg arg(state);
    world->CallEvent("ScriptLoaded", &arg, NULL);
    _obj = NULL;
}
Script::~Script()
{
    OneArg arg(state);
    world->CallEvent("ScriptUnloaded", &arg, NULL);
    lua_close(state);
}
void Script::SetObj(Entity* obj)
{
    _obj = obj;
}
Entity* Script::GetObj(void) const
{
    return _obj;
}
const char* Script::Execute(const std::string &code)
{
    if (luaL_dostring(state, code.c_str()) == 0) {
        return lua_tostring(state, -1);
    } else {
        return NULL;
    }
}
EVENT(EVENT_INIT_SCRIPT)
{
    lua_State* lstate = (lua_State*)((OneArg*)args)->_arg;

//open the lua libraries here
    luaL_openlibs(lstate);
}

Script *globl;
#endif

BOOL InitializeScript(void)
{
#ifdef MODULE_SCRIPTING
    world->WriteLog("Initializing scripting.");
    world->AddCallback("ScriptLoaded", EVENT_INIT_PLAYER_SCRIPT);
    world->AddCommand(new CMDExecute());
    if (!InitWorldScript()) {
        world->WriteLog("Initialization of world script system failed.", ERR);
        return false;
    }
    if (!InitPlayerScript()) {
        world->WriteLog("Initialization of player script system failed.", ERR);
        return false;
    }
    globl = new Script();
#endif
    return true;
}

#ifdef MODULE_SCRIPTING
CMDExecute::CMDExecute()
{
    SetName("execute");
    AddAlias("exec");
    SetAccess(RANK_BUILDER);
}
BOOL CMDExecute::Execute(const std::string &verb, Player* mobile,std::vector<std::string> &args,int subcmd)
{
    std::vector<std::string>::iterator it;
    std::string input;
    for (it = args.begin(); it != args.end(); it++) {
        input += (*it);
    }
    const char* ret = NULL;
    ret = globl->Execute(input);
    std::stringstream st;
    st << "Result: " << (ret == NULL?"no return":ret) << "\n";
    mobile->Message(MSG_INFO, st.str());
    return true;
}

void SCR_Error(lua_State* l, const char* msg)
{
    lua_pushlstring(l, msg, strlen(msg));
    lua_error(l);
}

#endif
