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
class ComponentAttachedArgs:public EventArgs
{
public:
  ComponentAttachedArgs(Entity* _obj):obj(_obj) {}
  Entity* obj;
};

class Editor;
class EditorSavedArgs:public EventArgs
{
public:
  EditorSavedArgs(Editor* ed):editor(ed) {}
  EditorSavedArgs();
  Editor* editor;
};
class EditorLoadedArgs:public EventArgs
{
public:
  EditorLoadedArgs(Editor* ed):editor(ed) {}
  EditorLoadedArgs() {}
  Editor* editor;
};
class EditorExitedArgs:public EventArgs
{
public:
  EditorExitedArgs(Editor* ed):editor(ed) {}
  EditorExitedArgs() {}
  Editor* editor;
};
#endif
