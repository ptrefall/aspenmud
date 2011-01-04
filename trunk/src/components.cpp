/*
*components.cpp
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


/*
*This is a bit of a messy way to set this up, but it works.
*Include the header file for every component here, then register it.
*/
#include "world.h"
#include "component.h"

//start components include
#include "components/stats.h"

void CreateComponents(void)
{
    world->WriteLog("Initializing components.");
    world->RegisterComponent("stats", STATS_CREATE, STATS_INIT);
}
