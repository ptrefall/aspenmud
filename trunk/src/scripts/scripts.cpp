#include <angelscript.h>
#include <string>
#include "../conf.h"
#include "../mud.h"
#include "../entity.h"
#include "../world.h"
#include "scripts.h"

#ifdef MODULE_SCRIPTING
Script::Script(Entity* obj)
{
  _obj = obj;
}

BOOL InitializeScript()
{
  return true;
}
#endif
