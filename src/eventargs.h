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
#endif
