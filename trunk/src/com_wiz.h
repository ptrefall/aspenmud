/*
*com_wiz.h
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

#ifndef COM_WIZ_H
#define COM_WIZ_H
#include "mud.h"
#include "conf.h"
#include "command.h"

//Command initialization
void InitializeWizCommands();

//Command declarations
class CMDCopyover:public Command
{
public:
  CMDCopyover();
  BOOL Execute(const std::string &verb, Player* mobile,std::vector<std::string> &args,int subcmd);
};
class CMDMkgod:public Command
{
public:
  CMDMkgod();
  BOOL Execute(const std::string &verb, Player* mobile,std::vector<std::string> &args,int subcmd);
};
class CMDMkbuilder:public Command
{
public:
  CMDMkbuilder();
  BOOL Execute(const std::string &verb, Player* mobile,std::vector<std::string> &args,int subcmd);
};
class CMDShutdown:public Command
{
public:
  CMDShutdown();
  BOOL Execute(const std::string &verb, Player* mobile,std::vector<std::string> &args,int subcmd);
};
#endif
