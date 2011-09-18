/*
*This menu is used for toggling specific bits on an integer.
*The purpose is to allow for setting flags on objects.
*/
#ifndef TOGGLE_MENU_H
#define TOGGLE_MENU_H
#include "conf.h"
#include "mud.h"
#include "menu.h"
typedef boost::function<void (FLAG)> ToggleSetter;
typedef boost::function<FLAG ()> ToggleGetter;

class ToggleMenu:public Menu
{
  std::map<std::string, int> _positions;
  ToggleGetter _g;
  ToggleSetter _s;
public:
  ToggleMenu(Player* mobile, ToggleGetter g, ToggleSetter s);
  ~ToggleMenu();
  void SaveFlag();
  void AddItem(const std::string &name, int position);
  void RemoveItem(const std::string &name);
  BOOL ItemExists(const std::string &name);
  BOOL ShowMenu();
  BOOL Attach();
  std::map<std::string, int>* GetPositions();
};

class ToggleMenuInput:public InputHandle
{
public:
  void Input(void* arg, const std::string &input);
};
#endif
