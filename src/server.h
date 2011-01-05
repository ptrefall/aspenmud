/*
*server.h
*
*   Copyright 2010 Tyler Littlefield.
*
*   Licensed under the Apache License, Version 2.0 (the "License");
*   you may not use this file except in compliance with the License.
*   You may obtain a copy of the License at
*
*       http://www.apache.org/licenses/LICENSE-2.0
*
*   Unless required by applicable law or agreed to in writing, software
*   distributed under the License is distributed on an "AS IS" BASIS,
*   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*   See the License for the specific language governing permissions and
*   limitations under the License.
*/


/*
*Main server class
*Handles incoming connections, and passes info to the parser.
*/

#ifndef HEADER_SERVER
#define HEADER_SERVER
#include "mud.h"
#include "conf.h"
#include "socket.h"

class Server
{
    /*
    *Accepts all incoming connections
    */
    void                 Accept           ( void );
    /*
    *Closes a specified socket and remove it from the list
    */
    std::list<Socket*>::iterator CloseSocket      ( Socket *pSocket );

    std::list<Socket*>   socketList;
    int control;
    fd_set               fSet;
    fd_set               rSet;
    sockaddr_in          my_addr;
    struct timeval       lastSleep;
public:
    Server  (void);
    ~Server ( void );
    /**
    *Establishes a listening point
    *param [in] The port to open as a listening poDWORD.
    *Returns True on success, false on failure.
    */
    BOOL                 Listen          (const int port );
    /*
    *Returns the listening socket
    */
    int GetListener(void) const;
    /**
    *Flushes all sockets in the list
    *Sends the data in the input buffer (if any) to the socket and clears it.
    */
    void                 FlushSockets     ( void );
    /*
    *Checks to make sure that no incoming connections need to be accepted, reads and handles data from sockets.
    *returns True on success, false on failure.
    */
    BOOL PollSockets(void );
    /**
    *sleeps for the remainder of the pulses that haven't been used up.
    */
    void Sleep            ( DWORD pps );
    /**
    *returns A list of pointers to the currently connected sockets.
    */
    std::list<Socket*>   GetSocketList    ( void );
//Sets the specified socket to the listener socket and put it in the descriptor set
    void Recover(int listener);
//Adds the specified socket to the socket list and adds it to the descriptor set:
    void AddSock(Socket* sock);
};
#endif

