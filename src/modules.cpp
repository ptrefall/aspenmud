#include "modules.h"

BOOL InitializeModules(void)
{
#ifdef MODULE_SYSLOG
  if(!InitializeSyslog())
    {
      return false;
    }
#endif
#ifdef MODULE_HELP
  if(!InitializeHelp())
    {
      return false;
    }
#endif

  return true;
}
