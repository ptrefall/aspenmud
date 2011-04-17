#ifndef HEADER_LOG
#define HEADER_LOG
#include <string>
#include <cstdio>
#include "mud.h"
#include "conf.h"

/*
*Logger class.
*Used to log messages to both console and files.
*/
class Log
{
  FILE* out;
public:
  Log();
  ~Log();
  /*
  *Opens a log file.
  *Params: [in] The name of the file to open.
  */
  void Open(std::string name);
  /*
  *Writes the data to the console and the file if possible.
  *Param: [in] The message to write.
  */
  void Write(const std::string &data,LOG_LEVEL l=INFORM);
};
#endif
