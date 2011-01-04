/*
*ComponentFactory.h
*
*   Copyright 2010 Tyler Littlefield.
*
*   Licensed under the Apache License, Version 2.0 (the "License");
*   you may not use this file except in compliance with the License.
*   You may obtain a copy of the License at
*
*       http://www.apache.org/licenses/LICENSE-2.0
*
*   Unless required by applicable law or agreed to in writing, software
*   distributed under the License is distributed on an "AS IS" BASIS,
*   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*   See the License for the specific language governing permissions and
*   limitations under the License.
*/

/*
*Component Factory
*Holds a list of component names and function pointers for their creation.
*/

#ifndef COMPONENT_FACTORY_H
#define COMPONENT_FACTORY_H
#include <map>
#include <string>
#include "mud.h"
#include "conf.h"
#include "component.h"

class ComponentFactory
{
    std::map <std::string,COMCREATECB> *_components;
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
};
#endif
