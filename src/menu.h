/*
*menu.h
*
*   Copyright 2010 Tyler Littlefield.
*
*   Licensed under the Apache License, Version 2.0 (the "License");
*   you may not use this file except in compliance with the License.
*   You may obtain a copy of the License at
*
*       http://www.apache.org/licenses/LICENSE-2.0
*
*   Unless required by applicable law or agreed to in writing, software
*   distributed under the License is distributed on an "AS IS" BASIS,
*   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*   See the License for the specific language governing permissions and
*   limitations under the License.
*/


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

class Menu;

//menu data structure
//used for holding information about each individual item.
struct menu_data
{
  std::string name;
  int subitem;
  MENUCB callback;
  BOOL retAfter; //should we return back to the menu after calling the callback?
  void* args;
  Menu* menu;
};
typedef struct menu_data MENU_DATA;

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

INPUT(menu_handler);
#endif
