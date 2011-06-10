#include <openssl/sha.h>
#include <tinyxml.h>
#include <map>
#include <string>
#include <sstream>
#include <list>
#include <cstring>
#include <boost/bind.hpp>
#include "player.h"
#include "event.h"
#include "utils.h"
#include "world.h"
#include "exception.h"

Player::Player()
{
  _password =new unsigned char[SHA256_DIGEST_LENGTH+1];
  _tempPassword = new unsigned char[SHA256_DIGEST_LENGTH+1];
  memset(_password, 0, SHA256_DIGEST_LENGTH+1);
  memset(_tempPassword, 0, SHA256_DIGEST_LENGTH+1);
  _invalidPassword=0;
  _prompt=">";
  _title="the brave";
  _level=1;
  _rank = BitSet(0, RANK_PLAYER);
  _flag = 0;
  _firstLogin=0;
  _onlineTime=0;
  _lastLogin=0;
  _lastSave = 0;
  _lastBackup = 0;
//config defaults:
  _config=new std::map<std::string, OptionNode*>();

//messages:
  _messages=new std::map<MessageType,std::string>();
  AddMessage(MSG_ERROR, C_RED);
  AddMessage(MSG_INFO, C_BLUE);
  AddMessage(MSG_CRITICAL, CB_CYAN);
  AddMessage(MSG_CHANNEL, C_YELLOW);
  AddMessage(MSG_LIST, C_CYAN);

#ifdef OLC
  RemoveOlc("name");
#endif

//events
  events.RegisterEvent("EnterGame", new Event());
  events.RegisterEvent("LeaveGame", new Event());

  events.AddCallback("HeartBeat", boost::bind(&Player::OnlineTime, this, _1, _2));
  events.AddCallback("HeartBeat", boost::bind(&Player::AutoSave, this, _1, _2));
}
Player::~Player()
{
  std::map<std::string, OptionNode*>::iterator oit, oitEnd;

  if (_password)
    {
      delete []_password;
      _password=NULL;
    }
  if (_tempPassword)
    {
      delete []_tempPassword;
      _tempPassword = NULL;
    }
  if (_messages)
    {
      delete _messages;
      _messages=NULL;
    }

  oitEnd = _config->end();
  for (oit = _config->begin(); oit != oitEnd; ++oit)
    {
      delete (*oit).second;
    }
  if (_config)
    {
      delete _config;
      _config=NULL;
    }
}

BOOL Player::IsPlayer(void)
{
  return true;
}

void Player::Serialize(TiXmlDocument* doc)
{
  World* world = World::GetPtr();

  TiXmlDeclaration *decl = new TiXmlDeclaration("1.0", "", "");
  doc->LinkEndChild(decl);
  TiXmlElement* root = new TiXmlElement("player");

//password info
  TiXmlElement* password = new TiXmlElement("password");
  password->SetAttribute("value", (char*)_password);
  password->SetAttribute("invalid", _invalidPassword);
  root->LinkEndChild(password);

//time info
  TiXmlElement* timeinfo = new TiXmlElement("timeinfo");
  timeinfo->SetAttribute("firstLogin", _firstLogin);
  timeinfo->SetAttribute("onlineTime", _onlineTime);
  timeinfo->SetAttribute("lastLogin", _lastLogin);
  root->LinkEndChild(timeinfo);

//options
  TiXmlElement* options = new TiXmlElement("options");
  std::map<std::string, OptionNode*>::iterator it, itEnd;
  OptionNode* onode = NULL;
  itEnd = _config->end();
  if (_config->size())
    {
//we serialize if there is actually something to serialize.
      for (it = _config->begin(); it != itEnd; ++it)
        {
          onode =  (*it).second;
          if (onode->_data == world->GetGlobalOption((*it).first)->GetData())
            {
              continue;
            }
          TiXmlElement* option = new TiXmlElement("option");
          option->SetAttribute("name", (*it).first.c_str());
          onode->_data.Serialize(option);
          options->LinkEndChild(option);
        }
    }
  root->LinkEndChild(options);

  root->SetAttribute("title", _title.c_str());
  root->SetAttribute("prompt", _prompt.c_str());
  root->SetAttribute("level", _level);
  root->SetAttribute("rank", _rank);
  root->SetAttribute("flag", _flag);
  Living::Serialize(root);
  doc->LinkEndChild(root);
}
void Player::Deserialize(TiXmlElement* root)
{
  World* world = World::GetPtr();
  int tmp = 0;
  OptionNode* onode = NULL;
  Option* opt = NULL;
  TiXmlElement* password = NULL;
  TiXmlElement* tinfo = NULL;
  TiXmlElement* option = NULL;
  TiXmlElement* options = NULL;
  TiXmlNode* node = NULL;
  Variant var;
  std::string name;

  if (!root)
    {
      throw(FileLoadException("Error loading file: player element was not found."));
    }

  node = root->FirstChild("password");
  if (!node)
    {
      throw(FileLoadException("Error loading file: password element was not found."));
    }
  password = node->ToElement();
  memcpy(_password, password->Attribute("value"), SHA256_DIGEST_LENGTH);
  password->Attribute("invalid", &_invalidPassword);

  node = root->FirstChild("timeinfo");
  if (!node)
    {
      throw(FileLoadException("Could not find timeinfo element."));
    }
  tinfo = node->ToElement();
  tinfo->Attribute("firstLogin", &tmp);
  _firstLogin = tmp;
  tinfo->Attribute("onlineTime", &tmp);
  _onlineTime = tmp;
  tinfo->Attribute("lastLogin", &tmp);
  _lastLogin = tmp;

  node = root->FirstChild("options")->ToElement();
  if (!node)
    {
      throw(FileLoadException("Error: options node was not found."));
    }
  options = node->ToElement();
//now we iterate through the options list, and pull in the options to deserialize.
  for (node = options->FirstChild(); node; node = node->NextSibling())
    {
      onode = new OptionNode();
      option = node->ToElement();
      name = option->Attribute("name");
      opt = world->GetGlobalOption(name);
      if (opt)
        {
          onode = new OptionNode();
          var.Deserialize(option->FirstChild("variable")->ToElement());
          onode->_data = var;
          onode->_option = opt;
          (*_config)[name] = onode;
        }
    }

  _title = root->Attribute("title");
  _prompt = root->Attribute("prompt");
  root->Attribute("level", &_level);
  root->Attribute("rank", &_rank);
  root->Attribute("flag", &_flag);
  Living::Deserialize(root->FirstChild("living")->ToElement());
}

