/*
*exception.h
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
*exception.h
*This file is used so that we can use exceptions; for some reason g++ doesn't like the basic std::exception class.
*/

#ifndef EXCEPTION_H
#define EXCEPTION_H
#include <string>
#include <stdexcept>
#include "mud.h"
#include "conf.h"

class Exception:public std::exception
{
    std::string _message;
public:
    Exception(const std::string &m):
        _message(m) {}
    ~Exception(void) throw();
    const std::string GetMessage(void) const;
    const char* what(void) const throw();
};

//some aspen-specific exceptions
class VarNotFoundException:public Exception
{
public:
    VarNotFoundException(const std::string &msg):Exception(msg) {}
};
class EventNotFoundException:public Exception
{
public:
    EventNotFoundException(const std::string &msg):Exception(msg) {}
};
#endif
