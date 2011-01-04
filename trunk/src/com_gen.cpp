/*
*com_gen.cpp
*
*   Copyright 2010 Tyler Littlefield.
*
*   Licensed under the Apache License, Version 2.0 (the "License");
*   you may not use this file except in compliance with the License.
*   You may obtain a copy of the License at
*
*       http://www.apache.org/licenses/LICENSE-2.0
*
*   Unless required by applicable law or agreed to in writing, software
*   distributed under the License is distributed on an "AS IS" BASIS,
*   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*   See the License for the specific language governing permissions and
*   limitations under the License.
*/

#include <list>
#include <map>
#include <sstream>
#include "com_gen.h"
#include "utils.h"
#include "player.h"
#include "world.h"
#include "option.h"

void InitializeGenCommands()
{
    world->WriteLog("Initializing general commands.");
    world->AddCommand(new CMDQuit());
    world->AddCommand(new CMDSave());
    world->AddCommand(new CMDWho());
    world->AddCommand(new CMDToggle());
    world->AddCommand(new CMDScore());
    world->AddCommand(new CMDCommands());
    world->AddCommand(new CMDHist());
    world->AddCommand(new CMDUptime());
    world->AddCommand(new CMDWhois());
    world->AddCommand(new CMDLook());
}

//quit
CMDQuit::CMDQuit()
{
    SetName("quit");
}
BOOL CMDQuit::Execute(const std::string &verb, Player* mobile,std::vector<std::string> &args,int subcmd)
{
    mobile->Write("Thanks for playing, please come back again!\n");
    mobile->GetSocket()->Kill();
    return true;
}

//save
CMDSave::CMDSave()
{
    SetName("save");
}
BOOL CMDSave::Execute(const std::string &verb, Player* mobile,std::vector<std::string> &args,int subcmd)
{
    mobile->Write("Saving\n");
    mobile->Save();
    return true;
}

//who
CMDWho::CMDWho()
{
    SetName("who");
    AddAlias("wh");
}
BOOL CMDWho::Execute(const std::string &verb, Player* mobile,std::vector<std::string> &args,int subcmd)
{
    std::list <Player*>::iterator it;
    std::list <Player*>::iterator itEnd;
    std::stringstream st;

    mobile->Write(Center(Capitalize(MUD_NAME),80)+"\n");
    mobile->Write(Repete("-",80));

    itEnd=world->GetPlayers()->end();
    for (it = world->GetPlayers()->begin(); it != itEnd; ++it) {
        st << "[" << (*it)->GetLevel() << "] ";
        if ((*it)->HasAccess(RANK_GOD)) {
            st << C_BLUE << "[ADMIN] " << C_RESET;
        }
        st << Capitalize((*it)->GetName()) << " " << (*it)->GetTitle();
        mobile->Message(MSG_LIST, st.str());
        st.str("");
    }
    mobile->Write(Repete("-",80));
    st << MUD_NAME << " currently has " << (int)world->GetPlayers()->size() << ((int)world->GetPlayers()->size()==1?"user ":"users ") << "online.\n";
    mobile->Write(st.str());
    return true;
}

//toggle
CMDToggle::CMDToggle()
{
    SetName("toggle");
    AddAlias("tog");
}
BOOL CMDToggle::Execute(const std::string &verb, Player* mobile,std::vector<std::string> &args,int subcmd)
{
    std::map<std::string, Option> *options;
    std::map<std::string, Option>::iterator it;
    std::map<std::string, Option>::iterator itEnd;
    std::stringstream st;

//if no args were given, print out a list of options:
    if (!args.size()) {
        options=mobile->GetOptions();
        itEnd = options->end();
        for (it = options->begin(); it != itEnd; ++it) {
            if ((*it).second.CanToggle()) {
                st << (*it).second.GetName() << " ";
                st << ((*it).second.GetData().GetInt()==0?"off: ":"on ");
                st << (*it).second.GetHelp() << "\n";
            }
        }
        options=GetGlobalOptions();
        itEnd = options->end();
        for (it = options->begin(); it != itEnd; ++it) {
            if ((*it).second.CanToggle()) {
                st << (*it).second.GetName() << " ";
                st << ((*it).second.GetData().GetInt()==0?"off: ":"on ");
                st << (*it).second.GetHelp() << "\n";
            }
        }
        return true;
    }

    mobile->ToggleOption(args[0]);
    return true;
}

//score
CMDScore::CMDScore()
{
    SetName("score");
    AddAlias("sc");
}
BOOL CMDScore::Execute(const std::string &verb, Player* mobile,std::vector<std::string> &args,int subcmd)
{
    mobile->Write(Capitalize(mobile->GetName())+"\n");
    mobile->Write(Repete("-",80));
    TimeInfo tm(mobile->GetOnlineTime());
    mobile->Write("Online time:\t"+tm.ToString()+"\n");
    std::stringstream st;
    mobile->Write(Repete("-",80));
    st << "int: " << mobile->GetInt("int") << "wis: " << mobile->GetInt("wis") << "\nstr: " << mobile->GetInt("str") << " dex: " << mobile->GetInt("dex") << "\nagi: " << mobile->GetInt("agi") << " con: " << mobile->GetInt("con") << "\n";
    mobile->Write(st.str());
    return true;
}

