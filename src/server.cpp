/*
*A basic server class.
*Manages all game connections and socket operations for the game.
*/
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <list>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <ctime>
#include "server.h"
#include "utils.h"
#include "log.h"
#include "player.h"
#include "world.h"
#include "telnet.h"
#include "socket.h"

BanList::BanList()
{
}
BanList::~BanList()
{
}
BOOL BanList::AddAddress(const std::string &address)
{
  if (AddressExists(address))
    {
      return false;
    }

  in_addr addr;
  if (!inet_aton(address.c_str(), &addr))
    {
      return false;
    }

  if (AddressExists(addr.s_addr))
    {
      return false;
    }

  _addresses.push_back(addr.s_addr);
  return true;
}
BOOL BanList::RemoveAddress(const std::string &address)
{
  std::vector<unsigned long>::iterator it, itEnd;

  if (!AddressExists(address))
    {
      return false;
    }

  in_addr addr;
  if (inet_aton(address.c_str(), &addr))
    {
      return false;
    }

  itEnd = _addresses.end();
  for (it = _addresses.begin(); it != itEnd; ++it)
    {
      if ((*it) == addr.s_addr)
        {
          it = _addresses.erase(it);
        }
    }

  return true;
}
BOOL BanList::AddressExists(const std::string &address)
{
  std::vector<unsigned long>::iterator it, itEnd;
  in_addr addr;
  if (inet_aton(address.c_str(), &addr))
    {
      return false;
    }

  itEnd = _addresses.end();
  for (it = _addresses.begin(); it != itEnd; ++it)
    {
      if ((*it) == addr.s_addr)
        {
          return true;
        }
    }

  return false;
}
BOOL BanList::AddressExists(unsigned long address)
{
  std::vector<unsigned long>::iterator it, itEnd;

  itEnd = _addresses.end();
  for (it = _addresses.begin(); it != itEnd; ++it)
    {
      if ((*it) == address)
        {
          return true;
        }
    }

  return false;
}
void BanList::ListAddresses(std::vector<std::string>* addresses)
{
  in_addr addr;
  std::vector<unsigned long>::iterator it, itEnd;
  std::string val;

  itEnd = _addresses.end();
  for (it = _addresses.begin(); it != itEnd; ++it)
    {
      addr.s_addr = (*it);
      val=inet_ntoa(addr);
      printf("%s\n", val.c_str());
      addresses->push_back(val);
    }
}

void BanList::Serialize(TiXmlElement* root)
{
  TiXmlElement *entries = new TiXmlElement("entries");
  std::vector<std::string>* addresses = new std::vector<std::string>();
  std::vector<std::string>::iterator it, itEnd;
  TiXmlElement *entry = NULL;

  ListAddresses(addresses);
  itEnd = addresses->end();
  for (it = addresses->begin(); it != itEnd; ++it)
    {
      entry = new TiXmlElement("entry");
      entry->SetAttribute("address", (*it).c_str());
      entries->LinkEndChild(entry);
    }

  root->LinkEndChild(entries);
  delete addresses;
}
void BanList::Deserialize(TiXmlElement* root)
{
  TiXmlElement* entries = NULL;
  TiXmlNode* node = NULL;
  TiXmlElement* entry = NULL;

  node = root->FirstChild("entries");
  if (!node)
    {
      return;
    }

  entries = node->ToElement();
  for (node = entries->FirstChild(); node; node = node->NextSibling())
    {
      entry = node->ToElement();
      AddAddress(entry->Attribute("address"));
    }
}

Server::Server()
{
  World*world = World::GetPtr();
  control = -1; //listening socket
//clear descriptor sets:
  FD_ZERO(&fSet);
  FD_ZERO(&rSet);
  // clear the address buffer
  memset(&my_addr, 0, sizeof(my_addr));
  // initialize lastSleep
  gettimeofday(&lastSleep, NULL);
//initialize the ban list.
  blist = new BanList();
  world->AddState("banlist", blist);
}
Server::~Server()
{
//free variables and close sockets.
  if (control != -1)
    close(control);
  delete blist;
}

