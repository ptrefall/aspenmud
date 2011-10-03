/*
*A system for handling and controling menus.
*/
#ifndef MENU_H
#define MENU_H
#include <string>
#include <vector>
#include <boost/function.hpp>
#include "mud.h"
#include "conf.h"
#include "socket.h"

class Menu;
#define MENU(name)\
void name(Player* mob, Menu* menu, void*args, int subitem)
#define CMENU(c, name)\
void c::name(Player* mob, Menu* menu, void*args, int subitem)
//our menu callback.
typedef boost::function<void (Player*, Menu*, void*, int)> MENUCB;
typedef boost::function<void (Menu*)> UNATTACHCB;

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
protected:
  std::vector <MENU_DATA*> _options;
  void* _arg;
  Player* _mobile;
  Socket* _sock;
  int _code;
  std::string _exitMessage;
  struct in_data* _data;
  BOOL _attached;
  UNATTACHCB _unattachcb;
  void _Initialize(void);
public:
  Menu(Player* mobile, void* arg=NULL);
  Menu(void);
  ~Menu(void);

  virtual void AddItem(const std::string &name, MENUCB callback, void* args, int subitem);
  virtual void RemoveItem(const std::string &item);
  virtual BOOL ItemExists(const std::string &item);

  Socket* GetConnectedSocket(void) const;
  void SetConnectedSocket(Socket* sock);
  Player* GetMobile(void) const;
  void SetMobile(Player* mobile);
  virtual int GetCode(void) const;
  virtual void SetCode(int code);
  virtual void* GetArg() const;
  virtual void SetArg(void* arg);
  virtual void AtUnattach(UNATTACHCB cb);
  std::string GetExitMessage(void) const;
  void SetExitMessage(const std::string &message);
  virtual MENU_DATA* GetDataByIndex(unsigned int index);
  virtual BOOL Attach();
  virtual BOOL Unattach(void);
  virtual BOOL CanShow();
  virtual BOOL ShowMenu();
};

class MenuInput:public InputHandle
{
public:
  void Input(void* arg, const std::string &input);
  void Active(in_data* in);
};
#endif
