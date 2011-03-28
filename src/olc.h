/*
*Olc mechenism used for registering
*Certain objects with OLC.
*/
#ifndef OLC_H
#define OLC_H
#include <string>
#include "mud.h"
#include "conf.h"
#include "command.h"
#include "menu.h"
#include "editor.h"

#ifdef OLC
//input defines
#define OLC_INPUT(name)\
void name(Entity* ed, Player* mob, const Variant* input)

struct OLC_DATA
{
  std::string name; //the name of the option.
  std::string prompt;
  OLC_INPUT_TYPE type;
  FP_INPUT callback;
};

struct OLC_IN_DATA
{
  Entity* obj;
  OLC_DATA* olc;
  Player*mobile;
  MENU_DATA* menudata;
};

BOOL InitializeOlc(void);
class CMDOedit:public Command
{
public:
  CMDOedit();
  BOOL Execute(const std::string &verb, Player* mobile,std::vector<std::string> &args,int subcmd);
};
MENU(olc_menu_cb);
class OlcInput:public InputHandle
{
public:
  void Input(void* arg, const std::string &input);
};
#endif
#endif
