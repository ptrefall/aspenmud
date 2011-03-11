#include "exception.h"

Exception::~Exception(void) throw()
{
}

const std::string Exception::GetMessage(void) const
{
  return _message;
}

const char* Exception::what(void) const throw()
{
  return _message.c_str();
}
