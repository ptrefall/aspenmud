/*
*main.cpp
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


#include <unistd.h>
#include <signal.h>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <cstdio>
#include "mud.h"
#include "conf.h"
#include "socket.h"
#include "server.h"
#include "log.h"
#include "player.h"
#include "command.h"
#include "channel.h"
#include "modules.h"
#ifdef MODULE_SCRIPTING
#include "scripts/scripts.h"
#endif
#include "zone.h"
#include "components.h"
#ifdef OLC
#include "olc.h"
#endif
#include "socials.h"

//prototypes:
//used to load essential files
static void CopyoverRecover(void);
//the main game loop function
static void GameLoop();
//our signal callback
void sig(int sig);

World* world;
BOOL running;

int main(int argc, char** argv)
{
    BOOL copyover=false; //are we rebooting for copyover?
    int listener=0; //the socket to listen on when recovering from copyover
    world=new World();
    world->WriteLog("Initializing "+MUD_NAME+".");
//make sure the mud isn't ran as root:
#ifdef ASPEN_UNIX
    if (getuid()==0) {
        world->WriteLog("This program may not be ran as root, now exiting.");
        return 1;
    }
#endif
//initialize the server class:
    int port;
//determine if a port was specified. If not, use default.
    switch (argc) {
    case 2: {
        port = atoi(argv[1]);
        if ((port < 1024)||(port>65535)) {
            world->WriteLog("Invalid port specified, program will now exit.");
            return 1;
        }
        break;
    }
    case 3: {
        if ((!strcmp(argv[1],"-c"))&&(atoi(argv[2])>0)) {
            copyover=true;
            listener=atoi(argv[2]);
        }
    }
    default:
        port=DEFAULT_PORT;
        break;
    }
    if (!world->InitializeFiles()) {
        return 1;
    }
//game initialization calls
    InitializeCommands();
    InitializeChannels();
#ifdef MODULE_SCRIPTING
    if (!InitializeScript()) {
        return 1;
    }
#endif
#ifdef OLC
    InitializeOlc();
#endif
    InitializeModules();
    if (!InitializeZones()) {
        return 1;
    }
    world->InitializeNums();
    InitializeSocials();
    CreateComponents();
    world->SetRealUptime(time(NULL));
    world->SetCopyoverUptime(time(NULL));
    srand(time(NULL));
//make the server listen:
    world->WriteLog("Attempting to establish listening point.");
    if (copyover) {
//set the listening socket to the descripter specified
        world->GetServer()->Recover(listener);
//load all saved connections
        CopyoverRecover();
    } else {
        if (!world->GetServer()->Listen(port)) {
            return 1;
        }
    }
//initialize signal callbacks
    world->WriteLog("Initializing signal callbacks.");
    signal(SIGTERM,sig);
    signal(SIGINT,sig);
    signal(SIGQUIT,sig);
    signal(SIGHUP,sig);
//start the game loop:
    running=true;
    world->WriteLog("Entering game loop.");
    GameLoop();
    world->WriteLog("Game loop finished, exiting.");
    delete world;
    return 0;
}

static void CopyoverRecover(void)
{
    FILE* recover;
    short family;
    unsigned short port;
    unsigned long addr;
    char *name=new char[15];
    char* host=new char[256];
    int desc;
    int cuptime, ruptime;

    world->WriteLog("Starting copyover recovery");
    recover=fopen(COPYOVER_FILE,"rb");
    if (recover==NULL) {
        world->WriteLog("There was an error opening the copyover recovery file, now exiting.", ERR);
        exit(1);
    }

    fscanf(recover, "%d %d\n", &cuptime, &ruptime);
    world->SetRealUptime((time_t)ruptime);
    world->SetCopyoverUptime((time_t)cuptime);
    while (1) {
        memset(name, 0, 15);
        memset(host, 0, 256);
        fscanf(recover,"%d %s %hd %hu %lu %s\n",
               &desc,name,&family,&port,&addr, host);
        if (desc==-1) {
            break;
        }
        Socket* sock=new Socket(desc);
        sock->GetAddr()->sin_family=family;
        sock->GetAddr()->sin_port=port;
        sock->GetAddr()->sin_addr.s_addr=addr;
        sock->SetHost(host);
        sock->AllocatePlayer();
        world->WriteLog("Loading "+std::string(name)+".");
        sock->GetPlayer()->SetName(name);
        sock->GetPlayer()->Load();
        sock->SetConnectionType(con_game);
        world->GetServer()->AddSock(sock);
        sock->GetPlayer()->SetSocket(sock);
        sock->GetPlayer()->EnterGame(false);
        sock->Write("Copyover complete.\n");
    }
    delete []name;
    delete []host;
    fclose(recover);
    remove(COPYOVER_FILE);
    world->WriteLog("Copyover completed.");
}

static void GameLoop()
{
    while (running) {
//update our world:
        world->Update();
    }
//the game loop finished, shutdown world.
    world->Shutdown();
    world->Update();
}

void sig(int sig)
{
    world->WriteLog("Caught signal, cleaning up.");
    running=false;
}
