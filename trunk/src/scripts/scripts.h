#ifndef SCRIPTS_H
#define SCRIPTS_H
#include "../conf.h"
#include "../mud.h"
#include "../entity.h"
#include "../exception.h"
#include "../event.h"
#include "../world.h"
#ifdef MODULE_SCRIPTING
#include <angelscript.h>
//script-wide exceptions:
class ScriptException:public Exception
{
public:
  ScriptException(const std::string &msg):Exception(msg) {}
};

class Script
{
  asIScriptEngine *_engine;
public:
  Script();
  ~Script();
  void ReceiveMessage(asSMessageInfo *message);
  void RegisterEntity();
  void RegisterWorld();
#ifdef OLC
  static EVENT(AddOlc);
#endif
  BOOL Execute(Entity* object);
};

World* GetWorldPointer();
#endif

BOOL InitializeScript();
#endif
