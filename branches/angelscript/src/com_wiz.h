#ifndef COM_WIZ_H
#define COM_WIZ_H
#include "mud.h"
#include "conf.h"
#include "command.h"

//Command initialization
void InitializeWizCommands();

//Command declarations
class CMDCopyover:public Command
{
public:
  CMDCopyover();
  BOOL Execute(const std::string &verb, Player* mobile,std::vector<std::string> &args,int subcmd);
};
class CMDMkgod:public Command
{
public:
  CMDMkgod();
  BOOL Execute(const std::string &verb, Player* mobile,std::vector<std::string> &args,int subcmd);
};
class CMDMkbuilder:public Command
{
public:
  CMDMkbuilder();
  BOOL Execute(const std::string &verb, Player* mobile,std::vector<std::string> &args,int subcmd);
};
class CMDShutdown:public Command
{
public:
  CMDShutdown();
  BOOL Execute(const std::string &verb, Player* mobile,std::vector<std::string> &args,int subcmd);
};
#endif
