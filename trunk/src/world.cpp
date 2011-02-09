/*
*world.cpp
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

#include <sys/stat.h>
#include <list>
#include <map>
#include <vector>
#include <string>
#include <cstring>
#include <ctime>
#include "world.h"
#include "channel.h"
#include "player.h"
#include "socket.h"
#include "com_gen.h"
#include "command.h"
#include "component.h"
#include "ComponentFactory.h"
#include "utils.h"
#include "zone.h"
#include "option.h"
extern BOOL running;

World::World()
{
    _server=new Server();
    _log=new Log();
    _log->Open(EVENT_FILE);
    _users=new std::list<Player*>();
    _channels=new std::map<int,Channel*>();
    _cfactory=new ComponentFactory();
    _properties=new std::map<std::string,void*>();
    _zones=new std::list<Zone*>();
    _rooms=new std::map<VNUM,Room*>();
    _objects=new std::map<VNUM,Entity*>();
    _onumPool=new std::list <VNUM>();
    _rnumPool=new std::list<VNUM>();
    _maxOnum=0;
    _maxRnum=0;
    _chanid=1;

//events
    events.RegisterEvent("LivingPulse",new DelayedEvent(LIVING_PULSE,0));
    events.RegisterEvent("WorldPulse", new DelayedEvent(WORLD_PULSE, 0));
    events.RegisterEvent("PlayerConnect", new Event());
    events.RegisterEvent("PlayerDisconnect", new Event());
    events.RegisterEvent("PlayerCreated", new Event());
    events.RegisterEvent("PlayerDeleted", new Event());
    events.RegisterEvent("Shutdown", new Event());
#ifdef MODULE_SCRIPTING
    events.RegisterEvent("ScriptLoaded", new Event());
    events.RegisterEvent("ScriptUnloaded", new Event());
#endif
}

World::~World()
{
    std::map<int,Channel*>::iterator cit;
    std::map<int,Channel*>::iterator citEnd;
    std::list<Zone*>::iterator zit;
    std::list<Zone*>::iterator zitEnd;

    delete [] _motd;
    delete [] _banner;
    delete _log;
    delete _server;
    delete _users;

    citEnd=_channels->end();
    for (cit = _channels->begin(); cit != citEnd; ++cit) {
        delete (*cit).second;
    }
    delete _channels;
    delete _cfactory;
    delete _properties;

    zitEnd = _zones->end();
    for (zit = _zones->begin(); zit != zitEnd; ++zit) {
        delete (*zit);
    }
    delete _zones;
    delete _rooms;
    delete _objects;
    delete _onumPool;
    delete _rnumPool;
}

void World::Shutdown()
{
    std::list <Player*>::iterator it;
    std::list <Player*>::iterator itEnd;
    std::list<Player*> players(_users->begin(), _users->end());

    Player* person = NULL;

    itEnd = players.end();
    for (it = players.begin(); it != itEnd; ++it) {
        person = (*it);
        person->Message(MSG_CRITICAL,"The mud is shutting down now. Your Character will be autosaved.");
        person->GetSocket()->Kill();
    }
    events.CallEvent("Shutdown", NULL, (void*)this);
    running = false;
}

Server* World::GetServer(void) const
{
    return _server;
}

Log* World::GetLog(void) const
{
    return _log;
}

std::list <Player*> *World::GetPlayers(void) const
{
    return _users;
}

void World::AddPlayer(Player* player)
{
    _users->push_back(player);
}
void World::RemovePlayer(Player* player)
{
    _users->remove(player);
}
Player* World::FindPlayer(const std::string &name) const
{
    std::list<Player*>::iterator it;
    std::list <Player*>::iterator itEnd;

    itEnd=_users->end();
    for (it = _users->begin(); it != itEnd; ++it) {
        if ((*it)->GetName()==name) {
            return (*it);
        }
    }
    return NULL;
}
Player* World::LoadPlayer(const std::string &name) const
{

    if (PlayerExists(name)) {
        Player* p=new Player();
        p->SetName(name);
        p->Load();
        return p;
    }

    return NULL;
}

void World::GetChannelNames(std::list <std::string>* out)
{
    std::map<int,Channel*>::iterator it;
    std::map<int, Channel*>::iterator itEnd;

    itEnd = _channels->end();
    for (it = _channels->begin(); it != itEnd; ++it) {
        out->push_back(((*it).second)->GetName());
    }
}

void World::AddChannel(Channel* chan,BOOL command)
{
    (*_channels)[_chanid]=chan;
    RegisterOption(chan->GetName(),
                   "Controls whether or not you can hear and broadcast to "+chan->GetName()+".",
                   chan->GetAccess(), (chan->GetName()=="newbie"?Variant((int)1):Variant((int)0)), VAR_INT, true);
    if (command) {
        CMDChan* com = new CMDChan();
        com->SetName(chan->GetName());
        com->SetAccess(chan->GetAccess());
        if (chan->GetAlias() != "") {
            com->AddAlias(chan->GetAlias());
        }
        commands.AddCommand(com);
    }
    _chanid++;
}

Channel* World::FindChannel(const int id) const
{
    if (!_channels->count(id)) {
        return NULL;
    }
    return (*_channels)[id];
}

Channel* World::FindChannel(const std::string &name)
{
//This method is a bit slower because we have to iterate through the mapping ourselves.
    std::map<int,Channel*>::iterator it;
    std::map<int, Channel*>::iterator itEnd;

    for (it = _channels->begin(); it != itEnd; ++it) {
        if ((*it).second->GetName()==name) {
            return ((*it).second);
        }
    }
    return NULL;
}

BOOL World::InitializeFiles(void)
{
    struct stat *fs; //holds file stats
//load our banner:
//retrieve size of file so we can create the buffer:
    fs=new struct stat();
    if(stat(LOGIN_FILE,fs)) {
        WriteLog("Could not stat login file.", CRIT);
        return false;
    }
    _banner=new char[fs->st_size+1];
    memset(_banner,0,fs->st_size+1);
//open and load the banner:
    FILE* banner_fd=fopen(LOGIN_FILE,"r");
    if (!banner_fd) {
        WriteLog("Could not fopen banner file.", CRIT);
        delete []_banner;
        return false;
    }
    if ((int)fread(_banner,1,fs->st_size,banner_fd)!=(int)fs->st_size) {
        WriteLog("Error loading banner.", CRIT);
        return false;
    }
    fclose(banner_fd);
    delete fs;
//load our motd:
//retrieve size of file so we can create the buffer:
    fs=new struct stat();
    if (stat(MOTD_FILE,fs)) {
        WriteLog("Could not stat MOTD file.", CRIT);
        return false;
    }

    _motd=new char[fs->st_size+1];
    memset(_motd,0,fs->st_size+1);
    FILE* motd_fd=fopen(MOTD_FILE,"r");
    if (!motd_fd) {
        WriteLog("Could not fopen MOTD.", CRIT);
        return false;
    }

    if ((int)fread(_motd,1,fs->st_size,motd_fd)!=(int)fs->st_size) {
        WriteLog("Error loading MOTD.", CRIT);
        return false;
    }
    fclose(motd_fd);
    delete fs;
    WriteLog("Files loaded successfully");
    return true;
}

const char* World::GetBanner(void) const
{
    return _banner;
}

const char* World::GetMotd(void) const
{
    return _motd;
}

void World::Update(void) const
{
    std::list <Player*>::iterator pit;
    std::list<Player*>::iterator pitEnd;
//flushes the output buffers of all sockets.
    _server->FlushSockets();
//checks for incoming connections or commands
    _server->PollSockets();
//update living objects:
    if (_users->size()) {
        pitEnd = _users->end();
        for (pit = _users->begin(); pit != pitEnd; ++pit) {
            (*pit)->Update();
        }
    }

//sleep so that we don't kill our cpu
    _server->Sleep(10);
}

BOOL World::RegisterComponent(const std::string &name,COMCREATECB cb, COMINITCB ib)
{
    if (ib!=NULL) {
        (ib)();
    }

    return _cfactory->RegisterComponent(name,cb);
}
Component*  World::CreateComponent(const std::string &name)
{
    return _cfactory->Create(name);
}

BOOL World::AddRoom(VNUM num,Room* room)
{
    if (RoomExists(num)) {
        return false;
    }

    (*_rooms)[num]=room;
    return true;
}
Room* World::GetRoom(VNUM num)
{
    if (!RoomExists(num)) {
        return NULL;
    }
    return (*_rooms)[num];
}
BOOL World::RemoveRoom(VNUM num)
{
    if (!RoomExists(num)) {
        return false;
    }
    _rooms->erase(num);
    return true;
}
BOOL World::RoomExists(VNUM num)
{
    return (_rooms->count(num)==0?false:true);
}

BOOL World::AddObject(VNUM num, Entity* obj)
{
    if (ObjectExists(num)) {
        return false;
    }

    (*_objects)[num]=obj;
    return true;
}
Entity* World::GetObject(VNUM num)
{
    if (ObjectExists(num)) {
        return (*_objects)[num];
    }

    return false;
}
BOOL World::RemoveObject(VNUM num)
{
    if (ObjectExists(num)) {
        _objects->erase(num);
        return true;
    }

    return false;
}
BOOL World::ObjectExists(VNUM num)
{
    return (_objects->count(num)==0?false:true);
}

time_t World::GetRealUptime(void) const
{
    return _ruptime;
}
void World::SetRealUptime(time_t tm)
{
    _ruptime=tm;
}

time_t World::GetCopyoverUptime(void) const
{
    return _cuptime;
}
void World::SetCopyoverUptime(time_t tm)
{
    _cuptime=tm;
}

BOOL World::AddProperty(const std::string &name,void* ptr)
{
    if (!_properties->count(name)) {
        (*_properties)[name]=ptr;
        return true;
    }
    return false;
}
void* World::GetProperty(const std::string &name) const
{
    if (_properties->count(name)) {
        return (*_properties)[name];
    }
    return NULL;
}
BOOL World::RemoveProperty(const std::string &name)
{
    if (_properties->count(name)) {
        _properties->erase(name);
        return true;
    }
    return false;
}

BOOL World::DoCommand(Player* mobile,std::string args)
{
    std::vector<Command*>* cptr = commands.GetPtr();
    std::string cmd = ""; // the parsed command name
    const char *line = args.c_str(); // the command line
    int len = strlen(line); // get length of string
    int i = 0; // counter
    std::vector<std::string> params; // the parameters being passed to the command
    //std::list<Command*>* externals; //external commands
    std::vector <Command*>::iterator it; //an iterator for iterating through the command list
    std::vector <Command*>::iterator itEnd; //an iterator to point to the end of the commands list.
    // parse command name
    for (i = 0; i < len; i++) {
        if (line[i] == ' ') break;
    }

    // copy the command
    cmd = args.substr(0, i);
//make the command lowercase
    /*
        for (int n = 0; n < (int)cmd.length(); n++) {
            cmd[n] = tolower(cmd[n]);
        }
    */

    // are there any arguments to parse?
    if (i != len) {
        // parse arguments
        for (; i < len; i++) {
            if (line[i] == ' ') continue;
            // is it a quoated argument
            /*
                        if ((line[i] == '\'') || (line[i] == '"')) {
                            char match = line[i];
                            int arg_start = i + 1;
                            i++;
                            // loop until we reach the closing character
                            for (; i < len; i++) if (line[i] == match) break;
                            int arg_end = i;
                            params.push_back(args.substr(arg_start, arg_end - arg_start));
                        }
            */
            if (isalnum(line[i])) {
                int arg_start = i;
                for (; i < len; i++)
                    if ((line[i] == ' '))
                        break;
                int arg_end = i;
                params.push_back(args.substr(arg_start, arg_end - arg_start));
            }
        }
    }
