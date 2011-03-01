#include <sstream>
#include "syslog.h"

void InitializeSyslog(void)
{
#ifdef MODULE_SYSLOG
    world->WriteLog("Initializing syslog.");
    world->events.GetEvent("PlayerConnect")->Add(SYSLOG_PlayerConnect);
    world->events.GetEvent("PlayerDisconnect")->Add(SYSLOG_PlayerDisconnect);
    world->AddChannel(new Channel("syslog","",RANK_ADMIN),false);
#endif
    return;
}

#ifdef MODULE_SYSLOG
EVENT(SYSLOG_PlayerConnect)
{
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
    Player* mobile=(Player*)caller;
    world->FindChannel("syslog")->Broadcast(mobile,"Disconnected",false);
    world->WriteLog(mobile->GetName()+" disconnected.");
}
#endif
