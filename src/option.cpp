#include <string>
#include <map>
#include "option.h"
#include "variant.h"
#include "player.h"
#include "world.h"

Option::Option()
{
  _rank = RANK_PLAYER;
  _data = Variant();
  _type = VAR_EMPTY;
  _toggle = false;
  _help = "";
  _name = "";
}
Option::Option(const std::string &name, const std::string & help, int rank, Variant value, VARIABLE_TYPE type, BOOL toggle)
{
  _name = name;
  _help = help;
  _rank = rank;
  _type = type;
  _data = value;
  _toggle = toggle;
}
Option::~Option()
{
}

Variant Option::GetData(void) const
{
  return _data;
}
void Option::SetData(Variant var)
{
  _data = var;
}
void Option::SetBool(BOOL val)
{
  _data.SetInt((val==true?1:0));
}
void Option::SetInt(int val)
{
  _data.SetInt(val);
}
void Option::SetString(const std::string &val)
{
  _data.SetStr(val);
}

std::string Option::GetName(void) const
{
  return _name;
}
void Option::SetName(const std::string &name)
{
  _name = name;
}
std::string Option::GetHelp(void) const
{
  return _help;
}
void Option::SetHelp(const std::string &help)
{
  _help = help;
}
int Option::GetRank(void) const
{
  return _rank;
}

BOOL Option::CanToggle() const
{
  return _toggle;
}

Variant GetOptionValue(const std::string &name, const Player* mobile)
{
  if (mobile->OptionExists(name))
    {
      return mobile->GetOption(name)->_data;
    }
  if (world->OptionExists(name))
    {
      return world->GetGlobalOption(name)->GetData();
    }

  return Variant();
}