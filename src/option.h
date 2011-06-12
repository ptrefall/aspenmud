/*
*A setup for holding options for players.
*We register each option here, then if the player has something different set we check that.
*This both saves us time and adds for more flexability.
*/
#ifndef OPTION_H
#define OPTION_H
#include <string>
#include <map>
#include "mud.h"
#include "conf.h"
#include "variant.h"

class Option
{
  FLAG _rank;
  Variant _data; //the value.
  VARIABLE_TYPE _type; //the type that the value should hold.
  BOOL _toggle;
  std::string _help; //a brief description of the option.
  std::string _name; //the name of the option.
public:
  Option();
  Option(const std::string &name, const std::string & help, int rank, Variant value, VARIABLE_TYPE type, BOOL toggle=false);
  ~Option();
  template <class T>
  inline T GetValue() const
  {
    return _data.Get<T>();
  }
  Variant GetData(void) const;
  void SetData(Variant var);
  void SetBool(BOOL val);
  void SetInt(int val);
  void SetString(const std::string &val);
  std::string GetName(void) const;
  void SetName(const std::string &name);
  std::string GetHelp(void) const;
  void SetHelp(const std::string &help);
  int GetRank(void) const;
  BOOL CanToggle() const;
};

/*
*This makes the overall setup easier, though the design becomes a bit more confusing.
*Rather than hold an option node per player which is just a copy of the option node stored on world, we store the name and the value of the option.
*We also store a pointer to the main option node.
*/
struct OptionNode
{
  Variant _data;
  VARIABLE_TYPE _type; //the type that the value should hold.
  Option* _option;
};

//option helper functions
/*
*Gets the value of the option either from the global option, or from the player's option. Checks the player first.
*Param: [in] the name of the option.
*Param: [in] a pointer to the player object.
*Return: A variant holding the value.
*/
Variant GetOptionValue(const std::string &name, const Player* mobile);
#endif
