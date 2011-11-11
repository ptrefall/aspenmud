#include "scr_events.h"

#ifdef MODULE_SCRIPTING
static const struct luaL_reg event_table [] =
{
  {"AddCallback", SCR_AddCallback},
//{"CallEvent", SCR_CallEvent},
  {NULL, NULL}
};

BOOL InitEventScript(Script* s)
{
  lua_State* lstate = s->GetState();
  luaL_newmetatable(lstate, "events");
  lua_pushstring(lstate, "__index");
  lua_pushvalue(lstate, -2);
  lua_settable(lstate, -3);
  luaL_openlib(lstate, "events", event_table, 0);
  return true;
}

int SCR_AddCallback(lua_State* l)
{
  UserData* udata = NULL;
  const char* func = NULL;
  const char* event = NULL;

  if (lua_gettop(l) != 3)
    {
      SCR_Error(l, "Invalid number of arguments to 'AddCallback'");
      return 0;
    }
  func = lua_tostring(l, -1);
  if (!func)
    {
      SCR_Error(l, "Argument 3 to 'AddCallback' must be the name of the function to call.");
      return 0;
    }

  event = lua_tostring(l, -2);
  if (!event)
    {
      SCR_Error(l, "Argument 2 to 'AddCallback' must be the name of the event to add a callback to.");
      return  0;
    }

  udata = (UserData*)lua_touserdata(l, -3);
  if (!IsObject(l, udata))
    {
      SCR_Error(l, "Argument 1 to 'AddCallback' must be the object to add the callback to.");
      return 0;
    }

  (((Entity*)udata->ptr)->events).AddScriptCallback(l, event, func);
  return 0;
}
BOOL SCR_CallEvent(lua_State* l, const char* func, EventArgs* args, void* caller)
{
  EventArgsUserData* udata = NULL;

//we get the function we need to call.
  lua_getfield(l, LUA_GLOBALSINDEX, func);
//we create the event args user data and push it.
  udata = (EventArgsUserData*)lua_newuserdata(l, sizeof(EventArgsUserData));
  udata->args = args;
  lua_call(l, 1, 0);
  return true;
}
#endif
