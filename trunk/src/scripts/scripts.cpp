#include <string>
#include <sstream>
#include "../conf.h"
#include "../mud.h"
#include "../entity.h"
#include "../world.h"
#include "../player.h"
#include "scripts.h"

#ifdef MODULE_SCRIPTING
#include <angelscript.h>

Script::Script(Entity* obj)
{
  int ret = 0;
  std::string error;

  _obj = obj;
  _engine = asCreateScriptEngine(ANGELSCRIPT_VERSION);
  ret = _engine->SetMessageCallback(asMETHOD(Script, ReceiveMessage), this, asCALL_THISCALL);
  if (ret != 0)
    {
      switch(ret)
        {
        default:
          error = "unknown";
          break;
        case asINVALID_ARG:
          error = "invalid Argument";
          break;
        case asNOT_SUPPORTED:
          error = "argument not supported";
          break;
        }
      throw(ScriptException("An error occured while trying to register the message callback for "+_obj->GetName()+". Error: "+error+"."));
    }
}
Script::~Script()
{
}

void Script::ReceiveMessage(asSMessageInfo *message)
{
  World* world = World::GetPtr();

//we need to create our string:
  std::stringstream st;
  switch(message->type)
    {
    case asMSGTYPE_ERROR:
      st << "[ERROR] ";
      break;
    case asMSGTYPE_WARNING:
      st << "[WARNING] ";
      break;
    case asMSGTYPE_INFORMATION:
      st << "[INFO] ";
      break;
    default:
      st << "[UNKNOWN] ";
    }
  st << "line " << message->row << "col " << message->col << "section " << message->section << ": " << message->message;
//now we need to see if the object is a player, and if so if it is a builder or higher. If not, we need to pass the message to the logger.
  if (_obj->IsPlayer())
    {
      if (((Player*)_obj)->HasAccess(RANK_BUILDER))
        {
          ((Player*)_obj)->Message(MSG_INFO, st.str());
          return;
        }
      else
        {
          goto logit;
        }
    }
  else
    {
      goto logit;
    }

logit:
  world->WriteLog(st.str(), SCRIPT, "script");
}
#endif

BOOL InitializeScript()
{
#ifdef MODULE_SCRIPTING
  World* world = World::GetPtr();
  if (!world->RegisterLog(SCRIPT_FILE, SCRIPT_NAME))
    {
      world->WriteLog("Something seems to have registered a script log already. Writing there anyway.", WARN);
    }
#endif
  return true;
}
