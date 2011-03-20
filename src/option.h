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
  int _rank;
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
*One of the problems with Players is that they're holding an entire option setup, which they do not need.
*/
struct OptionNode
{
  Variant _data;
  VARIABLE_TYPE _type; //the type that the value should hold.
  Option* _option;
};
#endif
