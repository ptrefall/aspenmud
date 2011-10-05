#ifndef COMPONENT_H
#define COMPONENT_H
#include <string>
#include <vector>
#include "mud.h"
#include "conf.h"
#include "entity.h"
#include "event.h"

class Component
{
  Entity* _object; //The object associated with the component (it's parent).
  BOOL _attached; //set if the component is attached to an object.
  std::string _name;
  std::vector <std::string> *_dependencies; //component dependencies.
public:
  EventManager events;
  Component(const std::string &name);
  Component(void);
  virtual ~Component();
  /*
  *Getters and setters
  *Getters:
  *Return: the value being requested.
  *Setters:
  *Param: [in] the value to set
  */
  void SetName(const std::string &name);
  std::string GetName(void) const;
  void SetObject(Entity *obj);
  Entity* GetObject(void) const;

  /*
  *Attaches the component to the object, sets the object if needed.
  *Also adds dependencies to the object, if they are not already added.
  *Param: [in] the object to attach to.
  */
  void Attach(Entity* obj);
  /*
  *Detaches the dependency from the object.
  */
  void Detach(void);

  /*
  *Checks to see if the specified dependency is located in the dependencies list.
  *Param: [in] the name of the dependency.
  *Return: True if the dependency exists in the dependencies list, false otherwise.
  */
  BOOL DependencyExists(const std::string &name) const;
  /*
  *Will add the specified dependency to the list.
  *Param: [in] The name of the component to add as a dependency.
  */
  BOOL AddDependency(const std::string &dependency);
  /*
  *Returns a pointer to a vector of dependency names.
  *Return: a pointer to the vector of strings containing the names of the dependencies associated with the component.
  */
  std::vector<std::string>* GetDependencies(void) const;
  virtual void _Init(void);
};

/*
*A function pointer passed to the component factory.
*Used to point to the create methods of a component.
*/
typedef Component* (*COMCREATECB)(void);
#define COMCREATE(name) Component* name(void)
typedef void (*COMINITCB)(void);
#define COMINIT(name) void name(void)

#endif