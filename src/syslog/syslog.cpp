#include <sstream>
#include "syslog.h"

BOOL InitializeSyslog()
{
#ifdef MODULE_SYSLOG
  World* world = World::GetPtr();

  world->WriteLog("Initializing syslog.");
  world->events.GetEvent("PlayerConnect")->Add(SYSLOG_PlayerConnect);
  world->events.GetEvent("PlayerDisconnect")->Add(SYSLOG_PlayerDisconnect);
#endif

  return true;
}

#ifdef MODULE_SYSLOG
EVENT(SYSLOG_PlayerConnect)
{
  World* world = World::GetPtr();

  Player* mobile=(Player*)caller;
  sockaddr_in* addr=mobile->GetSocket()->GetAddr();
  std::stringstream st;
  st << "Connected from: " << mobile->GetSocket()->GetHost() << " (" << inet_ntoa(addr->sin_addr) << ")";
  world->FindChannel("syslog")->Broadcast(mobile,st.str(),false);
  st.str("");
  st << mobile->GetName() << mobile->GetSocket()->GetHost() << " (" << inet_ntoa(addr->sin_addr) << ").";
  world->WriteLog(st.str(), CONNECTION);
}

EVENT(SYSLOG_PlayerDisconnect)
{
  World* world = World::GetPtr();

  Player* mobile=(Player*)caller;
  world->FindChannel("syslog")->Broadcast(mobile,"Disconnected",false);
  world->WriteLog(mobile->GetName()+" disconnected.");
}
#endif
