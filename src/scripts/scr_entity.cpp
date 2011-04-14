#include "scr_entity.h"

#ifdef MODULE_SCRIPTING
static const struct luaL_reg entity_table [] =
{
  {"GetName", SCR_GetName},
//{"SetName", SCR_SetName},
//{"GetDescription", SCR_GetDescription},
//{"SetDescription", SCR_SetDescription},
//{"GetPlural", SCR_GetPlural},
//{"SetPlural", SCR_SetPlural},
//{"GetScript", SCR_GetScript},
//{"SetScript", SCR_SetScript},
//{"GetLocation", SCR_GetLocation},
//{"SetLocation", SCR_SetLocation},
//{"GetContents", SCR_GetContents},
  {NULL, NULL}
};

EVENT(EVENT_INIT_ENTITY_SCRIPT)
{
  lua_State* lstate = (lua_State*)((OneArg*)args)->_arg;
  luaL_newmetatable(lstate, "aspen.entity");
  lua_pushstring(lstate, "__index");
  lua_pushvalue(lstate, -2);
  lua_settable(lstate, -3);
  luaL_openlib(lstate, "entity", entity_table, 0);
}
BOOL InitEntityScript()
{
  world->events.AddCallback("ScriptLoaded", EVENT_INIT_ENTITY_SCRIPT);
  return true;
}

//our script functions:
int SCR_GetName(lua_State* l)
{
  UserData* udata = NULL;
  std::string name;

  if (lua_gettop(l) != 1)
    {
      SCR_Error(l, "Invalid number of arguments to \'GetPassword\'.");
      return 0;
    }
  if (!IsObject(l, udata))
    {
      SCR_Error(l, "First argument to 'GetName' must be an object.");
      return 0;
    }

  name = ((Entity*)udata->ptr)->GetName();
  lua_pushlstring(l, name.c_str(), name.length());
  return 1;
}
#endif
