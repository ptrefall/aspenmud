/*
*event.cpp
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


#include <sys/time.h>
#include <string>
#include <list>
#include "event.h"
#include "exception.h"

Event::~Event()
{
}

BOOL Event::Add(const EVENTCB cb)
{
    _callbacks.push_back(cb);
    return true;
}
BOOL Event::Remove(const EVENTCB cb)
{
    _callbacks.remove(cb);
    return true;
}

BOOL Event::operator +=(const EVENTCB cb)
{
    Add(cb);
    return true;
}
BOOL Event::operator -=(const EVENTCB cb)
{
    Remove(cb);
    return true;
}

void Event::Invoke(EventArgs *args,void*caller)
{
    std::list <EVENTCB>::iterator it;
    for (it=_callbacks.begin(); it!=_callbacks.end(); it++) {
        (*it)(args,caller);
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
    std::list <EVENTCB>::iterator it;
    int secs; //seconds between last time and now
    int msecs; //microseconds between last time and now
    int felapse; //the full elapsed time that _fireTime requires.
    int celapse; //the time elapsed between now and last call
    gettimeofday(&curtime,NULL);
    secs=(int)(curtime.tv_sec-_lastTime.tv_sec)*1000;
    msecs=(int)(curtime.tv_usec-_lastTime.tv_usec)/1000;
    felapse=((_fireTime.tv_sec*1000)+(_fireTime.tv_usec/1000));
    celapse=msecs+secs;
    if (celapse>=felapse) { //the time between current time and last time elapsed is greater or equal to fire time
        for (it=_callbacks.begin(); it!=_callbacks.end(); it++) {
            (*it)(args,caller);
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
    for (it=_events.begin(); it!=_events.end(); it++) {
        delete (*it).second;
    }
}
BOOL EventManager::IsEventRegistered(const std::string &name)
{
    return (_events.count(name)==0?false:true);
}
Event* EventManager::GetEvent(const std::string &name)
{
    if (!IsEventRegistered(name)) {
        throw(EventNotFoundException("Couldn't get "+name+"."));
        return NULL;
    }
    return _events[name];
}
BOOL EventManager::RegisterEvent(const std::string &name, Event* event)
{
    if (IsEventRegistered(name)) {
        return false;
    }
    if (event==NULL) {
        return false;
    }

    _events[name]=event;
    return true;
}
BOOL EventManager::RemoveEvent(const std::string &name)
{
    if (IsEventRegistered(name)) {
        _events.erase(name);
        return true;
    }

    return false;
}
BOOL EventManager::CallEvent(const std::string &name, EventArgs* args, void* caller)
{
    if (!IsEventRegistered(name)) {
        throw(EventNotFoundException("Tried to call "+name+"."));
        return false;
    }
    if (_events[name]==NULL) {
//this should never be the case, but...
        return false;
    }

    (_events[name])->Invoke(args,caller);
    return true;
}
BOOL EventManager::AddCallback(const std::string &name, EVENTCB cb)
{
    if (!IsEventRegistered(name)) {
        throw(EventNotFoundException("Tried to call "+name+"."));
        return false;
    }

    (_events[name])->Add(cb);
    return true;
}
