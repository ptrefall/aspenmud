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

World::World()
{
  _running = true;
  _server=new Server();
  _log=new Log();
  _log->Open(EVENT_FILE);
  _users=new std::list<Player*>();
  _channels=new std::map<int,Channel*>();
  _options = new std::map<std::string, Option*>();
  _cfactory=new ComponentFactory();
  _properties=new std::map<std::string,void*>();
  _zones=new std::vector<Zone*>();
  _rooms=new std::map<VNUM,Room*>();
  _objects=new std::map<VNUM,Entity*>();
  _onumPool=new std::vector <VNUM>();
  _rnumPool=new std::vector<VNUM>();
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
  events.RegisterEvent("Copyover", new Event());
#ifdef MODULE_SCRIPTING
  events.RegisterEvent("ScriptLoaded", new Event());
  events.RegisterEvent("ScriptUnloaded", new Event());
#endif
}

World::~World()
{
  std::map<int,Channel*>::iterator cit, citEnd;
  std::vector<Zone*>::iterator zit, zitEnd;
  std::map<std::string, Option*>::iterator oit, oitEnd;

  delete [] _motd;
  delete [] _banner;
  delete _log;
  delete _server;
  delete _users;

  citEnd=_channels->end();
  for (cit = _channels->begin(); cit != citEnd; ++cit)
    {
      delete (*cit).second;
    }
  delete _channels;

  oitEnd = _options->end();
  for (oit = _options->begin(); oit != oitEnd; ++oit)
    {
      delete (*oit).second;
    }
  delete _options;

  delete _cfactory;
  delete _properties;

  zitEnd = _zones->end();
  for (zit = _zones->begin(); zit != zitEnd; ++zit)
    {
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
  for (it = players.begin(); it != itEnd; ++it)
    {
      person = (*it);
      person->Message(MSG_CRITICAL,"The mud is shutting down now. Your Character will be autosaved.");
      person->GetSocket()->Kill();
    }
  events.CallEvent("Shutdown", NULL, (void*)this);
  _running = false;
}
void World::Copyover(Player* mobile)
{
  int cuptime=(int)GetCopyoverUptime();
  int ruptime = (int)GetRealUptime();

  FILE* copyover=fopen(COPYOVER_FILE,"wb");
  if (copyover==NULL)
    {
      mobile->Message(MSG_ERROR,"couldn't open the copyover file.\nCopyover will not continue.");
      return;
    }
  Player* person;

  fprintf(copyover, "%d %d\n", cuptime, ruptime);
  sockaddr_in* addr=NULL;
//itterate through the players and write info to their copyover file:
  std::list <Player*>::iterator it, itEnd;
  char buff[16];

  itEnd=GetPlayers()->end();
  for (it = GetPlayers()->begin(); it != itEnd; ++it)
    {
      person = (*it);
      if (person->GetSocket()->GetConnectionType()!=con_game)
        {
          person->Write("We're sorry, but we are currently rebooting; please come back again soon.\n");
          person->GetSocket()->Kill();
          continue;
        }
      addr=person->GetSocket()->GetAddr();
      person->Save();
      fprintf(copyover,"%d %s %hd %hu %lu %s\n",
              person->GetSocket()->GetControl(), person->GetName().c_str(),
              addr->sin_family,addr->sin_port,(long int)addr->sin_addr.s_addr, person->GetSocket()->GetHost().c_str());
      person->Write("Copyover initiated by "+mobile->GetName()+".\n");
    }
  world->Update();
  fprintf(copyover,"-1\n");
  fclose(copyover);
  events.CallEvent("Copyover", NULL, (void*)this);
  memset(buff,0,16);
  snprintf(buff,16,"%d",world->GetServer()->GetListener());
  execl(BIN_FILE,BIN_FILE,"-c",buff,(char*)NULL);
  mobile->Write("Copyover failed!\n");
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

BOOL World::AddPlayer(Player* player)
{
  if (!player)
    {
      return false;
    }
  if (player->GetSocket()->GetConnectionType() != con_game)
    {
      return false;
    }

  _users->push_back(player);
  return true;
}
BOOL World::RemovePlayer(Player* player)
{
  std::list<Player*>::iterator it;
  std::list<Player*>::iterator itEnd = _users->end();

  if (_users->size())
    {
      for (it = _users->begin(); it != itEnd; ++it)
        {
          if ((*it) == player)
            {
              _users->erase(it);
              return true;
            }
        }
    }

  return false;
}
Player* World::FindPlayer(const std::string &name) const
{
  std::list<Player*>::iterator it;
  std::list <Player*>::iterator itEnd;

  itEnd=_users->end();
  if (_users->size())
    {
      for (it = _users->begin(); it != itEnd; ++it)
        {
          if ((*it)->GetName()==name)
            {
              return (*it);
            }
        }
    }

  return NULL;
}
Player* World::LoadPlayer(const std::string &name) const
{

  if (PlayerExists(name))
    {
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
  for (it = _channels->begin(); it != itEnd; ++it)
    {
      out->push_back(((*it).second)->GetName());
    }
}

void World::AddChannel(Channel* chan,BOOL command)
{
  (*_channels)[_chanid]=chan;
  RegisterOption(new Option(chan->GetName(),
                            "Controls whether or not you can hear and broadcast to "+chan->GetName()+".",
                            chan->GetAccess(), (chan->GetName()=="newbie"?Variant((int)1):Variant((int)0)), VAR_INT, true));
  if (command)
    {
      CMDChan* com = new CMDChan();
      com->SetName(chan->GetName());
      com->SetAccess(chan->GetAccess());
      if (chan->GetAlias() != "")
        {
          com->AddAlias(chan->GetAlias());
        }
      commands.AddCommand(com);
    }
  _chanid++;
}

Channel* World::FindChannel(const int id) const
{
  if (!_channels->count(id))
    {
      return NULL;
    }
  return (*_channels)[id];
}

Channel* World::FindChannel(const std::string &name)
{
//This method is a bit slower because we have to iterate through the mapping ourselves.
  std::map<int,Channel*>::iterator it;
  std::map<int, Channel*>::iterator itEnd;

  for (it = _channels->begin(); it != itEnd; ++it)
    {
      if ((*it).second->GetName()==name)
        {
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
  if(stat(LOGIN_FILE,fs))
    {
      WriteLog("Could not stat login file.", CRIT);
      return false;
    }
  _banner=new char[fs->st_size+1];
  memset(_banner,0,fs->st_size+1);
//open and load the banner:
  FILE* banner_fd=fopen(LOGIN_FILE,"r");
  if (!banner_fd)
    {
      WriteLog("Could not fopen banner file.", CRIT);
      delete []_banner;
      return false;
    }
  if ((int)fread(_banner,1,fs->st_size,banner_fd)!=(int)fs->st_size)
    {
      WriteLog("Error loading banner.", CRIT);
      return false;
    }
  fclose(banner_fd);
  delete fs;
//load our motd:
//retrieve size of file so we can create the buffer:
  fs=new struct stat();
  if (stat(MOTD_FILE,fs))
    {
      WriteLog("Could not stat MOTD file.", CRIT);
      return false;
    }

  _motd=new char[fs->st_size+1];
  memset(_motd,0,fs->st_size+1);
  FILE* motd_fd=fopen(MOTD_FILE,"r");
  if (!motd_fd)
    {
      WriteLog("Could not fopen MOTD.", CRIT);
      return false;
    }

  if ((int)fread(_motd,1,fs->st_size,motd_fd)!=(int)fs->st_size)
    {
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
  if (_users->size())
    {
      pitEnd = _users->end();
      for (pit = _users->begin(); pit != pitEnd; ++pit)
        {
          (*pit)->Update();
        }
    }

//sleep so that we don't kill our cpu
  _server->Sleep(PULSES_PER_SECOND);
}

BOOL World::RegisterComponent(const std::string &name,COMCREATECB cb, COMINITCB ib)
{
  if (ib!=NULL)
    {
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
  if (RoomExists(num))
    {
      return false;
    }

  (*_rooms)[num]=room;
  return true;
}
Room* World::GetRoom(VNUM num)
{
  if (!RoomExists(num))
    {
      return NULL;
    }
  return (*_rooms)[num];
}
BOOL World::RemoveRoom(VNUM num)
{
  if (!RoomExists(num))
    {
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
  if (ObjectExists(num))
    {
      return false;
    }

  (*_objects)[num]=obj;
  return true;
}
Entity* World::GetObject(VNUM num)
{
  if (ObjectExists(num))
    {
      return (*_objects)[num];
    }

  return false;
}
BOOL World::RemoveObject(VNUM num)
{
  if (ObjectExists(num))
    {
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
  if (!_properties->count(name))
    {
      (*_properties)[name]=ptr;
      return true;
    }
  return false;
}
void* World::GetProperty(const std::string &name) const
{
  if (_properties->count(name))
    {
      return (*_properties)[name];
    }
  return NULL;
}
BOOL World::RemoveProperty(const std::string &name)
{
  if (_properties->count(name))
    {
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
  for (i = 0; i < len; i++)
    {
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
  if (i != len)
    {
      // parse arguments
      for (; i < len; i++)
        {
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
          if (isalnum(line[i]))
            {
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
  for (it = cptr->begin(); it != itEnd; ++it)
    {
      if (((*it)->GetName() == cmd)||((*it)->HasAlias(cmd, true)))
        {
          if (!mobile->HasAccess((*it)->GetAccess()))
            {
              return false;
            }
          switch((*it)->GetType())
            {
            default:
              WriteLog("Invalid command type.");
              break;
            case normal:
            case social:
            case movement:
            case channel:
              (*it)->Execute((*it)->GetName(), mobile, params, (*it)->GetSubcmd());
              return true;
              break;
            case script:
              break;
            }
        }
    }
//todo: check inventory and room commands here.
  return false;
}

Entity* World::MatchKeyword(const std::string &name, Player* caller)
{
  if ((name=="me")||(name==caller->GetName()))
    {
      return (Entity*)caller;
    }
  if (name.length() < caller->GetName().length() && caller->GetName().substr(name.length()) == name)
    {
      return (Entity*)caller;
    }
  if (name == "here")
    {
      return (caller->GetLocation());
    }

  return NULL;
}

Entity* World::MatchObject(const std::string &name,Player* caller)
{
  std::list<Entity*> *contents; //holds contents for the location and current caller.
  std::list<Entity*>* val;
  std::list<Entity*>::iterator it, itEnd;
  Entity* obj = NULL;

  obj = MatchKeyword(name, caller);
  if (obj)
    {
      return obj;
    }

  contents = new std::list<Entity*>();
  val = caller->GetLocation()->GetContents();
  contents->insert(contents->begin(), val->begin(), val->end());
  val = caller->GetContents();
  contents->insert(contents->begin(), val->begin(), val->end());
  obj = MatchObjectInList(name, val);
  delete contents;
  return obj;
}
Entity* World::MatchObjectInList(const std::string &name, std::list<Entity*> *olist)
{
  std::list<Entity*>::iterator it, itEnd;
  std::string sub; //used for holding the subpart of the number.
  std::string temp; //used for holding a temp copy of the name after it is trimmed.
  std::string alias; //holds a copy of the alias.
  Entity* obj = NULL; //the object we are currently examining/returning.
  std::vector<std::string>* aliases = NULL; //a list of aliases.
  std::vector<std::string>::iterator ait, aitEnd;
  int number = 0; //used for holding the actual number.
  int count = 0; //used for holding the number of objects found.
  size_t marker; //used for holding the position of the '.'.

  if (!olist->size())
    {
      return NULL;
    }

//we check to see if the string has a '.', if so, there's a number.
  marker = name.find_first_of(".");
//check to see if it is 1) at the beginning, or 2) at the end.
  if ((marker == 0) || (marker == name.length()))
    {
      return NULL;
    }

  if (marker != std::string::npos)   //we found something.
    {
      sub = name.substr(marker); //the subnumber.
      temp = name.substr(marker+1); //trim off the x. bit
      number = atoi(sub.c_str());
      if (number == 0)
        {
          return NULL;
        }

      itEnd = olist->end();
      for (it = olist->begin(); it != itEnd && count < number; ++it)
        {
          obj = *it;
          if (obj->GetName().length() < temp.length())   //we check for a partial match
            {
              if (obj->GetName().substr(temp.length()) == temp)
                {
                  count++; //we found a match, increase the counter.
                  continue;
                }
            }
          if (obj->GetName() == temp)   //full match
            {
              count++;
              continue;
            }

//now we check a list of aliases.
          aliases = obj->GetAliases();
          if (aliases->size())
            {
              aitEnd = aliases->end();
              for (ait = aliases->begin(); ait != aitEnd; ++ait)
                {
                  alias = (*ait);
                  if (alias.length() < temp.length())   //we check for a partial match
                    {
                      if (alias.substr(temp.length()) == temp)
                        {
                          count++; //we found a match, increase the counter.
                          continue;
                        }
                    }
                  if (alias == temp)   //full match
                    {
                      count++;
                      continue;
                    }
                }
            }
        }
      if (count != (number -1))
        {
          return NULL;
        }
      else
        {
          return obj;
        }
    }
  else
    {
      itEnd = olist->end();
      for (it = olist->begin(); it != itEnd; ++it)
        {
          obj = *it;

          if (obj->GetName().length() < temp.length())   //we check for a partial match
            {
              if (obj->GetName().substr(temp.length()) == temp)
                {
                  return obj;
                }
            }
          if (obj->GetName() == temp)   //full match
            {
              return obj;
            }

//now we check a list of aliases.
          aliases = obj->GetAliases();
          if (aliases->size())
            {
              aitEnd = aliases->end();
              for (ait = aliases->begin(); ait != aitEnd; ++ait)
                {
                  alias = *ait;
                  if (alias.length() < temp.length())   //we check for a partial match
                    {
                      if (alias.substr(temp.length()) == temp)
                        {
                          return obj;
                        }
                    }
                  if (alias == temp)   //full match
                    {
                      return obj;
                    }
                }
            }
        }
    }

  return NULL;
}
Entity* World::MatchObjectInVector(const std::string &name, std::vector<Entity*> *olist)
{
  std::vector<Entity*>::iterator it, itEnd;
  std::string sub; //used for holding the subpart of the number.
  std::string temp; //used for holding a temp copy of the name after it is trimmed.
  std::string alias; //holds a copy of the alias.
  Entity* obj = NULL; //the object we are currently examining/returning.
  std::vector<std::string>* aliases = NULL; //a list of aliases.
  std::vector<std::string>::iterator ait, aitEnd;
  int number = 0; //used for holding the actual number.
  int count = 0; //used for holding the number of objects found.
  size_t marker; //used for holding the position of the '.'.

  if (!olist->size())
    {
      return NULL;
    }

//we check to see if the string has a '.', if so, there's a number.
  marker = name.find_first_of(".");
//check to see if it is 1) at the beginning, or 2) at the end.
  if ((marker == 0) || (marker == name.length()))
    {
      return NULL;
    }

  if (marker != std::string::npos)   //we found something.
    {
      sub = name.substr(marker); //the subnumber.
      temp = name.substr(marker+1); //trim off the x. bit
      number = atoi(sub.c_str());
      if (number == 0)
        {
          return NULL;
        }

      itEnd = olist->end();
      for (it = olist->begin(); it != itEnd && count < number; ++it)
        {
          obj = *it;
          if (obj->GetName().length() < temp.length())   //we check for a partial match
            {
              if (obj->GetName().substr(temp.length()) == temp)
                {
                  count++; //we found a match, increase the counter.
                  continue;
                }
            }
          if (obj->GetName() == temp)   //full match
            {
              count++;
              continue;
            }

//now we check a list of aliases.
          aliases = obj->GetAliases();
          if (aliases->size())
            {
              aitEnd = aliases->end();
              for (ait = aliases->begin(); ait != aitEnd; ++ait)
                {
                  alias = (*ait);
                  if (alias.length() < temp.length())   //we check for a partial match
                    {
                      if (alias.substr(temp.length()) == temp)
                        {
                          count++; //we found a match, increase the counter.
                          continue;
                        }
                    }
                  if (alias == temp)   //full match
                    {
                      count++;
                      continue;
                    }
                }
            }
        }
      if (count != (number -1))
        {
          return NULL;
        }
      else
        {
          return obj;
        }
    }
  else
    {
      itEnd = olist->end();
      for (it = olist->begin(); it != itEnd; ++it)
        {
          obj = *it;

          if (obj->GetName().length() < temp.length())   //we check for a partial match
            {
              if (obj->GetName().substr(temp.length()) == temp)
                {
                  return obj;
                }
            }
          if (obj->GetName() == temp)   //full match
            {
              return obj;
            }

//now we check a list of aliases.
          aliases = obj->GetAliases();
          if (aliases->size())
            {
              aitEnd = aliases->end();
              for (ait = aliases->begin(); ait != aitEnd; ++ait)
                {
                  alias = *ait;
                  if (alias.length() < temp.length())   //we check for a partial match
                    {
                      if (alias.substr(temp.length()) == temp)
                        {
                          return obj;
                        }
                    }
                  if (alias == temp)   //full match
                    {
                      return obj;
                    }
                }
            }
        }
    }

  return NULL;
}

BOOL World::AddZone(Zone* zone)
{
  std::vector<Zone*>::iterator it;
  std::vector<Zone*>::iterator itEnd = _zones->end();

  if (_zones->size())
    {
      for (it=_zones->begin(); it != itEnd; ++it)
        {
          if ((*it)==zone)
            {
              return false;
            }
        }
    }
  _zones->push_back(zone);
  return true;
}
BOOL World::RemoveZone(Zone* zone)
{
  std::vector<Zone*>::iterator it;
  std::vector <Zone*>::iterator itEnd = _zones->end();
  for (it = _zones->begin(); it != itEnd; ++it)
    {
      if ((*it)==zone)
        {
          _zones->erase(it);
          return true;
        }
    }

  return false;
}
Zone* World::GetZone(const std::string &name)
{
  std::vector <Zone*>::iterator it;
  std::vector<Zone*>::iterator itEnd = _zones->end();


  for (it=_zones->begin(); it != itEnd; it++)
    {
      if (name==(*it)->GetName())
        {
          return (*it);
        }
    }

  return NULL;
}
BOOL World::GetZones(std::vector<Zone*> *zones)
{
  if (!zones)
    {
      return false;
    }

  std::vector<Zone*>::iterator it;
  std::vector<Zone*>::iterator itEnd = _zones->end();

  if (_zones->size())
    {
      for (it=_zones->begin(); it != itEnd; ++it)
        {
          zones->push_back((*it));
        }
    }
  return true;
}

BOOL World::CreateObject(Entity* obj)
{
  VNUM id;

  if (_onumPool->size())
    {
      id=_onumPool->back();
      _onumPool->pop_back();
      obj->SetOnum(id);
      AddObject(id,obj);
      return true;
    }
  else
    {
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

  if (_rnumPool->size())
    {
      id=_rnumPool->back();
      _rnumPool->pop_back();
      room->SetOnum(id);
      AddRoom(id,room);
      return true;
    }
  else
    {
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
  for (rt=_rooms->begin(); rt!=_rooms->end(); rt++)
    {
      if ((*rt).first>max)
        {
          max=(*rt).first;
        }
    }
  _maxRnum=max;
  for (i=0; i!=max; i++)
    {
      if (!GetRoom(i))
        {
//GetRoom returned NULL, we have an empty number to add to the pool
          _rnumPool->push_back(i);
        }
    }

//now we do the same with objects:
  max=0;
  for (ot=_objects->begin(); ot!=_objects->end(); ot++)
    {
      if ((*ot).first>max)
        {
          max=(*ot).first;
        }
    }
  _maxOnum=max;
  for (i=0; i!=max; i++)
    {
      if (!GetObject(i))
        {
          _onumPool->push_back(i);
        }
    }
}

void World::WriteLog(const std::string &data,LOG_LEVEL l)
{
  _log->Write(data, l);
}

BOOL World::IsRunning() const
{
  return _running;
}
void World::SetRunning(BOOL running)
{
  _running = running;
}

BOOL World::RegisterOption(Option* option)
{
  if (!option)
    {
      return false;
    }

  if (OptionExists(option->GetName()))
    {
      return false;
    }

  (*_options)[option->GetName()] = option;
  return true;
}
BOOL World::OptionExists(const std::string &name)
{
  return (_options->count(name)?true:false);
}
Option* World::GetGlobalOption(const std::string &name)
{
  return (OptionExists(name)? (*_options)[name]:NULL);
}
std::map<std::string, Option*>* World::GetGlobalOptions()
{
  return _options;
}