//locate and execute the command:
//check the built-in commands first, then contents, then location.
    itEnd = cptr->end();
    for (it = cptr->begin(); it != itEnd; ++it) {
        if (((*it)->GetName() == cmd)||((*it)->HasAlias(cmd, true))) {
            if (!mobile->HasAccess((*it)->GetAccess())) {
                return false;
            }
            switch((*it)->GetType()) {
            default:
                WriteLog("Invalid command type.");
                break;
            case normal:
            case social:
            case movement:
            case channel:
                return (*it)->Execute((*it)->GetName(), mobile, params, (*it)->GetSubcmd());
                break;
            case script:
                break;
            }
        }
    }
//todo: check inventory and room commands here.
    return false;
}

Entity* World::MatchObject(const std::string &name,Player* caller)
{
    if ((name=="me")||(name==caller->GetName())) {
        return (Entity*)caller;
    }
    if (name == "here") {
        return (caller->GetLocation());
    }

    return NULL;
}

BOOL World::AddZone(Zone* zone)
{
    std::list<Zone*>::iterator it;

    if (_zones->size()) {
        for (it=_zones->begin(); it!=_zones->end(); it++) {
            if ((*it)==zone) {
                return false;
            }
        }
    }
    _zones->push_back(zone);
    return true;
}
BOOL World::RemoveZone(Zone* zone)
{
    std::list<Zone*>::iterator it;

    for (it=_zones->begin(); it!=_zones->end(); it++) {
        if ((*it)==zone) {
            _zones->erase(it);
            return true;
        }
    }

    return false;
}
Zone* World::GetZone(const std::string &name)
{
    std::list<Zone*>::iterator it;

    for (it=_zones->begin(); it!=_zones->end(); it++) {
        if (name==(*it)->GetName()) {
            return (*it);
        }
    }

    return NULL;
}
void World::GetZones(std::list <Zone*> *zones)
{
    std::list<Zone*>::iterator it;
    for (it=_zones->begin(); it!=_zones->end(); it++) {
        zones->push_back((*it));
    }
}

