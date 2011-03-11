#include <cstdio>
#include <string>
#include <ctime>
#include <cstring>
#include "log.h"
Log::Log()
{
  out = NULL;
}
Log::~Log()
{
  if (out)
    {
      fclose(out);
      out = NULL;
    }
}

void Log::Open(std::string name)
{
  if (!out)
    {
      out = fopen(name.c_str(), "a");
    }
}

void Log::Write(const std::string &data,LOG_LEVEL l)
{
  time_t curtime;
  char* stime=NULL;
  std::string level;
  struct tm* tinfo;

  if (out)
    {
      stime=new char[32];
      time(&curtime);
      tinfo=localtime(&curtime);

      if (stime==NULL)
        {
          return;
        }

      memset(stime, 0, 32);
      strftime(stime,32,"%x %X",tinfo);

      switch (l)
        {
        case INFORM:
          level="[INFO] ";
          break;
        case WARN:
          level="[WARNING] ";
          break;
        case ERR:
          level="[ERROR] ";
          break;
        case CRIT:
          level="[CRITICAL ERROR] ";
          break;
        case SCRIPT:
          level="[SCRIPT] ";
          break;
        case PLAYER:
          level="[player]";
          break;
        case CONNECTION:
          level="[CONNECTION]";
          break;
        default:
          break;
        }

      fprintf(out, "%s %s: %s.\n", level.c_str(), stime, data.c_str());
#ifdef LOG_CONSOLE
      if (MIN_LOG_LEVEL>=l)
        {
          printf("%s %s: %s\n",level.c_str(),stime,data.c_str());
        }
#endif

      if (stime)
        {
          delete []stime;
        }
    }
}
