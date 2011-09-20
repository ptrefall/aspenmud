#include <string>
#include <sstream>
#include <vector>
#include "menu.h"
#include "socket.h"
#include "player.h"

void Menu::_Initialize(void)
{
  _exitMessage="abort";
  _data=NULL;
  _attached = false;
}

Menu::Menu(Player* mobile)
{
  _mobile = mobile;
  _sock = _mobile->GetSocket();
  _Initialize();
}
Menu::Menu(void)
{
  _sock=NULL;
  _mobile = NULL;
  _Initialize();
}
Menu::~Menu(void)
{
  std::vector <MENU_DATA*>::iterator it, itEnd;

  itEnd = _options.end();
  for (it = _options.begin(); it != itEnd; ++it)
    {
      delete (*it);
    }
}

void Menu::AddItem(const std::string &name, MENUCB callback, void*args, int subitem)
{
  MENU_DATA* item = NULL;
  if ((callback == NULL) || (name == ""))
    {
      return;
    }

  item = new MENU_DATA();
  item->name = name;
  item->subitem = subitem;
  item->callback = callback;
  item->args = args;
  item->menu = this;

  _options.push_back(item);
}
void Menu::RemoveItem(const std::string &item)
{
  std::vector<MENU_DATA*>::iterator it;
  for (it = _options.begin(); it != _options.end(); it++)
    {
      if ((*it)->name == item)
        {
          delete (*it);
          return;
        }
    }
}
BOOL Menu::ItemExists(const std::string &item)
{
  std::vector<MENU_DATA*>::iterator it;

  for (it = _options.begin(); it != _options.end(); it++)
    {
      if ((*it)->name == item)
        {
          return true;
        }
    }
  return false;
}

Socket* Menu::GetConnectedSocket(void) const
{
  return _sock;
}
void Menu::SetConnectedSocket(Socket* sock)
{
  _sock=sock;
}
Player* Menu::GetMobile(void) const
{
  return _mobile;
}
void Menu::SetMobile(Player* mobile)
{
  _mobile = mobile;
}

int Menu::GetCode(void) const
{
  return _code;
}
void Menu::SetCode(int code)
{
  _code=code;
}

std::string Menu::GetExitMessage(void) const
{
  return _exitMessage;
}
void Menu::SetExitMessage(const std::string &message)
{
  _exitMessage=message;
}
MENU_DATA* Menu::GetDataByIndex(unsigned int index)
{
  index--;
  if ((index+1 > _options.size()) || (index < 0))
    {
      return NULL;
    }
  return _options.at(index);
}

BOOL Menu::Attach()
{
  if ((_sock==NULL) || (_attached))
    {
      return false;
    }

  _data = new in_data();
  _data->handle= new MenuInput();
  _data->args=(void*)this;

  _sock->SetInput(_data);
  _attached=true;
  ShowMenu();
  return true;
}
BOOL Menu::Unattach(void)
{
  if (_sock==NULL)
    {
      return false;
    }
  if (!_attached)
    {
      return false;
    }
  if (!_sock->HasHandle())
    {
      return false;
    }

  _sock->ClearInput();
  return true;
}
BOOL Menu::CanShow()
{
  if (_sock==NULL)
    {
      return false;
    }
  if (!_sock->HasHandle())
    {
      return false;
    }

  return true;
}
BOOL Menu::ShowMenu()
{
  std::vector <MENU_DATA*>::iterator it;

  if (!CanShow())
    {
      return false;
    }

  int count=0;
  std::stringstream st;
  st << "[0  ] ";
  st << _exitMessage << "\n";
  for (count=1, it = _options.begin(); it != _options.end(); it++, count++)
    {
      st << "[" << count << "] ";
      st << (*it)->name << "\n";
    }
  _mobile->Message(MSG_LIST, st.str());
  _sock->Flush();
  return true;
}

void MenuInput::Input(void* arg, const std::string &input)
{
  Menu* m=(Menu*)arg;
  std::string::iterator it, itEnd;
  std::string data = input;
  MENU_DATA* option = NULL;

  itEnd = data.end();
  for (it = data.begin(); it != itEnd; ++it)
    {
      if (!isdigit((*it)))
        {
          m->GetMobile()->Message(MSG_ERROR, "Invalid input.");
          m->ShowMenu();
          return;
        }
    }

  int result = atoi(input.c_str());

  if (!result)
    {
      m->GetMobile()->Message(MSG_INFO, "Exiting.");
      m->Unattach();
      return;
    }
  option = m->GetDataByIndex(result);
  (option->callback)(m->GetMobile(), m, option->args, option->subitem);
}
void MenuInput::Active(in_data* in)
{
  ((Menu*)in->args)->ShowMenu();
}