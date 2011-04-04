#include "../mud.h"
#include "../conf.h"
#include "../world.h"
#include "imud.h"

#ifdef MODULE_INTERMUD
Intermud::Intermud()
{
sock = NULL;
}
Intermud::~Intermud()
{
if (sock)
{
sock->Close();
delete sock;
sock = NULL;
}
}

void Intermud::Log(const std::string &message, LOG_LEVEL level)
{
world->WriteLog("[INTERMUD]"+message, level);
}

std::string Intermud::SerializeString(const std::string &str)
{
std::string ret, pref, post, data;
std::string::iterator it, itEnd;
size_t index = str.length()-1; //the last character in the string.

if (!str.length())
{
return "\"\"";
}

if (str[0] == "\\")
{
pref = "\\\\";
}
else if (str[0] == "\"")
{
pref = "\\\"";
}

if (str[index] == "\\")
{
post = "\\\\";
}
else if(str[index] == "\"")
{
post = "\\\"";
}

data = str;
itEnd = data.end();
for (it = data.begin(); it != itEnd; ++it)
{
if (((*it) == '\\') || ((*it) == '\"'))
{
data.insert(it, '\\');
}
}

return "\""+pref+data+post+"\"";
}

std::string Intermud::SerializePacket(const std::string &type, const std::string &origuser, const std::string &targetmud, const std::string &targetuser, const std::string &message)
{
std::string packet = "({";

if (type == "")
{
packet +=  "0,";
}
else
{
packet += SerializeString(type)+",";
}
packet += "5,";
packet += SerializeString(MUD_NAME)+",";
if (origuser == "")
{
packet +=  "0,";
}
else
{
packet += SerializeString(origuser)+",";
}
if (targetmud == "")
{
packet +=  "0,";
}
else
{
packet += SerializeString(targetmud)+",";
}
if (targetuser == "")
{
packet +=  "0,";
}
else
{
packet += SerializeString(targetuser)+",";
}
if (message == "")
{
packet += "0,";
}
else
{
packet+=SerializeString(message)+",";
}
packet+="})";
return packet;
}

void Intermud::SendPacket(const std::string &type, const std::string &targetmud, const std::string &targetuser, const std::string &sender, const std::string &message)
{
char* buffer = NULL;
char* p = NULL;
char* arr = NULL;
size_t length = 0;
std::string packet = SerializePacket(type, sender, targetmud, targetuser, message);

length = packet.length();
ar = (char*)&length;
buffer = new char[length+5];

p = buffer + 4;
memset(buffer, 0, length+5);
memcpy(buffer, arr, 4);
strcpy(p, packet.c_str());
sock->Write(buffer, length+5);
}
void Intermud::SendToRouter(const std::string &type, const std::string &message)
{
SendPacket(type, "*wpr", "", message);
}
void Intermud::SendToMud(const std::string &type, const std::string &mud, const std::string &message)
{
SendPacket(type, mud, "", message);
}
void Intermud::SendToUser(const std::string &type, const std::string &mud, const std::string &user, const std::string &message)
{
SendPacket(type, mud, user, message);
}
void Imud::SendToAll(const std::string &type, const std::string &message)
{
SendPacket(type, "", "", message);

void Intermud::Initialize()
{
if (sock)
{
sock->Close();
delete sock;
sock = NULL;
}

sock = new BaseSocket();
sock->Connect("195.242.99.94", 8080);
}

#endif
