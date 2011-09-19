#include "mud.h"
#include "conf.h"
#include "socket.h"
#include "utils.h"
#include "inputHandlers.h"

YesNoHandler::YesNoHandler(YESNOCB cb):InputHandle()
{
  _cb = cb;
}

void YesNoHandler::Input(void* arg, const std::string &input)
{
  std::string text = input;
  Lower(text);
  if (text == "y" || text == "yes")
    {
      _cb((Socket*)arg, true);
    }
  _cb((Socket*)arg, false);
}

BOOL YesNoHandler::CreateHandler(Socket* sock,  YESNOCB cb)
{
  if (sock == NULL)
    {
      return false;
    }

  in_data* input = new in_data();
  YesNoHandler* handle = new YesNoHandler(cb);
  if (!handle)
    {
      return false;
    }

  input->handle = handle;
  input->args = (void*)sock;
  return sock->SetInput(input);
}
