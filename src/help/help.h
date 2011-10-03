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
//this is our main menu callback.
  MENU(MainMenu);
//the callback called when the user chooses create topic from the main menu.
  void CreateTopic(Socket* sock, const std::string& name);
//the callback that gets called when the user chooses remove topic from the main menu.
  void RemoveTopic(Socket* sock, const std::string& name);
//the callback that gets called when the user does not specify an argument to hedit, but rather uses the edit entry item in the main menu.
  void TopicName(Socket* sock, const std::string &name);
//the function that gets called when the user specifies a topic to edit, either through the main menu or as an argument to hedit
  void EditTopic(Player* mobile, const std::string &name);
//cleans up the data structure we pass around for the edit menu.
  void EditMenuCleanup(Menu* m);
//cleans up the data structure we pass around for the see also menu removal.
  void SeeAlsoMenuCleanup(Menu* m);
//The callback for the topic edit menu
  MENU(EditMenu);
//the SeeAlso removal menu:
  MENU(RemoveSeeAlso);
//event editor load/save functions.
  EVENT(EditorLoad);
  EVENT(EditorSave);
  void AddSeealso(Socket* sock, std::string input, void* data);
};

EVENT(CleanupHelp);
#endif

BOOL InitializeHelp();
#endif