BOOL Server::Listen(const int port)
{
  World* world = World::GetPtr();

  int reuse = 1;
  // try to create a communications endpoint
  if ((control = socket(PF_INET, SOCK_STREAM, 0)) == -1)
    {
      world->WriteLog("could not create socket.", ERR);
      return false;
    }
  // set options for this control socket
  if (setsockopt(control, SOL_SOCKET, SO_REUSEADDR, (const char *)&reuse, sizeof(DWORD)) == -1)
    {
      world->WriteLog("Setting socket options failed.");
      close(control);
      return false;
    }
  // attach the control socket to it's own filedescriptor set
  FD_SET(control, &fSet);
  // settings for this socket, (and set the listning port)
  my_addr.sin_family      = PF_INET;
  my_addr.sin_addr.s_addr = INADDR_ANY;
  my_addr.sin_port        = htons(port);
  // try to bind the mud port
  if (bind(control, (struct sockaddr *) &my_addr, sizeof(my_addr)) == -1)
    {
      world->WriteLog("Could not bind socket.", CRIT);
      close(control);
      return false;
    }
  // start listening
  if (listen(control, LISTEN_BACKLOG) == -1)
    {
      world->WriteLog("Could not set parent socket to listening state.", CRIT);
      close(control);
      return false;
    }
  return true;
}

int Server::GetListener(void) const
{
  return control;
}

