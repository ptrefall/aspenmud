#ifndef MODULES_H
#define MODULES_H
#include "mud.h"
#include "conf.h"
#ifdef MODULE_SYSLOG
#include "syslog/syslog.h"
#endif
void InitializeModules(void);
#endif
