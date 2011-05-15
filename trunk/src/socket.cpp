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

InputHandle::InputHandle()
{
  _sock = NULL;
}
InputHandle::~InputHandle()
{
}
void InputHandle::Attach(Socket* sock)
{
  _sock = sock;
}
void InputHandle::Active()
{
}
void InputHandle::Input(void* arg, const std::string &input)
{
}

Socket::Socket(const int desc):
  BaseSocket(desc)
{
  _mobile=NULL;
  _Close=0;
  _input = new std::stack<in_data*>();
  _lastInput = time(NULL);
  _inSequence = false;
}
Socket::~Socket()
{
  in_data* data = NULL;
  if (_mobile)
    {
      delete _mobile;
      _mobile=NULL;
    }

  while (!_input->empty())
    {
      data = _input->top();
      _input->pop();
      if (data->handle)
        {
          delete data->handle;
        }
      delete data;
    }
  delete _input;
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

bool Socket::Flush()
{
  int b=0, w=0;
  World* world = World::GetPtr();
  if (!_outBuffer.length())
    {
      return true;
    }
//prepend buffer to prompt
  if ((_mobile!=NULL)&&(_con==con_game))
    {
      _outBuffer+="\r\n"+world->BuildPrompt(_mobile->GetPrompt())+TELNET_IAC+TELNET_GA;
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

ConType Socket::GetConnectionType() const
{
  return _con;
}
void Socket::SetConnectionType(const ConType &s)
{
  _con=s;
}

std::string Socket::GetHost() const
{
  return _host;
}
void Socket::SetHost(const std::string &host)
{
  _host = host;
}

void Socket::AllocatePlayer()
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

BOOL Socket::ShouldClose()
{
  return _Close;
}

BOOL Socket::HasHandle() const
{
  return (_input->empty()==true?false:true);
}
BOOL Socket::HandleInput()
{
  if (HasHandle())
    {
      in_data* data = _input->top();
      data->handle->Input(_input->top()->args, PopCommand());
      ClrInBuffer();
      return true;
    }

  return false;
}
void Socket::ClearInput()
{
  if (HasHandle())
    {
      in_data* in = _input->top();
      _input->pop();
      if (in->handle)
        {
          delete in->handle;
        }
      delete in;
      if (HasHandle())
        {
          in = _input->top();
          in->handle->Active();
        }
    }
}
BOOL Socket::SetInput(in_data* data)
{
  if (data->handle)
    {
      data->handle->Attach(this);
    }
  _input->push(data);
  ClrInBuffer();
  Flush();
  return true;
}

void Socket::UpdateLastCommand()
{
  _lastInput = time(NULL);
}
time_t Socket::GetLastCommand()
{
  return _lastInput;
}
BOOL Socket::CommandPending() const
{
  return (_cqueue.size()==0?false:true);
}
std::string Socket::PopCommand()
{
  if (!CommandPending())
    {
      return "";
    }
  std::string ret = _cqueue.front();
  _cqueue.pop();

  return ret;
}
void Socket::AddCommand(const std::string &input)
{
  _cqueue.push(input);
}
