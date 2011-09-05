#include "scr_entity.h"

#ifdef MODULE_SCRIPTING
static const struct luaL_reg entity_table [] =
{
  {"GetName", SCR_GetName},
  {"SetName", SCR_SetName},
  {"GetDescription", SCR_GetDescription},
  {"SetDescription", SCR_SetDescription},
//{"GetPlural", SCR_GetPlural},
//{"SetPlural", SCR_SetPlural},
//{"GetScript", SCR_GetScript},
//{"SetScript", SCR_SetScript},
//{"GetLocation", SCR_GetLocation},
//{"SetLocation", SCR_SetLocation},
//{"GetContents", SCR_GetContents},
  {NULL, NULL}
};

BOOL InitEntityScript(Script* s)
{
  lua_State* lstate = s->GetState();
  luaL_newmetatable(lstate, "entity");
  lua_pushstring(lstate, "__index");
  lua_pushvalue(lstate, -2);
  lua_settable(lstate, -3);
  luaL_openlib(lstate, "entity", entity_table, 0);
  return true;
}

//our script functions:
int SCR_GetName(lua_State* l)
{
  UserData* udata = NULL;
  std::string name;

  if (lua_gettop(l) != 1)
    {
      SCR_Error(l, "Invalid number of arguments to \'GetName\'.");
      return 0;
    }

  udata = (UserData*)lua_touserdata(l, -1);
  if (!udata)
    {
      SCR_Error(l, "Argument 1 to 'GetName' must be an object.");
      return 0;
    }

  name = ((Entity*)udata->ptr)->GetName();
  lua_pushlstring(l, name.c_str(), name.length());
  return 1;
}
int SCR_SetName(lua_State* l)
{
  UserData* udata = NULL;
  const char* name = NULL;

  if (lua_gettop(l) != 2)
    {
      SCR_Error(l, "Invalid number of arguments to \'SetName\'.");
      return 0;
    }

  name = lua_tostring(l, -1);
  if (!name)
    {
      SCR_Error(l, "Argument 2 to \'SetName\' must be a string.");
      return 0;
    }

  udata = (UserData*)lua_touserdata(l, -2);
  if (!udata)
    {
      SCR_Error(l, "Argument 1 to 'SetName' must be an object.");
      return 0;
    }

  ((Entity*)udata->ptr)->SetName(name);
  return 0;
}

int SCR_GetDescription(lua_State *l)
{
  UserData* udata = NULL;
  std::string description;

  if (lua_gettop(l) != 1)
    {
      SCR_Error(l, "Invalid number of arguments to \'GetDescription\'.");
      return 0;
    }

  udata = (UserData*)lua_touserdata(l, -1);
  if (!udata)
    {
      SCR_Error(l, "Argument 1 to 'GetDescription' must be an object.");
      return 0;
    }

  description = ((Entity*)udata->ptr)->GetDescription();
  lua_pushlstring(l, description.c_str(), description.length());
  return 1;
}
int SCR_SetDescription(lua_State* l)
{
  UserData* udata = NULL;
  const char* description = NULL;

  if (lua_gettop(l) != 2)
    {
      SCR_Error(l, "Invalid number of arguments to \'SetDescription\'.");
      return 0;
    }

  description = lua_tostring(l, -1);
  if (!description)
    {
      SCR_Error(l, "Argument 2 to \'SetDescription\' must be a string.");
      return 0;
    }

  udata = (UserData*)lua_touserdata(l, -2);
  if (!udata)
    {
      SCR_Error(l, "Argument 1 to 'SetDescription' must be an object.");
      return 0;
    }

  ((Entity*)udata->ptr)->SetDescription(description);
  return 0;
}
#endif
