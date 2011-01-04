/*
*player.cpp
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


#include <openssl/sha.h>
#include <map>
#include <string>
#include <sstream>
#include <list>
#include <cstring>
#include "player.h"
#include "event.h"
#include "utils.h"
#include "world.h"
#include "serializer.hpp"
#include "exception.h"
#include <openssl/sha.h>

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
    _exp=0;
    _rank=RANK_PLAYER;
    _firstLogin=0;
    _onlineTime=0;
    _lastLogin=0;
//config defaults:
    _config=new std::map<std::string, Option>();
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
    RegisterEvent("EnterGame", new Event());
    RegisterEvent("LeaveGame", new Event());

    AddCallback("HeartBeat", HB_OnlineTime);
    AddCallback("HeartBeat", HB_AUTOSAVE);
}
Player::~Player()
{
    if (_password) {
        delete []_password;
        _password=NULL;
    }
    if (_tempPassword) {
        delete []_tempPassword;
        _tempPassword = NULL;
    }
    if (_config) {
        delete _config;
        _config=NULL;
    }
    if (_messages) {
        delete _messages;
        _messages=NULL;
    }
}

BOOL Player::IsPlayer(void)
{
    return true;
}

void Player::Serialize(Serializer& ar)
{
    Living::Serialize(ar);

    ar.WriteBytes((char*)_password, SHA256_DIGEST_LENGTH+1);
    ar << _invalidPassword;
    ar << _title;
    ar << _prompt;
    ar << _level;
    ar << _exp;
    ar << _rank;
    ar << _firstLogin;
    ar << _onlineTime;
    ar << _lastLogin;

//serialize their options:
    int count = 0;
    std::map<std::string, Option>::iterator it;
    std::map<std::string, Option>::iterator itEnd;
    itEnd = _config->end();
    for (it = _config->begin(); it != itEnd; ++it) {
//we only serialize if:
//1) the option exists. (This keeps us from having to clean out files if we remove a channel, etc).
//2) The options value is not equal to the default value of the registered option (keeps overhead minimal).
        if ((!OptionExists((*it).first)) || (GetGlobalOption((*it).first)->GetData() == (*it).second.GetData())) {
            continue;
        }
        count++;
    }
    ar << count;
    if (count) {
//we serialize if there is actually something to serialize.
        for (it = _config->begin(); it != itEnd; ++it) {
            if ((!OptionExists((*it).first)) || (GetGlobalOption((*it).first)->GetData() == (*it).second.GetData())) {
                continue;
            }
            ar << (*it).first;
            (*it).second.GetData().Serialize(ar);
        }
    }
}
void Player::Deserialize(Serializer& ar)
{
    Living::Deserialize(ar);

    ar.ReadBytes((char*)_password, SHA256_DIGEST_LENGTH+1);
    ar >> _invalidPassword;
    ar >> _title;
    ar >> _prompt;
    ar >> _level;
    ar >> _exp;
    ar >> _rank;
    ar >> _firstLogin;
    ar >> _onlineTime;
    ar >> _lastLogin;

//deserialize options.
    int count = 0;
    ar >> count;
    if (count) {
//we only do this if there's something to deserialize.
        Variant var;
        std::string name;
        int i=0;
        for (; i < count; ++i) {
            ar >> name;
            var.Deserialize(ar);
            SetOption(name, var);
        }
    }
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
    for (i = 0, p = _password, t = _tempPassword; i < SHA256_DIGEST_LENGTH; i++, p++, t++) {
        if (*p==*t)
            continue;
        failed = true;
    }
    return failed? 0:1;
}

void Player::IncInvalidPassword(void)
{
    _invalidPassword++;
    Save();
}

void Player::Save(void)
{
    FILE* output=fopen((std::string(PLAYER_DIR)+GetName()).c_str(), "wb");
    Serializer* s=new Serializer(output, WRITE);
    Serialize(*s);
    delete s;
}
void Player::Load(void)
{
    FILE* in=fopen((std::string(PLAYER_DIR)+GetName()).c_str(), "rb");
    try {
        Serializer* ar=new Serializer(in,READ);
        Deserialize(*ar);
        delete ar;
    } catch (Exception x) {
        printf("%s\n", x.GetMessage().c_str());
    }
}

void Player::EnterGame(BOOL quiet)
{
    Living::EnterGame();
//add the player to the users list:
    world->AddPlayer(this);
//move the player if it doesn't already have a location
    if (GetLocation()==NULL) {
        SetLocation(world->GetRoom(ROOM_START));
    }
//if there were password attempts, tell the player.
    if (_invalidPassword) {
        std::stringstream st;
        st << (char)7 << _invalidPassword << (_invalidPassword==1?" attempt was":" attempts were") << " made on your password.";
        Message(MSG_CRITICAL,st.str());
        _invalidPassword=0;
    }
    if (!quiet) {
//show the login banner:
        Write("\n"+std::string(world->GetMotd())+"\n");
    }
    world->CallEvent("PlayerConnect", NULL,this);
    CallEvent("EnterGame", NULL, this);
    Save();
}
void Player::LeaveGame()
{
    Save();
    Living::LeaveGame();
//take the player from the users list:
    world->RemovePlayer(this);
    world->CallEvent("PlayerDisconnect", NULL, this);
    CallEvent("LeaveGame", NULL,this);
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

int Player::GetRank(void) const
{
    return _rank;
}
void Player::SetRank(const int s)
{
    _rank=s;
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
    if (OptionExists(option)) {
        (*_config)[option].GetData().SetStr(val.GetStr());
    }
}
Option* Player::GetOption(const std::string &option) const
{
    if (OptionExists(option)) {
        return &(*_config)[option];
    } else {
        return GetGlobalOption(option);
    }
}

BOOL Player::OptionExists(const std::string &option) const
{
    return (_config->count(option))==0?false:true;
}
void Player::ToggleOption(const std::string &option)
{
    if (OptionExists(option)) {
        if ((*_config)[option].GetData().Typeof() == VAR_INT) {
            if ((*_config)[option].GetData().GetInt()) {
                (*_config)[option].GetData().SetInt(0);
            } else {
                (*_config)[option].GetData().SetInt(1);
            }
        }
    }
}
std::map<std::string, Option>* Player::GetOptions(void) const
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

EVENT(HB_OnlineTime)
{
    Player* person=(Player*)caller;
    person->SetOnlineTime(person->GetOnlineTime()+LIVING_PULSE);
}

EVENT(HB_AUTOSAVE)
{
    Player* person = (Player*)caller;
    int saves=0;

    if (!person->VarExists("autosaves")) {
        person->AddVar("autosaves", saves);
        person->SetPersistents("autosaves", false);
        return;
    }

    saves = person->GetInt("autosaves");
    saves++;
    if (saves >= 150) {
        saves = 0;
        person->Save();
        person->Message(MSG_INFO, "Autosaving.");
    }

    person->SetInt("autosaves", saves);
}
