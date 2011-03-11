/*
*The main scripts header.
*Initializes other script components.
*/

#ifndef SCRIPTS_H
#define SCRIPTS_H
#include "../mud.h"
#include "../conf.h"
#include "../command.h"
#include "../player.h"
#include "../server.h"
#include "../socket.h"

#ifdef MODULE_SCRIPTING
extern "C"
{
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <luaconf.h>
}

//Basic userdata class
//used to pass in the type of class we're working with.
enum UDATA_TYPE {
  type_none, type_server, type_world, type_player
};
typedef struct
{
  UDATA_TYPE type;
  void* ptr;
} UserData;

class Script
{
  lua_State *state;
  Entity* _obj;
  std::string _script;
public:
  Script();
  ~Script();
  void SetObj(Entity* obj);
  Entity* GetObj(void) const;
  const char* Execute(const std::string &code);
  lua_State* GetState() const;
};

EVENT(EVENT_INIT_PLAYER_SCRIPT);
#endif

BOOL InitializeScript(void);
#ifdef MODULE_SCRIPTING
class CMDExecute:public Command
{
public:
  CMDExecute();
  BOOL Execute(const std::string &verb, Player* mobile,std::vector<std::string> &args,int subcmd);
};
void SCR_Error(lua_State* l, const char* msg);
#endif

#endif
