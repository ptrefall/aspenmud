/*
*ComponentFactory.cpp
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

#include <map>
#include "ComponentFactory.h"
#include "component.h"

ComponentFactory::ComponentFactory(void)
{
  _components=new std::map<std::string,COMCREATECB>();
}
ComponentFactory::~ComponentFactory(void)
{
  delete _components;
}
BOOL ComponentFactory::HasComponent(const std::string &name)
{
  return (_components->count(name)==0?0:1);
}

BOOL ComponentFactory::RegisterComponent(const std::string &name,COMCREATECB cb)
{
  if (HasComponent(name))
    {
      return false;
    }

  (*_components)[name]=cb;
  return true;
}

Component* ComponentFactory::Create(const std::string &name)
{
  if (HasComponent(name))
    {
      return ((*_components)[name])();
    }

  return NULL;
}
