/*
*command.h
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
*Classes for allowing objects to hold commands, as well as
*The actual command class, used for creating new commands.
*/
#ifndef COMMAND_H
#define COMMAND_H
#include <string>
#include <vector>
#include <list>
#include "mud.h"
#include "conf.h"
enum COMMAND_TYPE {movement, normal, script, social, channel, all};

class Command
{
protected:
    std::string _name;
    std::list<std::string> _aliases;
    int _subcmd;
    COMMAND_TYPE _type;
    FLAG _access;
    POSITION _position;
public:
    Command();
    virtual ~Command();
    void SetName(const std::string &name);
    std::string GetName() const;
    BOOL HasAlias(const std::string &name, BOOL partialMatch = false);
    BOOL AddAlias(const std::string &name);
    BOOL RemoveAlias(const std::string &name);
    void SetSubcmd(int subcmd);
    int GetSubcmd() const;
    void SetType(COMMAND_TYPE type);
    COMMAND_TYPE GetType() const;
    void SetAccess(FLAG access);
    FLAG GetAccess() const;
    void SetPosition(POSITION position);
    POSITION GetPosition() const;
    virtual BOOL Execute(const std::string &verb, Player* mobile,std::vector<std::string> &args,int subcmd) = 0;
    virtual BOOL CanExecute(Player* mobile, int subcmd);
};

/*
*Our commandable class
*Fascilitates adding and removing commands to an object.
*/
class Commandable
{
protected:
    std::vector <Command*> *_commands;
public:
    Commandable();
    ~Commandable();
    /*
    *Adds a command to the list of active commands:
    *Param: [in] A pointer to the command to add.
    */
    virtual void AddCommand(Command* com);
    /*
    *Removes the command from the list.
    *Param: [in] the name of the command.
    *Return: true if the command was found and removed, false otherwise.
    */
    virtual BOOL RemoveCommand(const std::string &name);
    /*
    *Checks to see if the specified command exists.
    *Param: [in] the name of the command.
    *Return: True if the command exists, false otherwise.
    */
    virtual BOOL CommandExists(const std::string &name);
    /*
    *Retrieves the names of the commands the player has access to.
    *Param: [out] A pointer to a list of strings that the function can fill in.
    *[in] The player object.
    */
    virtual void ListCommands(std::vector <std::string>* list,Player* mobile, COMMAND_TYPE filter=all);
};

/*
*Command initialization
*Param: [in] a pointer to the world object.
*/
void InitializeCommands();
#endif
