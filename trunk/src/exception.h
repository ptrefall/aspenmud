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
