#include <openssl/sha.h>
#include <tinyxml.h>
#include <map>
#include <string>
#include <sstream>
#include <list>
#include <cstring>
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
    events.RegisterEvent("EnterGame", new Event());
    events.RegisterEvent("LeaveGame", new Event());

    events.AddCallback("HeartBeat", HB_OnlineTime);
    events.AddCallback("HeartBeat", HB_AUTOSAVE);
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

void Player::Serialize(TiXmlDocument* doc)
{
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
    std::map<std::string, Option>::iterator it;
    std::map<std::string, Option>::iterator itEnd = _config->end();
    if (_config->size()) {
//we serialize if there is actually something to serialize.
        for (it = _config->begin(); it != itEnd; ++it) {
            if ((!OptionExists((*it).first)) || (GetGlobalOption((*it).first)->GetData() == (*it).second.GetData())) {
                continue;
            }
            TiXmlElement* option = new TiXmlElement("option");
            option->SetAttribute("name", (*it).first.c_str());
            (*it).second.GetData().Serialize(option);
            options->LinkEndChild(option);
        }
    }
    root->LinkEndChild(options);

    root->SetAttribute("title", _title.c_str());
    root->SetAttribute("prompt", _prompt.c_str());
    root->SetAttribute("level", _level);
    root->SetAttribute("rank", _rank);

    Living::Serialize(root);
    doc->LinkEndChild(root);
}
void Player::Deserialize(TiXmlElement* root)
{
    int tmp = 0;
    TiXmlElement* password = NULL;
    TiXmlElement* tinfo = NULL;
    TiXmlElement* option = NULL;
    TiXmlElement* options = NULL;
    Variant* var = NULL;
    std::string name;

    if (!root) {
        throw(FileLoadException("Error loading file: player element was not found."));
    }

    password = root->FirstChild("password")->ToElement();
    if (!password) {
        throw(FileLoadException("Error loading file: password element was not found."));
    }
    memcpy(_password, password->Attribute("value"), SHA256_DIGEST_LENGTH);
    password->Attribute("invalid", &_invalidPassword);

    tinfo = root->FirstChild("timeinfo")->ToElement();
    if (!tinfo) {
        throw(FileLoadException("Could not find timeinfo element."));
    }
    tinfo->Attribute("firstLogin", &tmp);
    _firstLogin = tmp;
    tinfo->Attribute("onlineTime", &tmp);
    _onlineTime = tmp;
    tinfo->Attribute("lastLogin", &tmp);
    _lastLogin = tmp;

    options=root->FirstChild("options")->ToElement();
    if (!options) {
        throw(FileLoadException("Error: options node was not found."));
    }
//now we iterate through the options list, and pull in the options to deserialize.
    for (option = options->FirstChild()->ToElement(); option; option = option->NextSibling()->ToElement()) {
        name = option->Attribute("name");
        var = new Variant();
        var->Deserialize(option->FirstChild("variable")->ToElement());
        SetOption(name, *var);
    }

    _title = root->Attribute("title");
    _prompt = root->Attribute("prompt");
    root->Attribute("level", &_level);
    root->Attribute("rank", &_rank);

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
    TiXmlDocument doc;
    Serialize(&doc);
    doc.SaveFile((std::string(PLAYER_DIR)+GetName()).c_str());
}

void Player::Load(void)
{
    TiXmlDocument doc((std::string(PLAYER_DIR)+GetName()).c_str());
    if (!doc.LoadFile()) {
        throw(FileLoadException("Error loading "+(std::string(PLAYER_DIR)+GetName())+"."));
    }
    TiXmlElement* root = doc.FirstChild("player")->ToElement();
    Deserialize(root);
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
    world->events.CallEvent("PlayerConnect", NULL,this);
    events.CallEvent("EnterGame", NULL, this);
    Save();
}
void Player::LeaveGame()
{
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

    if (!person->variables.FindProperty("autosaves")) {
        person->variables.AddProperty("autosaves", Variant(saves));
        return;
    }

    saves = person->variables.GetPropertyRef("autosaves").GetInt();
    saves++;
    if (saves >= 150) {
        saves = 0;
        person->Save();
        person->Message(MSG_INFO, "Autosaving.");
    }
    person->variables.GetPropertyRef("autosaves").SetInt(saves);
}
