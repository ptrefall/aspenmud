/*
*A Property tree, used for attaching and storing components on objects.
*Wrritten by CPPMan
*/
#ifndef _PROPERTY_HPP
#define _PROPERTY_HPP
#include "mud.h"
#include "conf.h"
#include <tinyxml.h>
#include "variant.h"
#include <vector>
#include <string>

class Property
{
  std::string Name;
  Variant Value;
  Property *Parent;
  std::vector<Property *> Children;
  std::string LastSearchName;
  Property *LastSearchRes;

public:
  Property(const std::string name, const Variant& value, Property *parent = NULL)
  {
    Name = name;
    Value = value;
    Parent = parent;
    LastSearchRes = NULL;
  }
  Property()
  {
    Name="root";
    Parent = NULL;
    Value = Variant();
    LastSearchRes = NULL;
  }

  ~Property()
  {
    RemoveSelf();
  }

  Variant GetValue()
  {
    return Value;
  }
  Variant& GetPropertyRef(const std::string &name)
  {
    Property *prop = FindProperty(name);
    if (! prop)
      throw std::runtime_error("Property not found.");

    else
      return prop->Value;
  }
  void SetValue(const Variant& value)
  {
    Value = value;
  }

  Property* GetParent() const
  {
    return Parent;
  }
  void SetParent(Property* s)
  {
    Parent = s;
  }

  // Adds a child to this Property.
  void AddProperty(Property *prop)
  {
    prop->SetParent(this);
    Children.push_back(prop);
  }

  // Adds a Property to this Property.
  // AddProperty("stats.hp")
  Property *AddProperty(const std::string &name, const Variant& value)
  {
    size_t dotInd = name.find_first_of(".");
    std::vector<Property *>::iterator it = Children.begin();
    std::vector<Property*>::iterator itEnd = Children.end();

    Property *prop = NULL;

    if (dotInd != std::string::npos)
      {
        std::string childPart = name.substr(0, dotInd);
        std::string newName = name.substr(dotInd + 1);

        // first we need to make sure that childPart exists.
        prop = FindProperty(childPart);
        if (! prop)
          {
            prop = new Property(childPart, Variant(), this);
            Children.push_back(prop);
            return prop->AddProperty(newName, value);
          }
      }
    else
      {
        // we're at the last dot, we can create the node
        prop = new Property(name, value, this);
        Children.push_back(prop);
        return prop;
      }

    return prop;
  }

  // Removes THIS Property from its own parent.
  void RemoveSelf()
  {
    if (! Parent)
      return;

    Parent->RemoveProperty(this);
    Parent = NULL;
  }

  // Removes a Property from this Property (and all of its children)
  void RemoveProperty(const std::string &name)
  {
    Property *prop = FindProperty(name);
    if (! prop)
      return;

    std::vector< Property *>::iterator it = prop->Children.begin();
    std::vector<Property*>::iterator itEnd = prop->Children.end();

    for (; it != itEnd; ++it)
      RemoveProperty(*it);
  }

  // Removes a Property from this Property (and all of its children)
  void RemoveProperty(Property *prop)
  {
    if (! prop)
      return;

    // first we have to remove all of its children first
    std::vector< Property *>::iterator it = prop->Children.begin();
    std::vector <Property*>::iterator itEnd = prop->Children.end();

    for (; it != itEnd; ++it)
      {
        Property *tmp = *it;
        prop->RemoveProperty(tmp);
        it = Children.erase(it);
        delete prop;
      }
  }

  // Finds a Property based on its name.
  Property *FindProperty(const std::string &name)
  {
    // first check cache
    if (name == LastSearchName)
      return LastSearchRes;

    size_t dotInd = name.find_first_of(".");
    std::vector< Property *>::iterator it = Children.begin();
    std::vector<Property*>::iterator itEnd = Children.end();

    if (dotInd != std::string::npos)
      {
        std::string childName = name.substr(0, dotInd);
        std::string newName = name.substr(dotInd + 1);

        for (; it != itEnd; ++it)
          {
            if ( (*it)->Name == childName )
              {
                LastSearchName = childName;
                LastSearchRes = *it;
                return ( (*it)->FindProperty(newName) );
              }
          }
      }

    for (; it != itEnd; ++it)
      {
        if ( (*it)->Name == name )
          {
            return *it;
          }
      }

    return NULL;
  }

  void Serialize(TiXmlElement *root)
  {
    std::vector<Property*>::iterator it, itEnd;

    TiXmlElement *prop = new TiXmlElement("property");
    prop->SetAttribute("name", Name.c_str());
    Value.Serialize(prop);
    if (Children.size())
      {
        it= Children.begin();
        itEnd = Children.end();
        for (; it != itEnd; ++it)
          {
            (*it)->Serialize(prop);
          }
        root->LinkEndChild(prop);

      }
  }
  void Deserialize(TiXmlElement* root, Property* parent = NULL)
  {
    TiXmlElement* property = NULL;
    TiXmlNode *node = NULL;
    Property prop;
    const char* val = NULL;

    val = root->Attribute("name");
    if (val)
      {
        Name = val;
      }

    node = root->FirstChild("variable");
    if (node)
      {
        Value.Deserialize(node->ToElement());
      }

    SetParent(parent);
    for (node = root->FirstChild(); node; node = node->NextSibling())
      {
        property = node->ToElement();
        prop = Property();
        prop.Deserialize(property, this);
      }
  }

};
#endif /* _Property_HPP */