void Player::SetSocket(Socket* sock)
{
  _sock = sock;
}

std::string Player::GetPassword() const
{
  return std::string((char*)_password);
}
void Player::SetPassword(const std::string &s)
{
  memset(_password, 0, SHA256_DIGEST_LENGTH+1);
  SHA256((unsigned char*)s.c_str(), s.length(), _password);
}

std::string Player::GetTempPassword(void) const
{
  return std::string((char*)_tempPassword);
}
void Player::SetTempPassword(const std::string &s)
{
  memset(_tempPassword, 0, SHA256_DIGEST_LENGTH+1);
  SHA256((unsigned char*)s.c_str(), s.length(), _tempPassword);
}
void Player::ClearTempPassword(void)
{
  memset(_tempPassword, 0, SHA256_DIGEST_LENGTH+1);
}
BOOL Player::ComparePassword(void)
{
  int i=0;
  unsigned char* p;
  unsigned char* t;
  BOOL failed = false;
  for (i = 0, p = _password, t = _tempPassword; i < SHA256_DIGEST_LENGTH; i++, p++, t++)
    {
      if (*p==*t)
        continue;
      failed = true;
    }
  return failed? 0:1;
}

void Player::IncInvalidPassword(void)
{
  _invalidPassword++;
  Save(true);
}

BOOL Player::Save(BOOL force)
{
  if (!force)
    {
      if ((time(NULL)-_lastSave) < SAVE_INTERVAL)
        {
          return false;
        }

      _lastSave = time(NULL);
    }

  TiXmlDocument doc;
  Serialize(&doc);
  doc.SaveFile((std::string(PLAYER_DIR)+GetName()).c_str());
  return true;
}
BOOL Player::Backup()
{
  if ((time(NULL)-_lastBackup) < BACKUP_INTERVAL)
    {
      return false;
    }

  TiXmlDocument doc;
  Serialize(&doc);
  doc.SaveFile((std::string(BACKUP_DIR)+GetName()).c_str());
  return true;
}
void Player::Load(void)
{
  TiXmlDocument doc((std::string(PLAYER_DIR)+GetName()).c_str());
  if (!doc.LoadFile())
    {
      throw(FileLoadException("Error loading "+(std::string(PLAYER_DIR)+GetName())+"."));
    }
  TiXmlElement* root = doc.FirstChild("player")->ToElement();
  Deserialize(root);
}

