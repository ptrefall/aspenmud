#ifndef SCRIPTS_H
#define SCRIPTS_H
#include "../conf.h"
#include "../mud.h"
#include "../entity.h"

#ifdef MODULE_SCRIPTING
#include <angelscript.h>

class Script
{
  asIScriptEngine *_engine;
  Entity* _obj;
public:
  Script(Entity* object);
};
#endif

BOOL InitializeScript();
#endif
