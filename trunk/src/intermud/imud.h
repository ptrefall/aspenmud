#ifndef IMUD_H
#define IMUD_H
#include "../mud.h"
#include "../conf.h"
#include "../baseSocket.h"

#ifdef MODULE_INTERMUD
#define IMUD_RECONNECT_INTERVAL 30 //30 seconds.
#define IMUD_KEEPALIVE_INTERVAL 60
#define IMUD_DATA_VERSION 5
#define IMUD_MAX_ERRCOUNT 3

class Intermud
{
  BaseSocket* sock;
public:
  Intermud();
  ~Intermud();
  void Log(const std::string &message, LOG_LEVEL level = INFORM);
  std::string SerializeString(const std::string &str);
  std::string SerializePacket(const std::string &type, const std::string &origuser, const std::string &targetmud, const std::string &targetuser, const std::string &message);
  void SendPacket(const std::string &type, const std::string &targetmud, const std::string &targetuser, const std::string &message, const std::string &sender="");
  void SendToRouter(const std::string &type, const std::string &message);
  void SendToMud(const std::string &type, const std::string &mud, const std::string &message);
  void SendToUser(const std::string &type, const std::string &mud, const std::string &user, const std::string &message);
  void SendToAll(const std::string &type, const std::string &message);
  void Initialize();
};
#endif
#endif
