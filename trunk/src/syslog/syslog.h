#ifndef SYSLOG_H
#define SYSLOG_H
#include "../mud.h"
#include "../conf.h"
#include "../event.h"
#include "../world.h"
#include "../channel.h"
#include "../player.h"

BOOL InitializeSyslog(void);
#ifdef MODULE_SYSLOG
EVENT(SYSLOG_PlayerConnect);
EVENT(SYSLOG_PlayerDisconnect);
#endif
#endif
