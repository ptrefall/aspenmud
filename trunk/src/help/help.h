#ifndef HELP_H
#define HELP_H
#include "../mud.h"
#include "../conf.h"
#include "../world.h"
#include "../player.h"
#include "../utils.h"
#include <string>
#include <vector>
#include <cstdlib>

enum HELP_ENTRY_TYPE {h_combat, h_movement, h_communication};
class HelpEntry
{
  time_t _lastModified;
  std::string _name;
  std::string _data;
  FLAG _access;
  HELP_ENTRY_TYPE _type;
public:
  HelpEntry(const std::string &name, const std::string &data, FLAG access, HELP_ENTRY_TYPE type);
  ~HelpEntry();
  std::string GetName() const;
  void SetName(const std::string &name);
  std::string GetData() const;
  void SetData(const std::string &data);
  HELP_ENTRY_TYPE GetType() const;
  void SetType(HELP_ENTRY_TYPE type);
  FLAG GetAccess() const;
  void SetAccess(FLAG access);
};
#endif
