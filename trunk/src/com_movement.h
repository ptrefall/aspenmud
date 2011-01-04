/*
*com_movement.h
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

#ifndef COM_MOVEMENT_H
#define COM_MOVEMENT_H
#include "mud.h"
#include "conf.h"
#include "command.h"

void InitializeMovementCommands(void);
BOOL DoMove(const std::string &verb, Player* mobile,std::vector<std::string> &args,int subcmd);
class CMDNorth:public Command
{
public:
    CMDNorth();
    BOOL Execute(const std::string &verb, Player* mobile,std::vector<std::string> &args,int subcmd);
};
class CMDSouth:public Command
{
public:
    CMDSouth();
    BOOL Execute(const std::string &verb, Player* mobile,std::vector<std::string> &args,int subcmd);
};
class CMDEast:public Command
{
public:
    CMDEast();
    BOOL Execute(const std::string &verb, Player* mobile,std::vector<std::string> &args,int subcmd);
};
class CMDWest:public Command
{
public:
    CMDWest();
    BOOL Execute(const std::string &verb, Player* mobile,std::vector<std::string> &args,int subcmd);
};
class CMDNortheast:public Command
{
public:
    CMDNortheast();
    BOOL Execute(const std::string &verb, Player* mobile,std::vector<std::string> &args,int subcmd);
};
class CMDSoutheast:public Command
{
public:
    CMDSoutheast();
    BOOL Execute(const std::string &verb, Player* mobile,std::vector<std::string> &args,int subcmd);
};
class CMDSouthwest:public Command
{
public:
    CMDSouthwest();
    BOOL Execute(const std::string &verb, Player* mobile,std::vector<std::string> &args,int subcmd);
};
class CMDNorthwest:public Command
{
public:
    CMDNorthwest();
    BOOL Execute(const std::string &verb, Player* mobile,std::vector<std::string> &args,int subcmd);
};
class CMDUp:public Command
{
public:
    CMDUp();
    BOOL Execute(const std::string &verb, Player* mobile,std::vector<std::string> &args,int subcmd);
};
class CMDDown:public Command
{
public:
    CMDDown();
    BOOL Execute(const std::string &verb, Player* mobile,std::vector<std::string> &args,int subcmd);
};
#endif