BOOL Server::PollSockets()
{
  time_t res;
  static struct timeval tv;
  std::list<Socket*>::iterator iSocket;
  std::list<Socket*>::iterator iSocketEnd = socketList.end();
  Socket *sock = NULL;
  Player* mob = NULL;
  std::string input;
  World* world = World::GetPtr();

  for (iSocket = socketList.begin(); iSocket != iSocketEnd; ++iSocket)
    {
      sock = *iSocket;
//check for idlers:
      res = time(NULL)-sock->GetLastCommand();
      switch(sock->GetConnectionType())
        {
        case con_game:
          if (res>=GAME_IDLE_TIME)
            {
              if (sock->GetPlayer() && (!BitIsSet(sock->GetPlayer()->GetRank(), RANK_ADMIN)))
                sock->Write("You idled out, thanks for playing!\n");
              sock->Kill();
            }
          break;
//assume any others are logins.
        default:
          if (res >= LOGIN_IDLE_TIME)
            {
              sock->Write("You must log in within 30 seconds.\n");
              sock->Kill();
            }
        }
    }

  // copy the permanent descriptor set
  memcpy(&rSet, &fSet, sizeof(fd_set));
  // poll the descriptor set
  if (select(FD_SETSIZE, &rSet, NULL, NULL, &tv) < 0)
    {
      return false;
    }
  // attempt to establish new connections
  Accept();

  // iterate through all sockets and read pending data
  iSocketEnd = socketList.end();
  for (iSocket = socketList.begin(); iSocket != iSocketEnd; ++iSocket)
    {
      sock = *iSocket;
//see if the socket is set to be closed:
      if (sock->ShouldClose())
        {
          iSocket = CloseSocket(iSocket);
          continue;
        }
      // attempt to read from this socket if pending incoming data
      if (FD_ISSET(sock->GetControl(), &rSet))
        {
          // if read fails, close the connection
          if (sock->Read() == false)
            {
              iSocket = CloseSocket(iSocket);
              continue;
            }
//update their last input since we received data
          sock->UpdateLastCommand();
        }
    }
//now we poll every socket for pending commands and handle them.
  iSocketEnd = socketList.end();
  for (iSocket = socketList.begin(); iSocket != iSocketEnd; iSocket++)
    {
      sock = *iSocket;
      mob=sock->GetPlayer();

      if (!sock->CommandPending())   //no input pending to parse.
        {
          continue;
        }
//handle incoming data based on a connection type
      switch (sock->GetConnectionType())
        {
//handles all in-game connections that aren't handled with a function
        case con_game:
        {
//check to see if an input handler was associated with the socket before we pass to command parsing
          if (sock->HasHandle())
            {
              sock->HandleInput();
              break;
            }
//No handle was found, pass on to command parsing
          input=sock->PopCommand();
          if (input=="")
            {
              break;
            }
          world->DoCommand(mob, input);
          break;
        }
//login username prompt
        case con_name:
        {
//we associate a player with the socket object so we can store data and later load.
          sock->AllocatePlayer();
//we didn't have a mobile object before, now we need to set the mob pointer after allocation.
          mob = sock->GetPlayer();
          input = sock->PopCommand();
//new player:
          if (input=="new")
            {
              sock->Write("Welcome, what name would you like?\n");
              sock->SetConnectionType(con_newname);
              break;
            }
//checks to see if the username is valid
          if (!IsValidUserName(input))
            {
              sock->Write("Invalid name, try again.\n");
              break;
            }
//check to see if the player exists
          if (!PlayerExists(input))
            {
              sock->Write("That player doesn't exist.\nWhat is your name? Type new for a new character.\n");
              break;
            }
//set the players name to the one specified and try to load the file.
          mob->SetName(input);
          mob->Load();
          sock->Write(TELNET_ECHO_OFF);
          sock->Write("Password?\n");
          sock->SetConnectionType(con_password);
          break;
        }
//login password prompt
        case con_password:
        {
          input = sock->PopCommand();
//sets the temp password to that specified by the user so that we can compare with the password that was loaded from the file.
          mob->SetTempPassword(input);
          if (!mob->ComparePassword())
            {
              sock->Write("That password isn't valid!\n");
              mob->IncInvalidPassword();
              iSocket = CloseSocket(iSocket);
              break;
            }
//the player successfully logged in, set them to the game connection and run the player's EnterGame function to handle game entrance.
          sock->Write(TELNET_ECHO_ON);
          sock->SetConnectionType(con_game);
          mob->SetSocket(sock);
          mob->SetLastLogin((UINT)time(NULL));
          mob->EnterGame();
          break;
        }
//login new username
        case con_newname:
        {
          input = sock->PopCommand();
          if (!IsValidUserName(input))
            {
              sock->Write("That is not a valid username. Usernames must contain 4-12 characters.\nWhat name would you like?\n");
              break;
            }
          if (PlayerExists(input))
            {
              sock->Write("That player already exists, please try again.\nWhat name would you like?\n");
              break;
            }
//transfer control to retrieval of new password
          sock->Write("Please choose a password. Please make your password between 5 and 20 characters long.\nStrong passwords contain both lower and uppercase letters, numbers, letters and a dash ('-').\nEnter your new password?");
          sock->Write(TELNET_ECHO_OFF);
//name was valid, set it in the player class.
          mob->SetName(input);
          sock->SetConnectionType(con_newpass);
          break;
        }
        //login new password
        case con_newpass:
        {
          input = sock->PopCommand();
          if (!IsValidPassword(input))
            {
              sock->Write("That password isn't valid, please try again.\n");
              break;
            }
//transfer control to password verification
          sock->Write("Please re-enter your password for varification.\n");
          mob->SetPassword(input);
          sock->SetConnectionType(con_verpass);
          break;
        }
//login verify password
        case con_verpass:
        {
          input = sock->PopCommand();
          if (!IsValidPassword(input))
            {
              sock->Write("That password isn't valid, please try again.\n");
              break;
            }
          mob->SetTempPassword(input);
          if (!mob->ComparePassword())
            {
              sock->Write("That password isn't valid, please try again.\n");
//passwords  did not match, transfer control back to new password.
              sock->SetConnectionType(con_newpass);
              break;
            }
          sock->Write(TELNET_ECHO_OFF);
//passwords matched, see if the player is the first user. If so, make it a god.
          if (IsFirstUser())
            {
              mob->SetRank(BitSet(mob->GetRank(), RANK_PLAYTESTER|RANK_NEWBIEHELPER|RANK_BUILDER|RANK_ADMIN|RANK_GOD));
              sock->Write("You are the first player to create, rank set to God.\n");
            }

          mob->SetFirstLogin((UINT)time(NULL));
//Set the connection type to game and enter the player.
          sock->SetConnectionType(con_game);
          mob->SetSocket(sock);
//call the Create event:
          world->events.CallEvent("PlayerCreated", NULL, (void*)mob);
          mob->EnterGame();
          break;
        }
        }
    }
  return true;
}

