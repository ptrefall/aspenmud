#ifndef HELP_H
#define HELP_H
#include "../mud.h"
#include "../conf.h"
#include "../menu.h"
#include "../socket.h"

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

class CMDHedit:public Command
{
public:
  CMDHedit();
  BOOL Execute(const std::string &verb, Player* mobile,std::vector<std::string> &args,int subcmd);
  MENU(MainMenu);
  void CreateTopic(Socket* sock, const std::string& name);
  void RemoveTopic(Socket* sock, const std::string& name);
};
#endif

BOOL InitializeHelp();
#endif
