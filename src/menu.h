/*
*A system for handling and controling menus.
*/
#ifndef MENU_H
#define MENU_H
#include <string>
#include <vector>
#include "mud.h"
#include "conf.h"
#include "socket.h"

class Menu;
#define MENU(name)\
void name(Player* mob, Menu* menu, void*args, int subitem)
//our menu callback.
typedef void (*MENUCB)(Player*, Menu*, void*, int);

//menu data structure
//used for holding information about each individual item.
struct MENU_DATA
{
  std::string name;
  int subitem;
  MENUCB callback;
  void* args;
  Menu* menu;
};

class Menu
{
  std::vector <MENU_DATA*> _options;
  Player* _mobile;
  Socket* _sock;
  int _code;
  std::string _exitMessage;
  struct in_data* _data;
  BOOL _attached;
  void _Initialize(void);
public:
  Menu(Player* mobile);
  Menu(void);
  ~Menu(void);

  void AddItem(const std::string &name, MENUCB callback, void* args, int subitem);
  void RemoveItem(const std::string &item);
  BOOL ItemExists(const std::string &item);

  Socket* GetConnectedSocket(void) const;
  void SetConnectedSocket(Socket* sock);
  Player* GetMobile(void) const;
  void SetMobile(Player* mobile);
  int GetCode(void) const;
  void SetCode(int code);
  std::string GetExitMessage(void) const;
  void SetExitMessage(const std::string &message);
  MENU_DATA* GetDataByIndex(unsigned int index);
  BOOL Attach(void);
  BOOL Unattach(void);
  BOOL ShowMenu(void);
};

class MenuInput:public InputHandle
{
public:
  void Input(void* arg, const std::string &input);
  void Active(in_data* in);
};
#endif