BOOL World::CreateObject(Entity* obj)
{
    VNUM id;

    if (_onumPool->size()) {
        id=_onumPool->back();
        _onumPool->pop_back();
        obj->SetOnum(id);
        AddObject(id,obj);
        return true;
    } else {
        _maxOnum++;
        id=_maxOnum;
        obj->SetOnum(id);
        AddObject(id,obj);
        return true;
    }

    return false;
}
BOOL World::CreateRoom(Room* room)
{
    VNUM id;

    if (_rnumPool->size()) {
        id=_rnumPool->back();
        _rnumPool->pop_back();
        room->SetOnum(id);
        AddRoom(id,room);
        return true;
    } else {
        _maxOnum++;
        id=_maxOnum;
        room->SetOnum(id);
        AddRoom(id,room);
        return true;
    }

    return false;
}

void World::InitializeNums(void)
{
    WriteLog("Initializing vnums for pool.");
    int max=0;
    int i;
    std::map<VNUM,Room*>::iterator rt;
    std::map<VNUM,Entity*>::iterator ot;

//iterate through rooms first and find the max number.
    for (rt=_rooms->begin(); rt!=_rooms->end(); rt++) {
        if ((*rt).first>max) {
            max=(*rt).first;
        }
    }
    _maxRnum=max;
    for (i=0; i!=max; i++) {
        if (!GetRoom(i)) {
//GetRoom returned NULL, we have an empty number to add to the pool
            _rnumPool->push_back(i);
        }
    }

//now we do the same with objects:
    max=0;
    for (ot=_objects->begin(); ot!=_objects->end(); ot++) {
        if ((*ot).first>max) {
            max=(*ot).first;
        }
    }
    _maxOnum=max;
    for (i=0; i!=max; i++) {
        if (!GetObject(i)) {
            _onumPool->push_back(i);
        }
    }
}

void World::WriteLog(const std::string &data,LOG_LEVEL l)
{
    _log->Write(data, l);
}