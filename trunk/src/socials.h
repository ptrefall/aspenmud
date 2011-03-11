/*
*socials.h
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
*A basic socials system.
*Substitutions:
*All substitutions have an inverse capital form
*Using %T for example will return a capitalized form of the target's name.
*%a: adjective.
*%n: name of the calling player.
*%t: The target's name.
*%v: The name of the verb.
*/
#ifndef SOCIALS_H
#define SOCIALS_H
#include "mud.h"
#include "conf.h"
#include "player.h"
#include "command.h"

typedef struct
{
  std::string name; //the name of the social
  std::string ynotarg; //what you see when you smile without a target.
  std::string rnotarg; //what the room sees when the social doesn't have a target
  std::string ttarg; //what the target sees.
  std::string rtarg; //what the room sees when a target is provided.
  std::string ytarg; //what you see when you provide a target.
  VNUM id;
} SOCIAL_DATA;

class Socials
{
  VNUM _socid;
  std::map <std::string, SOCIAL_DATA*> *_slist;
  std::string _notargPattern;
  std::string _targPattern;
  static Socials* _ptr;
public:
  static Socials* GetPtr(void);
  void Initialize(void);
  ~Socials(void);
  void Save(void);
  void Load(void);
  /*
  *Checks to see if the specified social exists.
  *Param: [in] The name of the social.
  *Return: True if the social exists, false otherwise.
  */
  BOOL SocialExists(const std::string &name) const;
  /*
  *Adds a social to the list of current socials.
  *Param: [in] the name of the social to add.
  *[in] The message the player sees when no target is provided.
  *[in] The message the room sees when no target is provided.
  *[in] The message you see when a target is provided.
  *[in] The message the room sees when a target is provided.
  *[in] The message the target sees when a target was provided.
  */
  BOOL AddSocial(const std::string &name, const std::string &ynotarg, const std::string &rnotarg, const std::string &ytarg, const std::string &rtarg, const std::string &ttarg);
  /*
  *Retrieves the social's pointer
  *param: [in] The name of the social.
  *Return: A pointer to the SOCIAL_DATA struct, NULL if it doesn't exist.
  */
  SOCIAL_DATA* GetSocial(const std::string &name) const;
  /*
  *Formats the social into a string that can be sent to a room.
  *Param: [in] the social_data entry for the social.
  *[in] A pointer to the player who performed the social.
  *[in] A pointer to the target.
  */
//std::string FormatSocial(SOCIAL_DATA* soc, Player* caller, Living* target);
  /*
  *Initializes a few default socials.
  */
  void InitializeDefaultSocials(void);
  /*
  *Adds social commands to the command list.
  */
  void AddCommands(void);
};

EVENT(socials_shutdown);

class CMDSocials:public Command
{
public:
  CMDSocials();
public:
  BOOL Execute(const std::string &verb, Player* mobile,std::vector<std::string> &args,int subcmd);
};
BOOL InitializeSocials(void);
#endif
