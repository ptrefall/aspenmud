#include <string>
#include <sstream>
#include <boost/bind.hpp>
#include "../conf.h"
#include "../mud.h"
#include "../entity.h"
#include "../world.h"
#include "../player.h"
#include "../event.h"
#include "scripts.h"

#ifdef MODULE_SCRIPTING
#include <angelscript.h>
#include "scriptbuilder/scriptbuilder.h"
#include "scriptstdstring/scriptstdstring.h"

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

  RegisterStdString(_engine);
  _obj->events.AddCallback("loaded", boost::bind(&Script::OnLoad, this, _1, _2));
}
Script::~Script()
{
  _engine->Release();
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
          world->WriteLog(st.str(), SCRIPT, "script");
        }
    }
  else
    {
      world->WriteLog(st.str(), SCRIPT, "script");
    }
}

CEVENT(Script, OnLoad)
{
  World* world = World::GetPtr();
  int ret = 0;
  CScriptBuilder builder;
  ret = builder.StartNewModule(_engine, "global");
  if (ret < 0)
    {
      world->WriteLog("Could not create script builder.", SCRIPT, "script");
      return;
    }
  ret = builder.AddSectionFromMemory(_obj->GetScript().c_str());
  if (ret < 0)
    {
      world->WriteLog("Could not load script from object.", SCRIPT, "script");
      return;
    }
  ret = builder.BuildModule();
  if (ret < 0)
    {
      world->WriteLog("Compilation error.", SCRIPT, "script");
      return;
    }

  asIScriptModule *module = _engine->GetModule("global");
  int gfid = module->GetFunctionIdByDecl("void main()");
  if (gfid < 0)
    {
      world->WriteLog("Could not find entrypoint.", SCRIPT, "script");
      return;
    }

  asIScriptContext *context = _engine->CreateContext();
  if (!context)
    {
      world->WriteLog("Could not create script context", SCRIPT, "script");
      return;
    }
  context->Prepare(gfid);

  ret = context->Execute();
  switch(ret)
    {
    default:
      world->WriteLog("Received an unknown return code from Execute.");
      break;
    case asEXECUTION_ABORTED:
    case asEXECUTION_SUSPENDED:
    case asEXECUTION_FINISHED:
      break;
    case asEXECUTION_EXCEPTION:
      world->WriteLog("Script exited with exception: "+std::string(context->GetExceptionString()), SCRIPT, "script");
      break;
    }

  context->Release();
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
