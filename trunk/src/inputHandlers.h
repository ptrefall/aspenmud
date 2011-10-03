/*
*These are basic utility classes, that create generic input handlers.
*/
#ifndef INPUT_HANDLERS_H
#define INPUT_HANDLERS_H
#include "mud.h"
#include "conf.h"
#include "socket.h"
#include <boost/function.hpp>

typedef boost::function<void (Socket*, BOOL, void*)> YESNOCB;
typedef boost::function<void (Socket*, std::string&, void*)> LINECB;

class YesNoHandler:public InputHandle
{
  YESNOCB _cb;
  void* _arg;
public:
  YesNoHandler(YESNOCB cb, void* arg=NULL);
  void Input(void* arg, const std::string &input);
  static BOOL CreateHandler(Socket* sock, YESNOCB cb, void* arg = NULL);
};

class LineHandler:public InputHandle
{
  LINECB _cb;
  void* _arg;
public:
  LineHandler(LINECB cb, void* arg = NULL);
  void Input(void* arg, const std::string &input);
  static BOOL CreateHandler(Socket* sock, LINECB cb);
};
#endif
