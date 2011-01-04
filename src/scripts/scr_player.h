/*
*scr_player.h
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


#ifndef SCR_PLAYER_H
#define SCR_PLAYER_H
#include "../mud.h"
#include "../conf.h"
#include "../player.h"
#include "../event.h"
#include "scripts.h"

#ifdef MODULE_SCRIPTING
EVENT(EVENT_INIT_PLAYER_SCRIPT);
BOOL InitPlayerScript(void);

#ifndef SCRIPT_SECURE
int SCR_GetPassword(lua_State* l);
int SCR_SetPassword(lua_State *l);
int SCR_SetRank(lua_State* l);
int SCR_SetOnlineTime(lua_State* l);
int SCR_SetFirstLogin(lua_State* l);
int SCR_SetLastLogin(lua_State* l);
#endif

int SCR_GetTitle(lua_State* l);
int SCR_SetTitle(lua_State*l);
int SCR_GetLevel(lua_State* l);
int SCR_SetLevel(lua_State* l);
int SCR_GetRank(lua_State* l);
int SCR_GetOnlineTime(lua_State* l);
int SCR_GetFirstLogin(lua_State* l);
int SCR_GetLastLogin(lua_State* l);
int SCR_GetPrompt(lua_State* l);
int SCR_SetPrompt(lua_State* l);

int SCR_Save(lua_State* l);
int SCR_Write(lua_State* l);
int SCR_Message(lua_State* l);

#endif
#endif