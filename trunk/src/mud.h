/*
*mud.h
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


//mud constants and values:
#ifndef HEADER_MUD
#define HEADER_MUD
#include "conf.h"
//used to show where an exception was thrown:
#define WHERE() std::string(" In ")+std::string(__FUNCTION__)+std::string(" at ")+std::string(__FILE__)+std::string(".")
//logger levels
enum LOG_LEVEL {INFORM,WARN,ERR,CRIT,SCRIPT, PLAYER, CONNECTION};

//typedefs:
typedef int FLAG; //bitfield flags
#ifndef ASPEN_WIN
typedef unsigned int DWORD;
typedef long int WORD;
typedef int BOOL;
typedef unsigned int UINT;
#endif
typedef int VNUM;

//forward declarations
//I wanted to avoid these, but it seems I have no choice
class World;
class Entity;
class Socket;
class Player;
class Component;
class Variant;

extern World* world;
//olc forward declarations
struct OLC_DATA;
enum OLC_INPUT_TYPE {NUM, STRING};
typedef void (*FP_INPUT)(Entity*, Player*, const Variant*);
#endif
