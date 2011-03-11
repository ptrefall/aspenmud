/*
*A class for handling sockets
*Also handles MCCP and telnet negotiation.
*/

#ifndef SOCKET_H
#define SOCKET_H
#include <queue>
#include <list>
#include <string>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "mud.h"
#include "conf.h"
#include "player.h"

enum ConType {con_name,con_password,con_newname,con_newpass,con_verpass,con_game};
class Socket;

typedef void (*IN_HANDLER)(Socket*, void*, const std::string&);
#define INPUT(name)\
void name(Socket* sock, void* arg, const std::string &input)

struct in_data
{
  IN_HANDLER handle;
  void* args;
};

class Socket
{
  int                  _control;
  std::list <in_data*> _input;
  std::queue<std::string> _cqueue;
  sockaddr_in          *_addr;
  std::string          _inBuffer;
  std::string          _outBuffer;
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
  *Param: [in] the fdto assign to the socket.
  */
  Socket  (const int desc );
  ~Socket ( void );
  /*
  *Returns the "control" fd.
  *Return: the fd associated with the socket.
  */
  int                  GetControl     ( void ) const;
  /*
  *Reads the pending data and places it in the sockets input buffer.
  *Return: True on success, false on failure.
  */
  bool                 Read           ( void );
  /*
  *Appends the text to the output buffer.
  *Param: [in] the data to append to the output buffer.
  */
  void                 Write          (const std::string &txt );
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
  *Clears the input buffer.
  */
  void                 ClrInBuffer    ( void );
  /*
  *Returns the address of the socket.
  *Return: The socket's addr struct.
  */
  sockaddr_in* GetAddr(void) const;
  /*
  *coppies the address to the socket's address buffer.
  *Param: [in] a pointer to a sockaddr_in structure.
  */
  void SetAddr(sockaddr_in* addr);
  /*
  *Returns the connection type
  *Return: the type of connection that is associated with this socket.
  */
  ConType GetConnectionType(void) const;
  /*
  *Sets the connection type.
  *Param: [in] The type of connection.
  */
  void SetConnectionType(const ConType &s);
  /*
  *Returns the host
  *Return: The hostname or IP address associated with the socket.
  */
  std::string GetHost(void) const;
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
  Player*GetPlayer(void) const;
  /*
  *Kills the socket's connection.
  *Param: [in] a pointer to the world object.
  */
  void Kill();
  /*
  *Checks to see if the socket needs to be closed on next update.
  *Return: true/false.
  */
  BOOL ShouldClose(void);
  /*
  *Checks to see if the socket has a handle associated with it.
  *This is used for capturing input before it hits the command checking.
  *Return: True if a handle is linked to the socket, false otherwise.
  */
  BOOL HasHandle(void) const;
  /*
  *Passes the input from the socket to the associated handle, if one exists.
  *Return: True if the input was sent to the handle, false if no handle was associated with the socket.
  */
  BOOL HandleInput(void);
  /*
  *Clears the current input handler.
  */
  void ClearInput(void);
  /*
  *Sets the input handler.
  *Param: [in] a pointer to the function that will capture input.
  *Return: True on success, false if an input handler is already associated with the socket.
  */
  BOOL SetInput(in_data* data);
  void UpdateLastInput(void);
  time_t GetLastInput(void);
  BOOL InputPending() const;
  std::string PopInput();
  void AddInput(const std::string &input);
};

#endif
