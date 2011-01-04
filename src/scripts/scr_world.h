/*
*scr_world.h
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


#ifndef SCR_WORLD_H
#include "../mud.h"
#include "../conf.h"
#include "../event.h"
#include "scripts.h"

#ifdef MODULE_SCRIPTING
EVENT(EVENT_INIT_WORLD_SCRIPT);
BOOL InitWorldScript(void);

//prototypes
int SCR_Shutdown(lua_State* l);
int SCR_GetRealUptime(lua_State* l);
int SCR_GetCopyoverUptime(lua_State* l);
int SCR_FindPlayer(lua_State* l);
int SCR_GetPlayers(lua_State* l);
int SCR_GetBanner(lua_State* l);
int SCR_GetMotd(lua_State* l);
int SCR_WriteLog(lua_State* l);
#endif
#endif
