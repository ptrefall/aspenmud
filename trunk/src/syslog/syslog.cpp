/*
*syslog.cpp
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
