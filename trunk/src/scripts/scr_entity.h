#ifndef SCR_ENTITY_H
#define SCR_ENTITY_H
#include "../mud.h"
#include "../conf.h"
#include "../player.h"
#include "../entity.h"
#include "../world.h"
#include "../variant.h"
#include "../event.h"
#include "scripts.h"
#include <string>
#include <cstring>

#ifdef MODULE_SCRIPTING
BOOL InitEntityScript(Script* s);
int SCR_GetName(lua_State* l);
int SCR_SetName(lua_State* l);
int SCR_GetDescription(lua_State* l);
int SCR_SetDescription(lua_State* l);
#endif
#endif
