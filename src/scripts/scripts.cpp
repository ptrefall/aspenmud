#include <string>
#include <sstream>
#include <sys/time.h>
#include "scripts.h"
#include "../mud.h"
#include "../conf.h"
#include "../world.h"
#include "../command.h"
#include "../event.h"
#include "../eventargs.h"
#include "scr_world.h"
#include "scr_player.h"

#ifdef MODULE_SCRIPTING
Script::Script()
{
    state = lua_open();
    luaL_openlibs(state);
    OneArg arg(state);
    world->events.CallEvent("ScriptLoaded", &arg, NULL);
    _obj = NULL;
}
Script::~Script()
{
    OneArg arg(state);
    world->events.CallEvent("ScriptUnloaded", &arg, NULL);
    lua_close(state);
}
void Script::SetObj(Entity* obj)
{
    _obj = obj;
}
Entity* Script::GetObj(void) const
{
    return _obj;
}
const char* Script::Execute(const std::string &code)
{
    if (luaL_dostring(state, code.c_str()) == 0) {
        return lua_tostring(state, -1);
    } else {
        return NULL;
    }
}
lua_State* Script::GetState() const
{
    return state;
}

EVENT(EVENT_INIT_SCRIPT)
{
    lua_State* lstate = (lua_State*)((OneArg*)args)->_arg;

//open the lua libraries here
    luaL_openlibs(lstate);
}
#endif

BOOL InitializeScript(void)
{
#ifdef MODULE_SCRIPTING
    world->WriteLog("Initializing scripting.");
    world->events.AddCallback("ScriptLoaded", EVENT_INIT_PLAYER_SCRIPT);
    world->commands.AddCommand(new CMDExecute());
    if (!InitWorldScript()) {
        world->WriteLog("Initialization of world script system failed.", ERR);
        return false;
    }
    if (!InitPlayerScript()) {
        world->WriteLog("Initialization of player script system failed.", ERR);
        return false;
    }
#endif
    return true;
}

#ifdef MODULE_SCRIPTING
CMDExecute::CMDExecute()
{
    SetName("execute");
    AddAlias("exec");
    AddAlias("eval");
    SetAccess(RANK_BUILDER);
}
BOOL CMDExecute::Execute(const std::string &verb, Player* mobile,std::vector<std::string> &args,int subcmd)
{
    timeval prev, now; //used for timing the whole thing.
    const char* ret = NULL;
    std::stringstream st;
    std::vector<std::string>::iterator it, itEnd;
    std::string input;
    int elapsed = 0;
    Script* scr = NULL;

    scr = new Script();
    if (!scr) {
        mobile->Message(MSG_INFO, "could not create script interface.");
        return false;
    }

    gettimeofday(&prev, NULL);

    itEnd = args.end();
    for (it = args.begin(); it != itEnd; ++it) {
        input += (*it);
    }

    ret = scr->Execute(input);
    st << "Result: " << (ret == NULL?"no return":ret) << "\n";
    delete scr;

    gettimeofday(&now, NULL);
    elapsed = (now.tv_sec - prev.tv_sec) * 1000;
    elapsed += (now.tv_usec - prev.tv_usec) / 1000;
    st << "[execution took " << elapsed << " ms].";

    mobile->Message(MSG_INFO, st.str());
    return true;
}

void SCR_Error(lua_State* l, const char* msg)
{
    lua_pushlstring(l, msg, strlen(msg));
    lua_error(l);
}

#endif
