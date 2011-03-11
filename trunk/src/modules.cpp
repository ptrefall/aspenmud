#include "modules.h"

void InitializeModules(void)
{
#ifdef MODULE_SYSLOG
  InitializeSyslog();
#endif
}
