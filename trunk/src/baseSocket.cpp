#include "mud.h"
#include "conf.h"
#include "baseSocket.h"
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>
#include <ctype.h>
#include <string>
#include <ctime>
#include <cstring>
#include <cstdlib>

BaseSocket::BaseSocket(const int desc)
{
  _control = desc;
  _addr=new sockaddr_in();
}
BaseSocket::~BaseSocket()
{
  if (_control != -1)
    {
      close(_control);
    }
  if (_addr)
    {
      delete _addr;
      _addr=NULL;
    }
}

int BaseSocket::GetControl() const
{
  return _control;
}

bool BaseSocket::Read()
{
  char temp[4096 + 2];
  int size,k=0;
  std::string line;

  while (true)
    {
      size = recv(_control, temp, 4096, 0);
      if (size > 0)
        {
          temp[size] = '\0'; //sets the last byte we received to null.
//iterate through the list and add that to the std::string
          for (k=0; k<size; k++)
            {
              _inBuffer+=temp[k];
            }
        }
      else if (size == 0)
        {
          return false;
        }
      else if (errno == EAGAIN || size == 4096)
        {
          break;
        }
      else
        {
          return false;
        }
    }

  return true;
}
void BaseSocket::Write(const std::string &txt)
{
  _outBuffer += txt;
}
bool BaseSocket::Flush()
{
  int b=0, w=0;
  if (!_outBuffer.length())
    {
      return true;
    }

  while (_outBuffer.length() > 0)
    {
      b = (_outBuffer.length() < 4096) ? _outBuffer.length() : 4096;
      // any write failures ?
      if (_control!=-1)
        {
          if ((w = send(_control, _outBuffer.c_str(), b, 0)) == -1)
            {
              return false;
            }
        }
      // move the buffer down
      _outBuffer.erase(0, w);
    }

  return true;
}

std::string BaseSocket::GetInBuffer()
{
  return _inBuffer;
}
void BaseSocket::ClrInBuffer()
{
  _inBuffer.erase();
}

sockaddr_in* BaseSocket::GetAddr() const
{
  return _addr;
}
void BaseSocket::SetAddr(sockaddr_in* addr)
{
  memcpy(_addr,addr,sizeof(sockaddr_in));
}
