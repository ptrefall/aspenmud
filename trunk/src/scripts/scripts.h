#ifndef SCRIPTS_H
#define SCRIPTS_H
#include "../conf.h"
#include "../mud.h"
#include "../entity.h"
#include "../exception.h"
#include "../event.h"
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
  Entity* _obj;
public:
  Script(Entity* object);
  ~Script();
  void ReceiveMessage(asSMessageInfo *message);
  EVENT(OnLoad);
};
#endif

BOOL InitializeScript();
#endif
