/*
*A class for handling client sockets
*Also handles telnet negotiation.
*/
#ifndef SOCKET_H
#define SOCKET_H
#include <queue>
#include <stack>
#include <string>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "mud.h"
#include "conf.h"
#include "player.h"
#include "baseSocket.h"

/*
*This is the connection type that each player has, it is used to hold the state of their current connection.
*/
enum ConType {con_name,con_password,con_newname,con_newpass,con_verpass,con_game};
class Socket;

/*
*The input handler is used to allow for another function to receive input.
*This is particularly useful when you want to request input from a user, and you can't stop the game. In order to request input, you create a handler, set the args field to the object that will receive the notification and push it to the input stack.
*/
struct in_data;
class InputHandle
{
protected:
  Socket* _sock;
public:
  InputHandle();
  ~InputHandle();
  void Attach(Socket* sock);
  virtual void Active(in_data* in);
  virtual void Input(void* arg, const std::string &input);
};

struct in_data
{
  InputHandle *handle;
  void* args;
};

class Socket:public BaseSocket
{
  std::stack <in_data*> *_input;
  std::queue<std::string> _cqueue;
  std::string _host;
  ConType _con;
  Player *_mobile;
  BOOL _Close;
  bool _inSequence;
  std::string _telnetBuffer;
  time_t _lastInput;
public:
  /*
  *Constructer
  *Param: [in] the FD to assign to the socket.
  */
  Socket  (const int desc );
  ~Socket ();
  /*
  *Reads the pending data and places it in the sockets input buffer.
  *Return: True on success, false on failure.
  */
  bool                 Read           ();
  /*
  *Flushes the sockets output buffer to the client.
  *Return: true on success, false on failure.
  */
  bool                 Flush          ( void );
  /*
  *Returns the input buffer.
  *Return: The input buffer of the socket.
  */
  std::string          GetInBuffer    ( void );
  /*
  *Returns the connection type
  *Return: the type of connection that is associated with this socket.
  */
  ConType GetConnectionType() const;
  /*
  *Sets the connection type.
  *Param: [in] The type of connection.
  */
  void SetConnectionType(const ConType &s);
  /*
  *Returns the host
  *Return: The hostname or IP address associated with the socket.
  */
  std::string GetHost() const;
  /*
  *Sets the hostname associated with the socket.
  *Param: [in] The hostname associated with the socket.
  */
  void SetHost(const std::string &host);
  /*
  *Allocates a player for the socket, if one isn't already assigned.
  */
  void AllocatePlayer();
  /*
  *Returns a pointer to the player object.
  *Return: A pointer to the player object associated with the socket.
  */
  Player*GetPlayer() const;
  /*
  *Kills the socket's connection.
  *Param: [in] a pointer to the world object.
  */
  void Kill();
  /*
  *Checks to see if the socket needs to be closed on next update.
  *Return: true/false.
  */
  BOOL ShouldClose();
  /*
  *Checks to see if the socket has a handle associated with it.
  *This is used for capturing input before it hits the command checking.
  *Return: True if a handle is linked to the socket, false otherwise.
  */
  BOOL HasHandle() const;
  /*
  *Passes the input from the socket to the associated handle, if one exists.
  *Return: True if the input was sent to the handle, false if no handle was associated with the socket.
  */
  BOOL HandleInput();
  /*
  *Clears the current input handler.
  */
  void ClearInput();
  /*
  *Sets the input handler.
  *Param: [in] a pointer to the function that will capture input.
  *Return: True on success, false if an input handler is already associated with the socket.
  */
  BOOL SetInput(in_data* data);
  /*
  *Updates the time at which the last command was entered.
  *This is used for checking how long a player has been idle, etc.
  */
  void UpdateLastCommand();
  /*
  *Return: the last time a command was entered.
  */
  time_t GetLastCommand();
  /*
  *Checks to see if a command is pending in the queue or not.
  *Return: True if one or more commands is in the command queue, false otherwise.
  */
  BOOL CommandPending() const;
  /*
  *Pops a command from the queue.
  *Return: The command at the front of the queue, or an empty string if the queue is empty.
  */
  std::string PopCommand();
  /*
  *Adds a command to the queue.
  *Param: [in] the command to add.
  */
  void AddCommand(const std::string &input);
};
#endif
