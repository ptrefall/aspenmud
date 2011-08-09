#ifndef HELP_H
#define HELP_H
#include "../mud.h"
#include "../conf.h"

#ifdef MODULE_HELP
//we need OLC for this:
#ifndef OLC
#error OLC must be enabled for help to compile, or help must be disabled.
#endif

class CMDHelp:public Command
{
public:
  CMDHelp();
  BOOL Execute(const std::string &verb, Player* mobile,std::vector<std::string> &args,int subcmd);
};

class CMDAddTopic:public Command
{
public:
  CMDAddTopic();
  BOOL Execute(const std::string &verb, Player* mobile,std::vector<std::string> &args,int subcmd);
};
class CMDHedit:public Command
{
public:
  CMDHedit();
  BOOL Execute(const std::string &verb, Player* mobile,std::vector<std::string> &args,int subcmd);
};

#endif

BOOL InitializeHelp();
#endif
