/*
*command.cpp
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

#include "command.h"
#include "world.h"
#include "com_gen.h"
#include "com_wiz.h"
#include "com_builder.h"
#include "com_movement.h"

Command::Command()
{
  _name = "";
  _subcmd=0;
  _type = normal;
  _access = RANK_PLAYER;
  _position = any;
}
Command::~Command()
{
}

void Command::SetName(const std::string &name)
{
  _name = name;
}
std::string Command::GetName() const
{
  return _name;
}
BOOL Command::AddAlias(const std::string &name)
{
  if (HasAlias(name))
    {
      return false;
    }

  _aliases.push_back(name);
  return true;
}
BOOL Command::RemoveAlias(const std::string &name)
{
  std::list <std::string>::iterator it;
  std::list <std::string>::iterator itEnd;
  itEnd=_aliases.end();

  for (it = _aliases.begin(); it != itEnd; ++it)
    {
      if ((*it) == name)
        {
          _aliases.remove((*it));
          return true;
        }
    }

  return false;
}
BOOL Command::HasAlias(const std::string &name, BOOL partialMatch)
{
  std::list <std::string>::iterator it;
  std::list <std::string>::iterator itEnd;
  itEnd=_aliases.end();

  for (it = _aliases.begin(); it != itEnd; ++it)
    {
      if ((*it) == name)
        {
          return true;
        }
      else if ((partialMatch) && (name.length() < (*it).length()) && ((*it).substr(name.length()) == name))
        {
          return true;
        }
    }

  return false;
}
void Command::SetSubcmd(int subcmd)
{
  _subcmd = subcmd;
}
int Command::GetSubcmd() const
{
  return _subcmd;
}
void Command::SetType(COMMAND_TYPE type)
{
  _type = type;
}
COMMAND_TYPE Command::GetType() const
{
  return _type;
}
void Command::SetAccess(FLAG access)
{
  _access = access;
}
FLAG Command::GetAccess() const
{
  return _access;
}
void Command::SetPosition(POSITION position)
{
  _position = position;
}
POSITION Command::GetPosition() const
{
  return _position;
}
BOOL Command::CanExecute(Player* mobile, int subcmd)
{
  return true;
}


Commandable::Commandable()
{
  _commands=new std::vector <Command*>();
}
Commandable::~Commandable()
{
  std::vector <Command*>::iterator comit;
  std::vector <Command*>::iterator comitEnd;

  comitEnd = _commands->end();
  for (comit = _commands->begin(); comit != comitEnd; ++comit)
    {
      delete (*comit);
    }
  delete _commands;
}

void Commandable::AddCommand(Command* com)
{
  if (CommandExists(com->GetName()))
    {
      return;
    }

  _commands->push_back(com);
}
BOOL Commandable::RemoveCommand(const std::string &name)
{
  if (!CommandExists(name))
    {
      return false;
    }

  std::vector <Command*>::iterator it; //an iterator for iterating through the command list
  std::vector <Command*>::iterator itEnd;

  itEnd = _commands->end();
  for (it = _commands->begin(); it != itEnd; ++it)
    {
      if ((*it)->GetName() == name)
        {
          _commands->erase(it);
          return true;
        }
    }
  return false;
}
BOOL Commandable::CommandExists(const std::string &name)
{
  std::vector <Command*>::iterator it; //an iterator for iterating through the command list
  std::vector <Command*>::iterator itEnd;

  itEnd = _commands->end();
  for (it = _commands->begin(); it != itEnd; ++it)
    {
      if ((*it)->GetName() == name)
        {
          return true;
        }
    }

  return false;
}
void Commandable::ListCommands(std::vector <std::string>* list, Player* mobile, COMMAND_TYPE filter)
{
  std::vector <Command*>::iterator it;
  std::vector <Command*>::iterator itEnd;

  itEnd = _commands->end();
  for (it = _commands->begin(); it != itEnd; ++it)
    {
      if (mobile->HasAccess((*it)->GetAccess()))
        {
          if (((*it)->GetType() == filter) || (filter == all))
            {
              list->push_back((*it)->GetName());
            }
        }
    }
}

std::vector <Command*> *Commandable::GetPtr()
{
  return _commands;
}


void InitializeCommands()
{
  world->WriteLog("Initializing commands.");
  InitializeGenCommands();
  InitializeMovementCommands();
  InitializeWizCommands();
  InitializeBuilderCommands();
}
