/*
*player.h
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
*player.h
*Contains the main player object prototypes.
*/

#ifndef PLAYER_H
#define PLAYER_H
#include <string>
#include <list>
#include <map>
#include <ctime>
#include "mud.h"
#include "conf.h"
#include "living.h"
#include "event.h"
#include "option.h"
#include "serializer.hpp"

//Message types:
enum MessageType {MSG_NORMAL, MSG_ERROR,MSG_INFO,MSG_CRITICAL,MSG_CHANNEL,MSG_LIST};
EVENT(HB_OnlineTime);
EVENT(HB_AUTOSAVE);

class Player:public Living
{
    unsigned char *_password;
    unsigned char *_tempPassword;
    int _invalidPassword;
    std::string _title;
    std::string _prompt;
    int _level;
    UINT _exp;
    int _rank;
    time_t _firstLogin;
    time_t _onlineTime;
    time_t _lastLogin;
    std::map <std::string,Option> *_config;
    std::map <MessageType,std::string> *_messages;
    Socket *_sock;
public:
    Player(void);
    ~Player(void);
    /*
    *Checks to see if the object is a player.
    *Return: True
    */
    BOOL IsPlayer(void);

    /*
    *Functions to load and retrieve player data from their file
    *param: [in] a boost archive to use for output.
    */
    void Serialize(Serializer& ar);
    void Deserialize(Serializer& ar);
    /*
    *Getters and setters
    *Exposes the properties that can be set on the player object.
    *Setters take the value to set.
    *Getters return the value.
    */
    void SetSocket(Socket* sock);
    std::string GetPassword(void) const;
    void SetPassword(const std::string &s);
    std::string GetTempPassword(void) const;
    void SetTempPassword(const std::string &s);
    /*
    *Increases the number of attempts that an invalid password was guessed.
    */
    void IncInvalidPassword(void);
    Socket* GetSocket(void);
    std::string GetTitle(void) const;
    void SetTitle(const std::string &s);
    UINT GetLevel(void) const;
    void SetLevel(UINT s);
    int GetRank(void) const;
    void SetRank(const int s);
    UINT GetOnlineTime(void) const;
    void SetOnlineTime(UINT s);
    UINT GetFirstLogin(void) const;
    void SetFirstLogin(UINT first);
    UINT GetLastLogin(void) const;
    void SetLastLogin(UINT last);
    std::string GetPrompt(void) const;
    void SetPrompt(const std::string &prompt);
    /*
    *Clears the temp password that was used to compare the password with the players.
    */
    void ClearTempPassword(void);
    /*
    *Makes a comparison of the password stored on the player object and the TempPassword.
    *Return: true if the match is complete, false otherwise.
    */
    BOOL ComparePassword(void);
    /*
    *Functions directly called to load and save the player object.
    */
    void Save(void);
    void Load(void);
    /*
    *EnterGame and LeaveGame are called when the player enters or leaves the game, and should be used to initialize the player after this stage.
    *Param: [in] True if EnterGame shouldn't print messages (used for copyover), false otherwise (default).
    */
    void EnterGame(BOOL quiet = true);
    void LeaveGame();
    /*
    *A low-level write, used for communicating with the socket.
    *Writes text to the player's socket buffer
    *Param: [in] A string of text.
    */
    void Write(const std::string &text) const;
    /*
    *Writes a message to the player
    *Params: [in] the type of message.
    *[in] The message
    */
    void Message(const MessageType type,const std::string &data) const;

    /*
    *Configuration management functions.
    *
    *Params: [in] The option to set.
    *[in] The value to set to the option.
    */
    void SetOption(const std::string &option, Variant &val);
    /*
    *Retrieves the specified option, if it exists.
    *Param: [in] The option to retrieve the value of.
    *Return: The value of the option, or -1 if it doesn't exist.
    */
    Option* GetOption(const std::string &option) const;
    /*
    *Checks to see if the specified option exists.
    *Param: [in] The name of the option.
    *Return: true if the option exists, false otherwise.
    */
    BOOL OptionExists(const std::string &option) const;
    /*
    *Toggles the specified option to on if it is off, or off if it is on.
    *Param: [in] The name of the option.
    */
    void ToggleOption(const std::string &option);
    /*
    *Retrieves a pointer to the players configuration mapping.
    *Return: a pointer to a map of type <std::string,int>.
    */
    std::map<std::string, Option>* GetOptions(void) const;

    /*
    *Checks to see if the player has the specified rank.
    *Param: [in] The rank to check for.
    *Return: True if the player has the specified access rank, false otherwise.
    */
    BOOL HasAccess(FLAG access) const;
    /*
    *Adds a message type to the player.
    *Param: [in] the type of the message.
    *[in] the color.
    */
    void AddMessage(MessageType type, const std::string &color);
};

#endif
