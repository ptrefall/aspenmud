#include <string>
#include <sstream>
#include <sys/time.h>
#include "scripts.h"
#include "scr_world.h"
#include "scr_player.h"
#include "scr_entity.h"
#include "../mud.h"
#include "../conf.h"
#include "../world.h"
#include "../command.h"
#include "../event.h"
#include "../eventargs.h"
#include "../variant.h"

#ifdef MODULE_SCRIPTING
static int traceback (lua_State *L)
{
  if (!lua_isstring(L, 1))  /* 'message' not a string? */
    return 1;  /* keep it intact */
  lua_getfield(L, LUA_GLOBALSINDEX, "debug");
  if (!lua_istable(L, -1))
    {
      lua_pop(L, 1);
      return 1;
    }
  lua_getfield(L, -1, "traceback");
  if (!lua_isfunction(L, -1))
    {
      lua_pop(L, 2);
      return 1;
    }
  lua_pushvalue(L, 1);  /* pass error message */
  lua_pushinteger(L, 2);  /* skip this function and traceback */
  lua_call(L, 2, 1);  /* call debug.traceback */
  return 1;
}

Script::Script()
{
  state = lua_open();
  luaL_openlibs(state);
  OneArg arg(state);
  world->events.CallEvent("ScriptLoaded", &arg, NULL);
  _obj = NULL;
}
Script::~Script()
{
  OneArg arg(state);
  world->events.CallEvent("ScriptUnloaded", &arg, NULL);
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
//we load our code to be executed.
  int status = luaL_loadbuffer(state, code.c_str(), code.length(), "execution");
  if (!status)
    {
//we push our traceback function on to the stack.
      int base = lua_gettop(state);
      lua_pushcfunction(state, traceback);
      lua_insert(state, base);
//we call the code.
      status = lua_pcall(state, 0, 0, base);
      lua_remove(state, base);
//if it failed, we garbage collect.
      if (status)
        {
          lua_gc(state, LUA_GCCOLLECT, 0);
        }
    }
  return NULL;
}
lua_State* Script::GetState() const
{
  return state;
}

EVENT(EVENT_INIT_SCRIPT)
{
  lua_State* lstate = (lua_State*)((OneArg*)args)->_arg;

//open the lua libraries here
  luaL_openlibs(lstate);
}
#endif

BOOL InitializeScript(void)
{
#ifdef MODULE_SCRIPTING
  world->WriteLog("Initializing scripting.");
  world->commands.AddCommand(new CMDExecute());
  if (!InitWorldScript())
    {
      world->WriteLog("Initialization of world script system failed.", ERR);
      return false;
    }
  if (!InitPlayerScript())
    {
      world->WriteLog("Initialization of player script system failed.", ERR);
      return false;
    }
  if (!InitEntityScript())
    {
      world->WriteLog("Initialization of entity script failed.", ERR);
      return false;
    }
#endif
  return true;
}

#ifdef MODULE_SCRIPTING
CMDExecute::CMDExecute()
{
  SetName("execute");
  AddAlias("exec");
  AddAlias("eval");
  SetAccess(RANK_BUILDER);
}
BOOL CMDExecute::Execute(const std::string &verb, Player* mobile,std::vector<std::string> &args,int subcmd)
{
  timeval prev, now; //used for timing the whole thing.
  const char* ret = NULL;
  std::stringstream st; //used for printing results.
  std::vector<std::string>::iterator it, itEnd;
  std::string input;
  int elapsed = 0;
  Script* scr = NULL;

  scr = new Script();
  if (!scr)
    {
      mobile->Message(MSG_INFO, "could not create script interface.");
      return false;
    }

  lua_State* state = scr->GetState();
  gettimeofday(&prev, NULL);
//we set the fd first
  lua_pushnumber(state, mobile->GetSocket()->GetControl());
  lua_setglobal(state, "__fd__");
//now we assign a "me" keyword to the player.
  UserData* data = (UserData*)lua_newuserdata(state, sizeof(UserData));
  data->ptr = (void*)mobile;
  data->type = type_player;
  lua_setglobal(state, "me");
//we push our print replacement to the global table:
  lua_pushcfunction(state, SCR_Print);
  lua_setfield(state, LUA_GLOBALSINDEX, "print");

//expand our arguments into a full string.
  itEnd = args.end();
  for (it = args.begin(); it != itEnd; ++it)
    {
      input += (*it);
    }

//execute the code.
  ret = scr->Execute(input);
  st << "Result: " << (ret == NULL?"no return":ret) << "\n";
  delete scr;

  gettimeofday(&now, NULL);
  elapsed = (now.tv_sec - prev.tv_sec) * 1000;
  elapsed += (now.tv_usec - prev.tv_usec) / 1000;
  st << "[execution took " << elapsed << " ms].";

  mobile->Message(MSG_INFO, st.str());
  return true;
}

