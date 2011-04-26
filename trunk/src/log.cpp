#include <cstdio>
#include <string>
#include <sstream>
#include <ctime>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include "mud.h"
#include "conf.h"
#include "log.h"
#include "utils.h"

Log::Log(const std::string &file)
{
  _out = fopen(file.c_str(), "w");
  _name = file;
}
Log::~Log()
{
  if (_out)
    {
      fflush(_out);
      fclose(_out);
      _out = NULL;
    }
}

void Log::Write(const std::string &data,LOG_LEVEL l)
{
  time_t curtime;
  char* stime=NULL;
  std::string level;
  struct tm* tinfo;

  if (_out)
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

      fprintf(_out, "%s %s: %s\n", level.c_str(), stime, data.c_str());
#ifdef LOG_CONSOLE
      if (MIN_LOG_LEVEL>=l)
        {
          printf("%s %s: %s\n", level.c_str(),stime,data.c_str());
        }
#endif

      if (stime)
        {
          delete []stime;
        }

      CheckSize();
    }
}
void Log::CheckSize()
{
  UINT size = GetFileSize(fileno(_out));
  if (size >= LOG_MAXSIZE)
    {
      CycleFiles();
    }
}
void Log::CycleFiles()
{
  int top = 0;
  int counter = 0;
  std::stringstream st;
  std::stringstream st2;

//we need the highest file that exists.
  for (counter = 0; counter <= LOG_MAXFILES; counter++)
    {
      st << _name << counter;
      if (!FileExists(st.str()))
        {
          top = counter;
          break;
        }
    }
//the file will be renamed, so it needs closed.
  fflush(_out);
  fclose(_out);
//if we had the maximum amount of files, top will be equal to 0 and we will need to remove one.
  if (top == 0)
    {
      st.str("");
      st << _name << ".log." << LOG_MAXFILES;
      unlink(st.str().c_str());
      top = counter;
    }
//now we run backwards and rename:
  for (counter = top; counter > 0; counter++)
    {
      st.str("");
      st2.str("");
      st << _name << ".log." << counter-1;
      st2 << _name << ".log." << counter;

      rename(st.str().c_str(), st2.str().c_str());
    }

  rename(_name.c_str(), std::string(_name+".log.1").c_str());
  _out = fopen(_name.c_str(), "w");
}
