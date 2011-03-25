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
#include <tinyxml.h>

enum HELP_ENTRY_TYPE {h_combat, h_movement, h_communication};

//the basic help node.
class HelpEntry
{
  time_t _lastModified;
  std::string _name;
  std::string _data;
  FLAG _access;
  HELP_ENTRY_TYPE _type;
public:
  HelpEntry(const std::string &name, const std::string &data, FLAG access, HELP_ENTRY_TYPE type);
  HelpEntry();
  ~HelpEntry();
  /*
  *return:  the name of the entry.
  */
  std::string GetName() const;
  /*
  *Sets the name on the help entry.
  *Param: [in] the name of the entry.
  */
  void SetName(const std::string &name);
  /*
  *Return: the text associated with the entry.
  */
  std::string GetData() const;
  /*
  *Sets the data associated with the entry.
  *Param: [in] the data for the entry.
  */
  void SetData(const std::string &data);
  /*
  *Return: The type of the help entry.
  */
  HELP_ENTRY_TYPE GetType() const;
  /*
  *Sets the type of the help entry.
  *Param: [in] the type for the entry.
  */
  void SetType(HELP_ENTRY_TYPE type);
  /*
  *Gets the minimum access required to view the entry.
  *Return: the minimum access.
  */
  FLAG GetAccess() const;
  /*
  *Sets the access required to view the help entry.
  *Param: [in] the minimum access.
  */
  void SetAccess(FLAG access);
  /*
  *Serializes the entry into the xml element.
  *Param: [in] the root xml element to append entries to.
  */
  void Serialize(TiXmlElement* root);
  /*
  *Retrieves help entry data from the specified XML entry.
  *Param: [in] the element where the data is to be retrieved from.
  */
  void Deserialize(TiXmlElement* entry);
};

class HelpTable
{
  std::vector<HelpEntry*> *_entries;
public:
  HelpTable();
  ~HelpTable();
  /*
  *Loads all help entries stored in the default help file.
  */
  void Load();
  /*
  *Iterates through all of the help entries stored and serializes them to one file.
  */
  void Save();
};
#endif
