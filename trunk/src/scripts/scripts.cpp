#include <string>
#include <sstream>
#include <boost/bind.hpp>
#include "../conf.h"
#include "../mud.h"
#include "../entity.h"
#include "../world.h"
#include "../player.h"
#include "../utils.h"
#include "scripts.h"

#ifdef MODULE_SCRIPTING
#include <angelscript.h>
#include <scriptbuilder.h>
#include <scriptstdstring.h>

Script::Script()
{
  int ret = 0;
  std::string error;

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
      throw(ScriptException("An error occured while trying to register the message callback. Error: "+error+"."));
    }

  RegisterStdString(_engine);
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
  world->WriteLog(st.str(), SCRIPT, "script");
}

BOOL Script::Execute(Entity* object)
{
  if (object->GetScript() == "")
    {
      return true;
    }

  World* world = World::GetPtr();
  int ret = 0;
  CScriptBuilder builder;
  char idstr [11];
  NumberToString(idstr, object->GetOnum());
  ret = builder.StartNewModule(_engine, idstr);
  if (ret < 0)
    {
      world->WriteLog("Could not create script builder.", SCRIPT, "script");
      return false;
    }
  ret = builder.AddSectionFromMemory(object->GetScript().c_str());
  if (ret < 0)
    {
      world->WriteLog("Could not load script from object.", SCRIPT, "script");
      return false;
    }
  ret = builder.BuildModule();
  if (ret < 0)
    {
      world->WriteLog("Compilation error.", SCRIPT, "script");
      return false;
    }

  asIScriptModule *module = _engine->GetModule(idstr);
  int gfid = module->GetFunctionIdByDecl("void main(Entity* obj)");
  if (gfid < 0)
    {
      world->WriteLog("Could not find entrypoint.", SCRIPT, "script");
      return false;
    }

  asIScriptContext *context = _engine->CreateContext();
  if (!context)
    {
      world->WriteLog("Could not create script context", SCRIPT, "script");
      return false;
    }
  context->Prepare(gfid);
  context->SetArgObject(0, object);
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
  return true;
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

  Script* script = new Script();
  world->AddProperty("script", script);
#endif
  return true;
}
