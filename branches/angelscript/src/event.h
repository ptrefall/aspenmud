#ifndef EVENT_H
#define EVENT_H
#include <sys/time.h>
#include <vector>
#include <map>
#include <string>
#include <boost/function.hpp>
#include "mud.h"
#include "conf.h"

/*
*EventArgs
*Any event passing args to it's caller will have a class that inherits this class
*/

class EventArgs
{
public:
  virtual ~EventArgs() {}
};

/*
*Our main event callback
*/
typedef void (*EVENTCB)(EventArgs*,void*);
typedef boost::function<void (EventArgs*, void*)> EVENTFUNC;

/*
*This holds our event function object, as well as an ID associated with each event.
*/
struct EventContainer
{
  EVENTFUNC cb;
  UINT id;
};

/*
*Event function declarations
*/
#define EVENT(name)\
void name(EventArgs* args, void* caller)
#define CEVENT(c, name)\
void c::name(EventArgs* args,void* caller)


/*
*Main event class.
*Events will be declared with this; any other types of events should inherit this class.
*/
class Event
{
protected:
  std::vector<EventContainer*> *_callbacks;
  UINT _id;
public:
  Event();
  virtual ~Event();
  /*
  *Add and remove a callback from the event list.
  */
  virtual BOOL operator +=(const EVENTFUNC cb);
  virtual BOOL operator -=(UINT id);
  virtual UINT Add(const EVENTFUNC cb);
  virtual BOOL Remove(UINT id);
  /*
  *Trigger all events in the callback list, passing args.
  *Params: [in] EventArgs specific to this event.
  *[in] A pointer to the caller object (if any).
  */
  virtual void Invoke(EventArgs *args,void* caller);
};

/*
*Delayed events
*These are used to make events go off on a certain amount of time, rather than
*When the invoke method is called one per pulse.
*/
class DelayedEvent:public Event
{
  timeval _fireTime; //how much time before we fire?
  timeval _lastTime; //when was the last time this event was fired?
public:
  /*
  *Constructor:
  *Params: [in] The number of seconds between each fire.
  *[in] The number of milliseconds between each fire.
  */
  DelayedEvent(int sec,int msec);
  DelayedEvent();
  /*
  *Sets the delay for the event.
  *Params: [in] The number of seconds between each fire.
  *[in] The number of milliseconds between each fire.
  */
  void SetDelay(int sec,int msec);
  /*
      *Trigger all events in the callback list, passing args.
  *Params: [in] EventArgs specific to this event.
  *[in] A pointer to the caller object (if any).
  */
  void Invoke(EventArgs *args,void* caller);
};

class EventManager
{
  std::map <std::string, Event*> _events;
public:
  EventManager(void);
  ~EventManager(void);
  /*
  *Checks to see if the event is registered.
  *Param: [in] the name of the event.
  *Return: True on success, false on failure.
  */
  BOOL IsEventRegistered(const std::string &name);
  /*
  *Retrieves an event
  *Param: [in] the name of the event.
  *Return: A pointer to the event, NULL if no event was found.
  */
  Event* GetEvent(const std::string &name);
  /*
  *Registers an event with the object.
  *Param: [in] the name of the event to register.
  *Param: [in] a pointer to the event object.
  *Return: True on success, false on failure.
  */
  BOOL RegisterEvent(const std::string &name,Event* event);
  /*
  *Removes the event from the events table.
  *Param: [in] the name of the event.
  *Return: True on success, false on failure.
  */
  BOOL RemoveEvent(const std::string &name);
  /*
  *Calls the event, passing in the caller and event args.
  *Param: [in] the name of the event.
  *[in] The event args.
  *[in] the event caller.
  *Return: True on success, false on failure.
  */
  BOOL CallEvent(const std::string &name, EventArgs* args, void* caller);
  /*
  *Adds a callback to the event.
  *Param: [in] the name of the event.
  *[in] the callback.
  */
  BOOL AddCallback(const std::string &name, EVENTFUNC callback);
};

#endif