void Server::FlushSockets()
{
  std::list<Socket*>::iterator iSocket;
  std::list<Socket*>::iterator iSocketEnd = socketList.end();
  Socket *pSocket;

  // iterate through all sockets and flush outgoing data
  for (iSocket = socketList.begin(); iSocket != iSocketEnd; ++iSocket)
    {
      pSocket = (*iSocket);
      // Attempt to flush this socket, close socket if failure
      if (pSocket->Flush() == false)
        {
          iSocket = CloseSocket(iSocket);
        }
    }
}

// Sleep() should be called with an argument that can be divided by 1000,
// like 4, 5, 8 or 10. This is the amount of "commands" that will
// be processed each second, and it is recommended to have a
// constant defined for this purpose.
void Server::Sleep(DWORD pps)
{
  struct timeval newTime;
  int secs, usecs;

  if (pps <= 0)
    {
      return;
    }
  gettimeofday(&newTime, NULL);

  // calculate exact amount of time we need to sleep
  usecs = (lastSleep.tv_usec -  newTime.tv_usec) + 1000000 / pps;
  secs  =  (lastSleep.tv_sec  -  newTime.tv_sec);

  while (usecs < 0)
    {
      usecs += 1000000;
      secs  -= 1;
    }
  while (usecs >= 1000000)
    {
      usecs -= 1000000;
      secs  += 1;
    }
  // sleep if needed
  if (secs > 0 || (secs == 0 && usecs > 0))
    {
      struct timeval sleepTime;

      sleepTime.tv_usec = usecs;
      sleepTime.tv_sec  = secs;
      select(0, NULL, NULL, NULL, &sleepTime);
    }

  // remember when we last slept
  gettimeofday(&lastSleep, NULL);
}

std::list<Socket*>::iterator Server::CloseSocket(std::list<Socket*>::iterator &it)
{
  Socket* tmp = (*it);
  tmp->Flush();
  tmp->Kill();
  // clear the sockets descriptor from the listening set
  FD_CLR(tmp->GetControl(), &fSet);
  // and finally delete the socket
  delete tmp;
//remove the socket.
  return socketList.erase(it);
}

void Server::Accept()
{
  Socket *pSocket;
  sockaddr_in addr;
  DWORD len = sizeof(addr);
  int desc, argp = 1;
  World* world = World::GetPtr();

//clear the address buffer:
  memset(&addr,0,len);
  // any new connections pending ?
  if (!FD_ISSET(control, &rSet))
    {
      return;
    }
  // try to accept new connection
  if ((desc = accept(control, (struct sockaddr *) &addr, &len)) == -1)
    {
      return;
    }
  // allocate a new socket
  pSocket = new Socket(desc);
  if (blist->AddressExists(addr.sin_addr.s_addr))
    {
      pSocket->Write("This IP has been banned.");
      pSocket->Kill();
      delete pSocket;
      return;
    }

//greet users:
  pSocket->Write("\n"+std::string(world->GetBanner())+"What is your name? Type new for a new character.\n");
  pSocket->Flush();

//Set the connection state
  pSocket->SetConnectionType(con_name);
  // set non-blocking I/O
  ioctl(desc, FIONBIO, &argp);
  // attach to socket list
  socketList.push_back(pSocket);
  // attach to file descriptor set
  FD_SET(desc, &fSet);
//copy the address to memory:
  memcpy(pSocket->GetAddr(),&addr,sizeof(sockaddr_in));
//perform lookup:
  struct hostent *host = NULL;
  host = gethostbyaddr((&addr.sin_addr), sizeof(in_addr), AF_INET);
  if ((!host)||(!host->h_name))
    {
      pSocket->SetHost(inet_ntoa(pSocket->GetAddr()->sin_addr));
    }
  else
    {
      pSocket->SetHost(std::string(host->h_name));
    }
  world->WriteLog("New connection from "+pSocket->GetHost()+".", CONNECTION);
}

std::list<Socket*> Server::GetSocketList()
{
  return socketList;
}

void Server::Recover(int listener)
{
  control=listener;
  FD_SET(control, &fSet);
}

void Server::AddSock(Socket* sock)
{
  socketList.push_back(sock);
  // attach to file descriptor set
  FD_SET(sock->GetControl(), &fSet);
}
BanList* Server::GetBanList() const
{
  return blist;
}
