#ifndef MODULES_H
#define MODULES_H
#include "mud.h"
#include "conf.h"
#ifdef MODULE_SYSLOG
#include "syslog/syslog.h"
#endif
#ifdef MODULE_HELP
#include "help/help.h"
#endif

BOOL InitializeModules(void);
#endif
