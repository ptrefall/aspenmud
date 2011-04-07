#include <sys/time.h>
#include <string>
#include <boost/function.hpp>
#include <vector>
#include "event.h"
#include "exception.h"

Event::Event()
{
  _id = 0;
  _callbacks = new std::vector<EventContainer*>();
}
Event::~Event()
{
  std::vector<EventContainer*>::iterator it, itEnd;

  itEnd = _callbacks->end();
  for (it = _callbacks->begin(); it != itEnd; ++it)
    {
      delete (*it);
    }
  delete _callbacks;
  _callbacks = NULL;
}

UINT Event::Add(const EVENTFUNC cb)
{
  EventContainer* c = new EventContainer();
  if (!c)
    {
      return 0;
    }

  _id++;
  c->id = _id;
  c->cb = cb;
  _callbacks->push_back(c);
  return c->id;
}
BOOL Event::Remove(UINT id)
{
  std::vector<EventContainer*>::iterator it, itEnd;

  itEnd = _callbacks->end();
  for (it = _callbacks->begin(); it != itEnd; ++it)
    {
      if (id == (*it)->id)
        {
          _callbacks->erase(it);
          return true;
        }
    }

  return false;
}

BOOL Event::operator +=(const EVENTFUNC cb)
{
  Add(cb);
  return true;
}
BOOL Event::operator -=(UINT id)
{
  Remove(id);
  return true;
}

void Event::Invoke(EventArgs *args,void*caller)
{
  std::vector <EventContainer*>::iterator it, itEnd;

  itEnd = _callbacks->end();
  for (it = _callbacks->begin(); it != itEnd; ++it)
    {
      (*it)->cb(args,caller);
    }
}

DelayedEvent::DelayedEvent(int sec,int msec)
{
  SetDelay(sec,msec);
}

DelayedEvent::DelayedEvent()
{
  SetDelay(0,0);
}

void DelayedEvent::SetDelay(int sec,int msec)
{
  msec*=1000;
  _fireTime.tv_sec=sec;
  _fireTime.tv_usec=msec;
  gettimeofday(&_lastTime,NULL);
}

void DelayedEvent::Invoke(EventArgs *args,void* caller)
{
  timeval curtime;
  std::vector <EventContainer*>::iterator it, itEnd;
  int secs; //seconds between last time and now
  int msecs; //microseconds between last time and now
  int felapse; //the full elapsed time that _fireTime requires.
  int celapse; //the time elapsed between now and last call

  gettimeofday(&curtime,NULL);
  secs=(int)(curtime.tv_sec-_lastTime.tv_sec)*1000;
  msecs=(int)(curtime.tv_usec-_lastTime.tv_usec)/1000;
  felapse=((_fireTime.tv_sec*1000)+(_fireTime.tv_usec/1000));
  celapse=msecs+secs;

  if (celapse>=felapse)   //the time between current time and last time elapsed is greater or equal to fire time
    {
      itEnd = _callbacks->end();
      for (it = _callbacks->begin(); it != itEnd; ++it)
        {
          (*it)->cb(args,caller);
        }
//update the last time the event was fired:
      gettimeofday(&_lastTime,NULL);
    }
}

EventManager::EventManager(void)
{
}
EventManager::~EventManager(void)
{
  std::map<std::string, Event*>::iterator it;
  for (it=_events.begin(); it!=_events.end(); it++)
    {
      delete (*it).second;
    }
}
BOOL EventManager::IsEventRegistered(const std::string &name)
{
  return (_events.count(name)==0?false:true);
}
Event* EventManager::GetEvent(const std::string &name)
{
  if (!IsEventRegistered(name))
    {
      throw(EventNotFoundException("Couldn't get "+name+"."));
      return NULL;
    }
  return _events[name];
}
BOOL EventManager::RegisterEvent(const std::string &name, Event* event)
{
  if (IsEventRegistered(name))
    {
      return false;
    }
  if (event==NULL)
    {
      return false;
    }

  _events[name]=event;
  return true;
}
BOOL EventManager::RemoveEvent(const std::string &name)
{
  if (IsEventRegistered(name))
    {
      _events.erase(name);
      return true;
    }

  return false;
}
BOOL EventManager::CallEvent(const std::string &name, EventArgs* args, void* caller)
{
  if (!IsEventRegistered(name))
    {
      throw(EventNotFoundException("Tried to call "+name+"."));
      return false;
    }
  if (_events[name]==NULL)
    {
//this should never be the case, but...
      return false;
    }

  (_events[name])->Invoke(args,caller);
  return true;
}
BOOL EventManager::AddCallback(const std::string &name, EVENTFUNC cb)
{
  if (!IsEventRegistered(name))
    {
      throw(EventNotFoundException("Tried to call "+name+"."));
      return false;
    }

  (_events[name])->Add(cb);
  return true;
}
