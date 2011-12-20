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
#include <tinyxml.h>
#include "mud.h"
#include "conf.h"
#include "living.h"
#include "event.h"
#include "option.h"
#include "exception.h"

//exceptions:
class FileLoadException:public Exception
{
public:
  FileLoadException(const std::string &msg):Exception(msg) { }
};

class Player:public Living
{
  unsigned char *_password;
  unsigned char *_tempPassword;
  int _invalidPassword;
  std::string _title;
  std::string _prompt;
  int _level;
  FLAG _rank;
  FLAG _pflag;
  time_t _firstLogin;
  time_t _onlineTime;
  time_t _lastLogin;
  time_t _lastSave;
  time_t _lastBackup;
  std::map <std::string,OptionNode*> *_config;
  std::map <MessageType,std::string> *_messages;
  Socket *_sock;
  Player* _watching;
  std::list<Player*> *_watchers;
public:
  Player(void);
  ~Player(void);
  /*
  *Checks to see if the object is a player.
  *Return: True
  */
  BOOL IsPlayer() const;

  /*
  *Functions to load and retrieve player data from their file
  *param: [in] a boost archive to use for output.
  */
  void Serialize(TiXmlDocument* doc);
  void Deserialize(TiXmlElement* root);
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
  FLAG GetRank() const;
  void SetRank(const FLAG s);
  FLAG GetPflag() const;
  void SetPflag(FLAG flag);
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
  BOOL Save(BOOL force=false);
  BOOL Backup();
  void Load(void);
  /*
  *EnterGame and LeaveGame are called when the player enters or leaves the game, and should be used to initialize the player after this stage.
  *Param: [in] True if EnterGame shouldn't print messages (used for copyover), false otherwise (default).
  */
  void EnterGame(BOOL quiet = false);
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
  OptionNode* GetOption(const std::string &option) const;
  /*
  *Checks to see if the specified option exists on the player.
  *Param: [in] The name of the option.
  *Return: true if the option exists, false otherwise.
  */
  BOOL OptionExists(const std::string &option) const;
  /*
  *Toggles the specified option to on if it is off, or off if it is on.
  *Param: [in] The name of the option.
  */
  BOOL ToggleOption(const std::string &option);
  /*
  *Retrieves a pointer to the players configuration mapping.
  *Return: a pointer to a map of type <std::string,int>.
  */
  std::map<std::string, OptionNode*>* GetOptions(void) const;
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
//events
  EVENT(OnlineTime);
  EVENT(AutoSave);
};

#endif
