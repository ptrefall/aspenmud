#include "help.h"
#include "../mud.h"
#include "../conf.h"
#include "../world.h"
#include "../player.h"
#include "../utils.h"
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>

HelpEntry::HelpEntry(const std::string &name, const std::string &data, FLAG access, HELP_ENTRY_TYPE type):_name(name), _data(data), _access(access), _type(type)
{
  _lastModified = time(NULL);
}
HelpEntry::~HelpEntry()
{
}

std::string HelpEntry::GetName() const
{
  return _name;
}
void HelpEntry::SetName(const std::string &name)
{
  _name = name;
}

std::string HelpEntry::GetData() const
{
  return _data;
}
void HelpEntry::SetData(const std::string &data)
{
  _data = data;
}

HELP_ENTRY_TYPE HelpEntry::GetType() const
{
  return _type;
}
void HelpEntry::SetType(HELP_ENTRY_TYPE type)
{
  _type = type;
}

FLAG HelpEntry::GetAccess() const
{
  return _access;
}
void HelpEntry::SetAccess(FLAG access)
{
  _access = access;
}
