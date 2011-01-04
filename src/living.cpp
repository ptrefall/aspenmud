/*
*living.cpp
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


#include "living.h"
#include "event.h"
#include "serializer.hpp"

Living::Living()
{
    RegisterEvent("HeartBeat", new DelayedEvent(LIVING_PULSE,0));
}
Living::~Living()
{
}

void Living::EnterGame()
{
}
void Living::LeaveGame()
{
}

void Living::Update()
{
    CallEvent("HeartBeat", NULL, (void*)this);
}

BOOL Living::IsNpc(void)
{
    return false;
}
BOOL Living::IsPlayer(void)
{
    return false;
}

void Living::Serialize(Serializer& ar)
{
    Entity::Serialize(ar);
}
void Living::Deserialize(Serializer& ar)
{
    Entity::Deserialize(ar);
}