void SCR_Error(lua_State* l, const char* msg)
{
  lua_pushlstring(l, msg, strlen(msg));
  lua_error(l);
}

Variant IndexToVariant(lua_State* l, int index)
{
  int type = 0;

  if (lua_isnoneornil(l, index))
    {
      SCR_Error(l, "Specified index is either out of range or nil.");
      return Variant();
    }

  type = lua_type(l, index);
  switch(type)
    {
    default:
      SCR_Error(l, "Can not convert the type at the specified index.");
      return Variant();
    case LUA_TNUMBER:
      return Variant(lua_tonumber(l, index));
    case LUA_TBOOLEAN:
      return lua_toboolean(l, index);
    case LUA_TSTRING:
      return Variant(lua_tostring(l, index));
    }
}
BOOL VariantToStack(lua_State* l, Variant& var)
{
  VARIABLE_TYPE type = var.Typeof();

  switch(type)
    {
    case VAR_INT:
      lua_pushinteger(l, var.GetInt());
      return true;
    case VAR_DOUBLE:
      lua_pushnumber(l, var.GetDouble());
      return true;
    case VAR_BYTE:
      lua_pushinteger(l, (int)var.GetByte());
      return true;
    case VAR_STR:
      lua_pushstring(l, var.GetStr().c_str());
      return true;
    default:
      SCR_Error(l, "Invalid variable type handled when pushing a variant to the stack.");
      return false;
    }
}

BOOL IsPlayer(lua_State* l, UserData* udata)
{
  if (!udata)
    {
      SCR_Error(l, "Udata passed to IsPlayer was NULL.");
      return false;
    }

  if ((udata->type != type_player) || (udata->ptr == NULL))
    {
      SCR_Error(l, "Invalid type.");
      return false;
    }

  return true;
}
BOOL IsLiving(lua_State* l, UserData* udata)
{
  if ((udata->type != type_player) || (udata->type != type_npc) || (udata->ptr == NULL))
    {
      SCR_Error(l, "Invalid type.");
      return false;
    }

  return true;
}
BOOL IsObject(lua_State* l, UserData* udata)
{
  if ((udata->type != type_player) || (udata->type != type_npc) || (udata->type != type_object) || (udata->ptr == NULL))
    {
      SCR_Error(l, "Invalid type.");
      return false;
    }

  return true;
}
int SCR_Print(lua_State* l)
{
  std::string data;
  int nargs = lua_gettop(l);
  int i = 0;
  int fd = 0;
  const char* result = NULL;

  lua_getglobal(l, "tostring");
  for (i = 1; i <= nargs; i++)
    {
      lua_pushvalue(l, -1);
      lua_pushvalue(l, i);
      lua_call(l, 1, 1);
      result = lua_tostring(l, -1);
      if (result == NULL)
        {
          SCR_Error(l, "Error in print function--NULL was returned from tostring.");
          return 0;
        }

      if (i > 1)
        data += "    ";
      data += result;
      lua_pop(l, 1);
    }
  data += "\n";
  lua_getglobal(l, "__fd__");
  fd = lua_tonumber(l, -1);
  write(fd, data.c_str(), data.length());
  return 0;
}
#endif