//channels
CMDChan::CMDChan()
{
    SetName("channels");
    AddAlias("chan");
}
BOOL CMDChan::Execute(const std::string &verb, Player* mobile,std::vector<std::string> &args,int subcmd)
{
    Channel* chan=(Channel*)world->FindChannel(subcmd);
    std::string message=Explode(args);

    if (chan==NULL) {
        mobile->Message(MSG_ERROR,"That channel doesn't exist.");
        return false;
    }

    chan->Broadcast((Player*)mobile,message,true);
    return true;
}

//commands
CMDCommands::CMDCommands()
{
    SetName("commands");
}
BOOL CMDCommands::Execute(const std::string &verb, Player* mobile,std::vector<std::string> &args,int subcmd)
{
    std::vector <std::string> commands;
    COMMAND_TYPE type;

    if (!args.size()) {
        type=all;
    } else     if (args[0] == "general") {
        type = normal;
    } else if (args[0] == "all") {
        type = all;
    } else if (args[0] == "social") {
        type = social;
    } else if (args[0] == "movement") {
        type = movement;
    } else if (args[0] == "channel") {
        type = channel;
    } else {
        mobile->Message(MSG_INFO, "You must specify a valid type.\nValid types are: movement, general, all, channel and social.");
        return false;
    }

    world->ListCommands(&commands,mobile, type);
    mobile->Message(MSG_LIST, Columnize(&commands, 4));
    return true;
}

//history
CMDHist::CMDHist()
{
    SetName("history");
    AddAlias("hist");
}
BOOL CMDHist::Execute(const std::string &verb, Player* mobile,std::vector<std::string> &args,int subcmd)
{
    Channel* chan=NULL;
    std::list <HistoryNode*> *history;
    std::list <HistoryNode*>::iterator it;
    TimeInfo tm;

    if (!args.size()) {
        mobile->Message(MSG_INFO,"Syntax: hist [channel name].");
        return false;
    }

    chan=world->FindChannel(args[0]);
    if (chan==NULL) {
        mobile->Message(MSG_ERROR,"That channel doesn't exist.");
        return false;
    }

    history=chan->GetHistory();

    if (!history->size()) {
        mobile->Message(MSG_INFO,"There is no recorded history, perhaps you should say something?");
        return true;
    }

    for (it=history->begin(); it!=history->end(); it++) {
        tm.Calculate(time(NULL)-(*it)->when);
        mobile->Message(MSG_LIST,tm.ToString()+": "+(*it)->message);
    }

    return true;
}

//uptime
CMDUptime::CMDUptime()
{
    SetName("uptime");
}
BOOL CMDUptime::Execute(const std::string &verb, Player* mobile,std::vector<std::string> &args,int subcmd)
{
    TimeInfo ruptime(time(NULL)-world->GetRealUptime());
    TimeInfo cuptime(time(NULL)-world->GetCopyoverUptime());

    mobile->Message(MSG_INFO,Capitalize(MUD_NAME)+" has been up for "+ruptime.ToString()+".");
    if (world->GetCopyoverUptime()==world->GetRealUptime()) {
        mobile->Message(MSG_INFO,"There hasn't been a copyover this reboot.");
    } else {
        mobile->Message(MSG_INFO,"The last copyover occured "+cuptime.ToString()+" ago.");
    }

    return true;
}

//whois
CMDWhois::CMDWhois()
{
    SetName("whois");
    AddAlias("finger");
}
BOOL CMDWhois::Execute(const std::string &verb, Player* mobile,std::vector<std::string> &args,int subcmd)
{
    Player* targ=NULL;
    BOOL load=false;
    TimeInfo tm;

    if ((!args.size())||(args.size()!=1)) {
        mobile->Message(MSG_ERROR,"Syntax: whois <player>.");
        return false;
    }
//check to see if the player is online:
    targ=world->FindPlayer(args[0]);
    if (!targ) {
//check to see if the player can be loaded
        targ=world->LoadPlayer(args[0]);
        load=true;
    }
//check to see if the load succeeded.
    if (!targ) {
        mobile->Message(MSG_ERROR,"Couldn't find the specified player.");
        return false;
    }

//check to see if a player is trying to retrieve info on a god
    if ((targ->HasAccess(RANK_GOD))&&(!mobile->HasAccess(RANK_GOD))) {
        mobile->Message(MSG_ERROR,"You can't whois the gods.");
//delete the pointer if it was allocated by load
        if (load) {
            delete targ;
        }
        return false;
    }

    tm.Calculate(targ->GetOnlineTime());
    mobile->Message(MSG_LIST,Capitalize(targ->GetName()+" "+targ->GetTitle()));
    mobile->Message(MSG_LIST,Repete("-",80));
    mobile->Message(MSG_LIST,Capitalize(targ->GetName())+" has spent "+tm.ToString()+" online.");
    mobile->Message(MSG_LIST,Repete("-",80));
    if (load) {
        delete targ;
    }

    return true;
}

//look
CMDLook::CMDLook()
{
    SetName("look");
    AddAlias("l");
}
BOOL CMDLook::Execute(const std::string &verb, Player* mobile,std::vector<std::string> &args,int subcmd)
{
    if (!args.size()) {
        mobile->Message(MSG_INFO,mobile->GetLocation()->DoLook(mobile));

        return true;
    }
    Entity* obj=world->MatchObject(args[0],mobile);
    if (obj==NULL) {
        mobile->Message(MSG_ERROR,"You don't see that here.");
        return false;
    }

    mobile->Message(MSG_INFO,obj->DoLook(mobile));
    return true;
}