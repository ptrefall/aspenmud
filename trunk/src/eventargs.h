#ifndef EVENT_ARGS_H
#define EVENT_ARGS_H
#include "mud.h"
#include "conf.h"
#include "entity.h"
/*
*LookArgs
*Called when a player looks at an object.
*Args: Caller (The player's object), and targ (The object being looked at).
*/
class LookArgs:public EventArgs
{
public:
  LookArgs(Player* caller, Entity* targ,std::string &desc):_caller(caller),_targ(targ),_desc(desc) {}
  ~LookArgs(void) {}

  Player* _caller;
  Entity* _targ;
  std::string &_desc;
};

class OneArg:public EventArgs
{
public:
  OneArg(void* arg):_arg(arg) { }
  void* _arg;
};

//this breaks our general style, but there's no need to declare this or include anything
//if scripts aren't enabled anyway, so we'll just move on with breaking the style, shall we?
#ifdef MODULE_SCRIPTING
class ScriptUnloadedArg:public EventArgs
{
public:
  ScriptUnloadedArg(void* arg):_arg(arg) { }
  void* _arg;
};

class ScriptLoadedArg:public EventArgs
{
public:
  ScriptLoadedArg(void* arg, Entity* obj):_arg(arg), _obj(obj) { }
  void* _arg;
  Entity* _obj;
};
#endif
#endif