void Player::EnterGame(BOOL quiet)
{
  World* world = World::GetPtr();

  Living::EnterGame();
//add the player to the users list:
  world->AddPlayer(this);
//move the player if it doesn't already have a location
  if (GetLocation()==NULL)
    {
      SetLocation(world->GetRoom(ROOM_START));
    }
//if there were password attempts, tell the player.
  if (_invalidPassword)
    {
      std::stringstream st;
      st << (char)7 << _invalidPassword << (_invalidPassword==1?" attempt was":" attempts were") << " made on your password.";
      Message(MSG_CRITICAL,st.str());
      _invalidPassword=0;
    }
  if (!quiet)
    {
//show the login banner:
      Write("\n"+std::string(world->GetMotd())+"\n");
    }
  world->events.CallEvent("PlayerConnect", NULL,this);
  events.CallEvent("EnterGame", NULL, this);
  Save();
}
void Player::LeaveGame()
{
  World* world = World::GetPtr();

  Save();
  Living::LeaveGame();
//take the player from the users list:
  world->RemovePlayer(this);
  world->events.CallEvent("PlayerDisconnect", NULL, this);
  events.CallEvent("LeaveGame", NULL,this);
}

void Player::Write(const std::string &text) const
{
  if (_sock)
    _sock->Write(text);
}

void Player::Message(const MessageType type,const std::string &data) const
{
  Write((*_messages)[type]+data+C_NORMAL+"\n");
}

Socket* Player::GetSocket(void)
{
  return _sock;
}

std::string Player::GetTitle(void) const
{
  return _title;
}
void Player::SetTitle(const std::string &s)
{
  _title=s;
}

UINT Player::GetLevel(void) const
{
  return _level;
}
void Player::SetLevel(const UINT l)
{
  _level=l;
}

FLAG Player::GetRank() const
{
  return _rank;
}
void Player::SetRank(const FLAG s)
{
  _rank=s;
}

FLAG Player::GetFlag() const
{
  return _flag;
}
void Player::SetFlag(FLAG flag)
{
  _flag = flag;
}

UINT Player::GetOnlineTime(void) const
{
  return (UINT)_onlineTime;
}
void Player::SetOnlineTime(UINT s)
{
  _onlineTime=(time_t)s;
}

UINT Player::GetFirstLogin(void) const
{
  return (UINT)_firstLogin;
}
void Player::SetFirstLogin(UINT first)
{
  _firstLogin=(time_t)first;
}

UINT Player::GetLastLogin(void) const
{
  return (time_t) _lastLogin;
}
void Player::SetLastLogin(UINT last)
{
  _lastLogin=(time_t)last;
}

std::string Player::GetPrompt(void) const
{
  return _prompt;
}
void Player::SetPrompt(const std::string &prompt)
{
  _prompt=prompt;
}

void Player::SetOption(const std::string &option, Variant &val)
{
  World* world = World::GetPtr();

  Option* opt;
  if (OptionExists(option))
    {
      if ((*_config)[option]->_option->GetData().Typeof() == val.Typeof())
        {
          (*_config)[option]->_option->GetData().SetStr(val.GetStr());
        }
    }
//checks for the global existance of the option.
  if (world->OptionExists(option))
    {
      opt = world->GetGlobalOption(option);
      if (opt->GetData().Typeof() == val.Typeof())
        {
          OptionNode* node = new OptionNode();
          node->_data = val;
          node->_option = opt;
          (*_config)[option] = node;
        }
    }
}
OptionNode* Player::GetOption(const std::string &option) const
{
  if (OptionExists(option))
    {
      return (*_config)[option];
    }
  else
    {
      return NULL;
    }
}

BOOL Player::OptionExists(const std::string &option) const
{
  return (_config->count(option))==0?false:true;
}
void Player::ToggleOption(const std::string &option)
{
  if (OptionExists(option))
    {
      if ((*_config)[option]->_data.Typeof() == VAR_INT)
        {
          if ((*_config)[option]->_data.GetInt())
            {
              (*_config)[option]->_data.SetInt(0);
            }
          else
            {
              (*_config)[option]->_data.SetInt(1);
            }
        }
    }
}
std::map<std::string, OptionNode*>* Player::GetOptions(void) const
{
  return _config;
}

BOOL Player::HasAccess(FLAG access) const
{
  return BitIsSet(_rank,access);
}
void Player::AddMessage(MessageType type, const std::string &color)
{
  (*_messages)[type]=color;
}

CEVENT(Player, OnlineTime)
{
  SetOnlineTime(GetOnlineTime()+LIVING_PULSE);
}

CEVENT(Player, AutoSave)
{
  int saves=0;

  if (!variables.FindProperty("autosaves"))
    {
      variables.AddProperty("autosaves", Variant(saves));
      return;
    }

  saves = variables.GetPropertyRef("autosaves").GetInt();
  saves++;
  if (saves >= 150)
    {
      saves = 0;
      Save();
      Message(MSG_INFO, "Autosaving.");
    }
  variables.GetPropertyRef("autosaves").SetInt(saves);
}
