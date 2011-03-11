#ifndef COM_BUILDER_H
#define COM_BUILDER_H
#include "mud.h"
#include "conf.h"
#include "command.h"

//Command initialization
void InitializeBuilderCommands(void);

class CMDZlist:public Command
{
public:
  CMDZlist();
  BOOL Execute(const std::string &verb, Player* mobile,std::vector<std::string> &args,int subcmd);
};

class CMDRlist:public Command
{
public:
  CMDRlist();
  BOOL Execute(const std::string &verb, Player* mobile,std::vector<std::string> &args,int subcmd);
};

class CMDDig:public Command
{
public:
  CMDDig();
  BOOL Execute(const std::string &verb, Player* mobile,std::vector<std::string> &args,int subcmd);
};
#endif
