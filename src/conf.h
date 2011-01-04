/*
*conf.h
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


/**
*configuration defines for the mud
*Holds configuration and other constants.
*/

#ifndef CONFIG_H
#define CONFIG_H

//Mud configuration constants
#define DEFAULT_PORT 6666 //our default port: The mud will use this, if the port isn't provided as an argument.
#define LISTEN_BACKLOG 5 //the backlog for the listening socket.
#define PULSES_PER_SECOND 60 //how many game loop itterations will be ran each second
#define LOG_CONSOLE //will tell the logger to write to console. Comment out if you do not wish this.
#define MIN_LOG_LEVEL (LOG_LEVEL)WARN //the minimum level of logging that will be written to console.
#define MUD_NAME std::string("Falcon") //name of your mud.
#define MAX_CHAN_HISTORY_LENGTH 20 //the maximum number of entries channels will log.
#define GAME_IDLE_TIME (60*45)
#define LOGIN_IDLE_TIME (45)
/*
*Comment this out if you want all scripting functions enabled by default.
*Leaving it commented will remove functions like SetPassword on player from being exposed to scripting.
*/
//#define SCRIPT_SECURE
//delayed defines
#define LIVING_PULSE 2 //seconds for each living heartbeat to take place.
#define WORLD_PULSE 10 //the world pulse, used for room resets, etc.
//Defines for locating files:
#define MOTD_FILE "../txt/motd" //message of the day shown after login
#define LOGIN_FILE "../txt/welcome" //banner shown at login
#define COPYOVER_FILE "../copyover.dat" //stores player connection info
#define BIN_FILE "../bin/aspen" //used for copyovers.
#define EVENT_FILE "../logs/events.log" //our main log file
#define HELP_FILE "../data/help.dat"
#define SOCIALS_FILE "../data/socials.dat"
//directories:
#define PLAYER_DIR "../data/players/"
#define AREA_FILE "../data/area.dat"
//player rankings:
#define RANK_PLAYER 1<<0
#define RANK_PLAYTESTER 1<<1|RANK_PLAYER
#define RANK_NEWBIEHELPER 1<<2|RANK_PLAYER
#define RANK_BUILDER 1<<3|RANK_PLAYER
#define RANK_NEWBIE 1<<4|RANK_PLAYER
#define RANK_ADMIN 1<<30|RANK_BUILDER|RANK_NEWBIEHELPER|RANK_PLAYTESTER|RANK_PLAYER
#define RANK_GOD 1<<31|RANK_ADMIN|RANK_BUILDER|RANK_NEWBIEHELPER|RANK_PLAYTESTER|RANK_PLAYER

//positions:
enum POSITION {any, unconcious, sleeping, laying, resting, sitting, standing, flying};
//exit/room constants:
#define EXIT_NOWHERE -1
#define ROOM_NOWHERE -1
#define ROOM_START 1
//Direction defines for subcmd.
#define DIR_NORTH 1
#define DIR_SOUTH 2
#define DIR_EAST 3
#define DIR_WEST 4
#define DIR_NORTHEAST 5
#define DIR_NORTHWEST 6
#define DIR_SOUTHEAST 7
#define DIR_SOUTHWEST 8
#define DIR_UP 9
#define DIR_DOWN 10
//colors:
#define C_NORMAL "\x1B[0m"
#define C_RED "\x1B[31m"
#define C_GREEN "\x1B[32m"
#define C_YELLOW "\x1B[33m"
#define C_BLUE "\x1B[34m"
#define C_MAGENTA "\x1B[35m"
#define C_CYAN "\x1B[36m"
#define C_WHITE "\x1B[37m"
#define C_RESET "\033[0m"
//bold
#define CB_RED "\x1B[1;31m"
#define CB_GREEN "\x1B[1;32m"
#define CB_YELLOW "\x1B[1;33m"
#define CB_BLUE "\x1B[1;34m"
#define CB_MAGENTA "\x1B[1;35m"
#define CB_CYAN "\x1B[1;36m"
#define CB_WHITE "\x1B[1;37m"
//module defines
//below, define all the modules you want to be compiled in.
#define MODULE_SCRIPTING
#define MODULE_SYSLOG
#define MODULE_HELP
#define OLC
//if this is defined, the program will try to secure it's process environment when
//starting up. Leaving it defined can't hurt, but comment it out if this is giving you some sort of problem.
#define SECURE_INITIALIZATION
// conditional whether we are compiling for Windows or Unix
#if defined(_WIN32)
#define ASPEN_WIN32
#define ASPEN_WIN
#elif defined(_WIN64)
#define ASPEN_WIN64
#define ASPEN_WIN
#else
#define ASPEN_UNIX
#endif
#ifdef ASPEN_UNIX
#ifndef OPEN_MAX
#define OPEN_MAX 256
#endif
#endif

#endif

