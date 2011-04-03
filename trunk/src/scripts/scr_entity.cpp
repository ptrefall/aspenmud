#include "scr_entity.h"

#ifdef MODULE_SCRIPTING
static const struct luaL_reg entity_table [] =
{
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
#endif
