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
#include "socket.h"
#include "player.h"
#include "world.h"
#include "telnet.h"

Socket::Socket(const int desc)
{
  _control = desc;
  _mobile=NULL;
  _Close=0;
  _addr=new sockaddr_in();
  _lastInput = time(NULL);
  _inSequence = false;
}
Socket::~Socket()
{
  if (_control != -1)
    {
      close(_control);
    }
  if (_mobile)
    {
      delete _mobile;
      _mobile=NULL;
    }
  if (_addr)
    {
      delete _addr;
      _addr=NULL;
    }
}

int Socket::GetControl() const
{
  return _control;
}

bool Socket::Read()
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
//also parse out telnet
          for (k=0; k<size; k++)
            {
              if (temp[k] == TELNET_IAC)
                {
//we found a telnet sequence.
                  if (_inSequence && temp[k-1] == TELNET_IAC)
                    {
//we found a IAC IAC (escape), lets add IAC to the string.
                      _inBuffer += temp[k];
//clear the Telnet buffer:
                      _telnetBuffer.clear();
                      _inSequence = false; //we are no longer in a sequence.
                      continue;
                    }
                  else     //we have a single IAC:
                    {
                      _inSequence = true;
                      _telnetBuffer +=  temp[k];
                      continue;
                    }
                } //check for IAC
//now we check for everything else:
              else if (_inSequence)   //we're in a sequence, the following should be part of that.
                {
//check for a three byte sequence: iac will/do/dont/dont command
                  if (temp[k] == TELNET_WILL || temp[k] == TELNET_WONT || temp[k] == TELNET_DO || temp[k] == TELNET_DONT)
                    {
                      _telnetBuffer += temp[k];
                      continue;
                    }
//we check for a do/will/wont/dont sequence in the char before and handle whatever option:
                  else if ((k > 0) && (temp[k - 1] == TELNET_WILL || temp[k - 1] == TELNET_WONT || temp[k - 1] == TELNET_DO || temp[k - 1] == TELNET_DONT))
                    {
                      switch(temp[k])
                        {
                        default:
                          _inSequence = false;
                          _telnetBuffer.clear();
                          continue;
                        }
                    }
                }
              else
                {
                  _inBuffer+=temp[k];
                }
            }
//check for a newline character.
          if (temp[size - 1] == '\n' || temp[size-1] == '\r')
            {
              line=_inBuffer.substr(0,_inBuffer.find_first_of("\n\r"));
//we have a newline, push it to our command queue.
              if (line.length())
                {
                  _cqueue.push(line);
                }
              _inBuffer.clear();
              break;
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

void Socket::Write(const std::string &txt)
{
  _outBuffer += txt;
}

bool Socket::Flush()
{
  int b=0, w=0;
  if (!_outBuffer.length())
    {
      return true;
    }
//prepend buffer to prompt
  if ((_mobile!=NULL)&&(_con==con_game))
    {
      _outBuffer+="\r\n"+_mobile->GetPrompt()+TELNET_IAC+TELNET_GA;
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

std::string Socket::GetInBuffer()
{
//we need to clean up extra junk at the end
  if (_inBuffer=="")
    {
      return "";
    }
  return _inBuffer.substr(0,_inBuffer.find_first_of("\n\r"));
}

void Socket::ClrInBuffer()
{
  _inBuffer.erase();
}

sockaddr_in* Socket::GetAddr() const
{
  return _addr;
}
void Socket::SetAddr(sockaddr_in* addr)
{
  memcpy(_addr,addr,sizeof(sockaddr_in));
}

ConType Socket::GetConnectionType() const
{
  return _con;
}
void Socket::SetConnectionType(const ConType &s)
{
  _con=s;
}

std::string Socket::GetHost(void) const
{
  return _host;
}
void Socket::SetHost(const std::string &host)
{
  _host = host;
}

void Socket::AllocatePlayer(void)
{
  if (!_mobile)
    {
      _mobile=new Player();
    }
}
Player* Socket::GetPlayer() const
{
  return _mobile;
}

void Socket::Kill()
{
  if (!_Close)
    {
      if (_mobile)
        {
          if (GetConnectionType()==con_game)
            {
              _mobile->Save(true);
              _mobile->LeaveGame();
            }
        }
      _Close=1;
    }
}

BOOL Socket::ShouldClose(void)
{
  return _Close;
}

BOOL Socket::HasHandle(void) const
{
  if (_input.size())
    {
      return true;
    }
  return false;
}
BOOL Socket::HandleInput(void)
{
  if (HasHandle())
    {
      (_input.front()->handle)(this,_input.front()->args,PopInput());
      ClrInBuffer();
      return true;
    }
  return false;
}
void Socket::ClearInput(void)
{
  if (_input.size())
    {
      in_data* in = _input.front();
      _input.pop_front();
      delete in;
    }
}
BOOL Socket::SetInput(in_data* data)
{
  _input.push_front(data);
  ClrInBuffer();
  Flush();
  return true;
}

void Socket::UpdateLastInput()
{
  _lastInput = time(NULL);
}
time_t Socket::GetLastInput(void)
{
  return _lastInput;
}

BOOL Socket::InputPending() const
{
  return (_cqueue.size()==0?false:true);
}
std::string Socket::PopInput()
{
  if (_cqueue.size() == 0)
    {
      return "";
    }
  std::string ret = _cqueue.front();
  _cqueue.pop();

  return ret;
}
void Socket::AddInput(const std::string &input)
{
  _cqueue.push(input);
}
