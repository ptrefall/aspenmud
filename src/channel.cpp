#include <sstream>
#include <string>
#include <list>
#include "mud.h"
#include "channel.h"
#include "player.h"
#include "world.h"
#include "utils.h"
#include "option.h"

void InitializeChannels()
{
  World* world = World::GetPtr();

  world->WriteLog("Initializing channels");
  world->events.AddCallback("PlayerConnect", SubscribeChannels);
  world->events.AddCallback("PlayerDisconnect", UnsubscribeChannels);
//Register channels here.
  world->AddChannel(new Channel("chat","ch",RANK_PLAYER));
}

EVENT(SubscribeChannels)
{
  World* world = World::GetPtr();

  std::list <std::string> *names=new std::list<std::string>();
  std::list <std::string>::iterator it, itEnd;
  Channel* chan=NULL;
  Player* mobile=(Player*)caller;
  world->GetChannelNames(names);

  itEnd = names->end();
  for (it = names->begin(); it != itEnd; ++it)
    {
      if (GetOptionValue((*it), mobile).GetInt() == 1)
        {
          chan=world->FindChannel((*it));
          if (chan)
            {
              chan->AddListener(mobile,true);
            }
        }
    }
  delete names;
}
EVENT(UnsubscribeChannels)
{
  World* world = World::GetPtr();

  std::list <std::string>* names=new std::list <std::string>();
  std::list <std::string>::iterator it, itEnd;
  Channel* chan=NULL;
  Player* mobile=(Player*)caller;
  world->GetChannelNames(names);

  itEnd=names->end();
  for (it = names->begin(); it != itEnd; ++it)
    {
      chan=world->FindChannel((*it));
      if (chan)
        {
          chan->RemoveListener(mobile,true);
        }
    }
  delete names;
}

Channel::Channel(const std::string &name,const std::string &alias,const FLAG access)
{
  _access=access;
  _name=name;
  _alias=alias;
  _listeners=new std::list <Player*>();
  _history=new std::list <HistoryNode*>();
  _pattern="[%N]: %P says, \'%M.\'";
}
Channel::~Channel(void)
{
  std::list <HistoryNode*>::iterator it, itEnd;
  if (_history->size())
    {
      itEnd = _history->end();
      for (it = _history->begin(); it != itEnd; ++it)
        {
          delete (*it);
        }
    }
  delete _history;

  delete _listeners;
}

void Channel::_AddHistoryEntry(const std::string &message)
{
  HistoryNode* node=NULL;

  if (message=="")
    {
      return;
    }

  if (_history->size()==MAX_CHAN_HISTORY_LENGTH)
    {
      node=_history->front();
      _history->pop_front();
      delete node;
      node=NULL;
    }

  node=new HistoryNode();
  node->when=time(NULL);
  node->message=message;
  _history->push_back(node);
}

std::string Channel::_Patternize(const std::string &message,Player* caller)
{
  std::string m=message;
  std::stringstream format;
  std::string::iterator it, itEnd;
  BOOL emote=false;
  if (m[0]==':')
    {
      emote=true;
      m.erase(m.begin());
    }
  if (emote)
    {
      itEnd = _epattern.end();
    }
  else
    {
      itEnd = _pattern.end();
    }
  for (it=_pattern.begin(); it != itEnd; ++it)
    {
      if (((*it)=='%')&&((it++)!=(emote?_epattern.end():_pattern.end())))
        {
          switch ((*it))
            {
            case 'p':
              format << caller->GetName();
              break;
            case 'P':
              format << Capitalize(caller->GetName());
              break;
            case 'n':
              format << _name;
              break;
            case 'N':
              format << Capitalize(_name);
              break;
            case 'm':
              format <<m;
              break;
            case 'M':
              format << Capitalize(m);
              break;
            case '%':
              format << "%";
              break;
            }
        }
      else
        {
          format << (*it);
        }
    }
  return format.str();
}

void Channel::SetName(const std::string &name)
{
  _name=name;
}
std::string Channel::GetName(void) const
{
  return _name;
}

void Channel::SetAlias(const std::string &alias)
{
  _alias=alias;
}
std::string Channel::GetAlias(void) const
{
  return _alias;
}

void Channel::SetPattern(const std::string &pattern)
{
  _pattern=pattern;
}
std::string Channel::GetPattern(void) const
{
  return _pattern;
}

void Channel::SetAccess(const FLAG access)
{
  _access=access;
}
FLAG Channel::GetAccess(void) const
{
  return _access;
}

std::list <HistoryNode*>* Channel::GetHistory(void) const
{
  return (_history->size()?_history:NULL);
}

void Channel::AddListener(Player* subscriber,BOOL quiet)
{
  std::list<Player*>::iterator it, itEnd;

  if (!subscriber->HasAccess(_access))
    {
      if (!quiet)
        {
          subscriber->Message(MSG_ERROR,"You don't have seficient permissions to access this channel.");
        }
      return;
    }

  itEnd = _listeners->end();
  for (it = _listeners->begin(); it != itEnd; ++it)
    {
      if ((*it)==subscriber)
        {
          if (!quiet)
            {
              subscriber->Message(MSG_ERROR,"You are already subscribed to that channel.");
            }
          return;
        }
    }

  _listeners->push_back(subscriber);
  if (!quiet)
    {
      subscriber->Message(MSG_INFO,"Tuned in.");
    }
}
void Channel::RemoveListener(Player* subscriber,BOOL quiet)
{
  std::list<Player*>::iterator it, itEnd;

  itEnd = _listeners->end();
  for (it = _listeners->begin(); it != itEnd; ++it)
    {
      if ((*it)==subscriber)
        {
          _listeners->remove(subscriber);
          if (!quiet)
            {
              subscriber->Message(MSG_INFO,"Tuned out.");
            }
          return;
        }
    }

  if (!quiet)
    {
      subscriber->Message(MSG_INFO,"You are not tuned into that channel.");
    }
}

void Channel::Broadcast(Player* caller,const std::string &message,BOOL access)
{
  std::list <Player*>::iterator it, itEnd;
  std::string paternized;

  if (access)
    {
      if (!caller->HasAccess(_access))
        {
          caller->Message(MSG_ERROR,"You don't have the access needed to be able to broadcast to this channel.");
          return;
        }
    }

  if (message=="")
    {
      caller->Message(MSG_ERROR,"You must provide a message.");
      return;
    }
  if (BitIsSet(caller->GetPflag(), PF_SILENCE))
    {
      caller->Message(MSG_ERROR, "You can not broadcast to channels while you are silenced.");
      return;
    }

  paternized=_Patternize(message,caller);

  itEnd = _listeners->end();
  for (it = _listeners->begin(); it != itEnd; ++it)
    {
      (*it)->Message(MSG_CHANNEL,paternized);
    }

  _AddHistoryEntry(paternized);
}
