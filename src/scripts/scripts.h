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
#include "../variant.h"

#ifdef MODULE_SCRIPTING
extern "C"
{
#include <lua5.1/lua.h>
#include <lua5.1/lauxlib.h>
#include <lua5.1/lualib.h>
#include <lua5.1/luaconf.h>
}

//Basic userdata class
//used to pass in the type of class we're working with.
enum UDATA_TYPE {
  type_none, type_server, type_world, type_object, type_player, type_npc
};
struct UserData
{
  UDATA_TYPE type;
  void* ptr;
};

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

/*
*Auxiliary functions
*/
/*
*Helps with printing an error message, when a problem is encountered.
*Param: [in] the lua state that the error belongs to.
*Param: [in] the error message.
*/
void SCR_Error(lua_State* l, const char* msg);
/*
*converts a value on the stack to a variant.
*Param: [in] the lua state where the index will be pulled from.
*Param: [in] the index on the Lua stack where the variant should be converted from.
*Return: The variant with the value at the given index, or an empty Variant if there was a problem.
*/
Variant IndexToVariant(lua_State*l, int index);
/*
*Converts a given Variant to a value on the stack.
*Param: [in] the state of which the stack belongs that we will put the variant on.
*Param: [in] the variant we will push to the stack.
*Return: True on success, false on failure.
*/
BOOL VariantToStack(lua_State* l, Variant &var);
/*
*checks to see if the type of the udata is a player.
*Param: [in] the lua state for the operation.
*Param: [in] the UserData object to check.
*Return: True if the object is a player, false otherwise.
*/
BOOL IsPlayer(lua_State* l, UserData* udata);
/*
*Checks to see if the provided object is living.
*Param: [in] the active lua state.
*Param: [in] the UserData struct.
*/
BOOL IsLiving(lua_State* l, UserData* udata);
/*
*Checks to see if the specified object is of type Entity
*This is slightly dificult because everything in game that is actually in game is an entity, but we need multiple checks since
*lua requires us to have a separation.
*Param: [in] the currently active lua state.
*Param: [in] The object to check.
*/
BOOL IsObject(lua_State* l, UserData* udata);
/*
*Used as a drop-in replacement for print.
*/
int SCR_Print(lua_State* l);
#endif
#endif
