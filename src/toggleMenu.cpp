#include "conf.h"
#include "mud.h"
#include "toggleMenu.h"
#include "menu.h"
#include "utils.h"
#include "player.h"
#include <iterator>
#include <boost/function.hpp>
#include <sstream>

ToggleMenu::ToggleMenu(Player* mobile, ToggleGetter g, ToggleSetter s):Menu(mobile)
{
  _Initialize();
  _g = g;
  _s = s;
  _code = g();
}
ToggleMenu::~ToggleMenu()
{
}

void ToggleMenu::SaveFlag()
{
  _s(_code);
}

void ToggleMenu::AddItem(const std::string &name, int position)
{
  if (!ItemExists(name))
    {
      _positions[name] = position;
    }
}
void ToggleMenu::RemoveItem(const std::string &name)
{
  _positions.erase(name);
}
BOOL ToggleMenu::ItemExists(const std::string &name)
{
  return (_positions.count(name)?true:false);
}
BOOL ToggleMenu::ShowMenu()
{
  std::map<std::string, int>::iterator it, itEnd;
  int i = 1;
  std::stringstream st;

  if (!CanShow())
    {
      return false;
    }

  st << "[0  ] ";
  st << _exitMessage << "\n";

  itEnd = _positions.end();
  for (it = _positions.begin(); it != itEnd; ++it, ++i)
    {
      st << "[" << i << "] " << (BitIsSet(_code, (*it).second)?"*":"") << (*it).first << "\n";
    }
  _mobile->Message(MSG_LIST, st.str());
  _sock->Flush();

  return true;
}
BOOL ToggleMenu::Attach()
{
  if ((_sock==NULL) || (_attached))
    {
      return false;
    }

  _data = new in_data();
  _data->handle= new ToggleMenuInput();
  _data->args=(void*)this;

  _sock->SetInput(_data);
  _attached=true;
  ShowMenu();
  return true;
}

std::map<std::string, int>* ToggleMenu::GetPositions()
{
  return &_positions;
}

void ToggleMenuInput::Input(void* arg, const std::string &input)
{
  ToggleMenu* m=(ToggleMenu*)arg;
  std::string::iterator it, itEnd;
  std::map<std::string, int>::iterator mit;
  std::map<std::string, int> *_positions = m->GetPositions();
  std::string data = input;
  int bit;

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
      m->SaveFlag();
      m->Unattach();
      return;
    }

  if ((int)_positions->size() < result)
    {
      m->GetMobile()->Message(MSG_ERROR, "Invalid input.");
      m->ShowMenu();
      return;
    }

//now we find out what position they chose and toggle that bit.
  mit = _positions->begin();
  advance(mit, result-1);
  bit = (*mit).second;
  if (BitIsSet(m->GetCode(), bit))
    {
      m->SetCode(BitClear(m->GetCode(), bit));
    }
  else
    {
      m->SetCode(BitSet(m->GetCode(), bit));
    }
  m->ShowMenu();
}
