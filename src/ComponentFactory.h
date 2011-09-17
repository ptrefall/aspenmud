/*
*Component Factory
*Holds a list of component names and function pointers for their creation.
*/
#ifndef COMPONENT_FACTORY_H
#define COMPONENT_FACTORY_H
#include <string>
#include <map>
#include "mud.h"
#include "conf.h"
#include "component.h"

struct componentEntry
{
  COMCREATECB cb;
  unsigned int id;
};

class ComponentFactory
{
  std::map <std::string, componentEntry*> *_components;
  unsigned int _id;
public:
  ComponentFactory(void);
  ~ComponentFactory(void);
  /*
  *Checks for the existance of the specified component.
  *Param: [in] the name of the component.
  *Return: true if the component was found, false otherwise.
  */
  BOOL HasComponent(const std::string &name);
  /*
  *Registers the component with the factory if it isn't already registered by the same name.
  *Param: [in] the name of the component.
  *[in] a function pointer for the creation method of the component.
  *Returns: True on success, false on failure.
  */
  BOOL RegisterComponent(const std::string &name,COMCREATECB cb);
  /*
  *Creates the specified component if it's in the registered components.
  *Param: [in] the name of the component.
  *Returns: A pointer to the object created, or NULL if the component wasn't created.
  */
  Component* Create(const std::string &name);
  unsigned int GetId(const std::string &name);
};
#endif
