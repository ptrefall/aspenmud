/*
*world.h
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
*world
*This class is responsible for managing users, zones, mobs and coordenating the mud world
*All of the core functions will be kept within this class, and tasks delegated to other classes as needed for organizational purposes.
*/

#ifndef WORLD_H
#define WORLD_H
#include "mud.h"
#include "conf.h"
#include "server.h"
#include "log.h"
#include "command.h"
#include "channel.h"
#include "event.h"
#include "player.h"
#include "component.h"
#include "ComponentFactory.h"
#include "room.h"
#include "zone.h"

class World:public EventManager, public Commandable
{
    Server* _server; //our main server object.
    Log *_log;
    ComponentFactory* _cfactory;
    std::list <Player*> *_users; //a list of the currently connected players.
    std::map <int,Channel*>* _channels;
    std::map <std::string,void*> *_properties;
    std::list<Zone*> *_zones;
    std::map<VNUM,Room*> *_rooms;
    std::map<VNUM,Entity*> *_objects;
    std::list<VNUM> *_onumPool;
    std::list <VNUM> *_rnumPool;
    VNUM _maxOnum;
    VNUM _maxRnum;
    int _chanid;
    time_t _ruptime; //real uptime.
    time_t _cuptime; //uptime since last copyover.
//mud text buffers
    char* _motd;
    char* _banner;
public:
//Events
//methods:
    World();
    ~World();
    /*
    *Shuts down the mud: saves open objects, sends messages to players, etc.
    */
    void Shutdown(void);
    /*
    *Returns: a pointer to the main server object.
    */
    Server* GetServer(void) const;
    /*
    *Returns: a pointer to the log object.
    */
    Log* GetLog(void) const;
    /*
    *Returns: a pointer to a list of pointers to player objects.
    */
    std::list <Player*> *GetPlayers(void) const;
    /*
    *Adds the player to the players list.
    *Only connected players should be added. Inactive players shouldn't be in this list.
    *Param: [in] a pointer to the player object to add.
    */
    void AddPlayer(Player* player);
    /*
    *Removes the specified player from the list of active players.
    *Param: [in] A pointer to the player object
    */
    void RemovePlayer(Player* player);
    /*
    *Locates the specified player:
    *Param: [in] The name of the player.
    *Return: A pointer to the player's object, NULL if the player wasn't found.
    */
    Player* FindPlayer(const std::string &name) const;
    /*
    *Loads the specified player and returns a pointer to the object.
    *Param: [in] the name of the player to load.
    *Return: a pointer to the player's object, if the player could be loaded.
    */
    Player* LoadPlayer(const std::string &name) const;
    /*
    *Adds a channel to the worlds channel lookup table.
    *Param: [in] A pointer to the object to add.
    */
    void AddChannel(Channel* chan,BOOL command=true);
    /*
    *Will locate a channel object based on it's ID.
    *Param: [in] the id of the channel.
    *Return: a pointer to the channel object if it was found, NULLotherwise.
    */
    Channel* FindChannel(const int id) const;
    /*
    *Will locate a channel object based on it's name.
    *Param: [in] The name of the channel to locate.
        *Return: a pointer to the channel object if it was found, NULLotherwise.
    */
    Channel* FindChannel(const std::string &name);
    /*
    *Returns a list of all channel names, used to show channels and to allow for easily getting access to their pointers.
    *Param: [out] A pointer to a list of strings that will receive the chanel names.
    */
    void GetChannelNames(std::list <std::string>* out);
    /*
    *Registers the specified component
    *Params: [in] The name of the component.
    *[in] The component registration callback.
    *[in] the component init callback.
    *Return: True if the component was registered, false otherwise.
    */
    BOOL RegisterComponent(const std::string &name,COMCREATECB cb, COMINITCB ib);
    /*
    *Creates the specified component
    *Parameters: [in] the name of the component to create.
    *Return: The object created.
    */
    Component*  CreateComponent(const std::string &name);
    /*
    *Adds a room to the mapping.
    *Param: [in] The number of the room object.
    *[in] A pointer to the object to add.
    *Return: True on success, false otherwise.
    */
    BOOL AddRoom(VNUM num,Room* room);
    /*
    *Gets a room object based on its number.
    *Param: [in] the number of the room object to retrieve.
    *Return: A pointer to the room object, or NULL if it wasn't in the list.
    */
    Room* GetRoom(VNUM num);
    /*
    *Removes a room from the list
    *Param: [in] the number of the object to remove
    *Return: True on success, false on failure.
    */
    BOOL RemoveRoom(VNUM num);
    /*
    *Checks to see if the specified room exists.
    *Return: True on success, false otherwise.
    */
    BOOL RoomExists(VNUM num);
    /*
    *Adds an object to the object table.
    *Param: [in] a VNUM for the object to add.
    *[in] A pointer to the object.
    *Return: True on success, false on failure.
    */
    BOOL AddObject(VNUM num,Entity* obj);
    /*
    *Retrieves a pointer to the specified object.
    *Param: [in] the VNUM of the object.
    *Return: A pointer to the object, NULL on failure.
    */
    Entity* GetObject(VNUM num);
    /*
    *Removes an object from the object table.
    *Param: [in] the VNUM of the object to remove.
    *Return: True on success, false on failure.
    */
    BOOL RemoveObject(VNUM num);
    /*
    *checks for the existance of an object.
    *Param: [in] The vnum of the object.
    *Return: True if the object exists, false otherwise.
    */
    BOOL ObjectExists(VNUM num);
    /*
    *Gets the real uptime.
    *Return: the time the mud was last rebooted.
    */
    time_t GetRealUptime(void) const;
    /*
    *Sets the real uptime
    *Param: [in] the time of the last hard reboot.
    */
    void SetRealUptime(time_t tm);
    /*
    *gets the time of the last copyover.
    *Return: time of last copyover.
    */
    time_t GetCopyoverUptime(void) const;
    /*
    *Sets the time of the last copyover.
    *Param: [in] the time of last copyover.
    */
    void SetCopyoverUptime(time_t tm);

    /*
    *Adds a property to the world's property table.
    *Param: [in] the name of the property to add.
    *[in] a pointer to the object.
    *Return: true on success, false if the property couldn't be added (null pointer provided, or the property already exists).
    */
    BOOL AddProperty(const std::string &name,void* ptr);
    /*
    *Retrieves the property from the world's property table.
    *Param: [in] the name of the property.
    *Return: a pointer to the object, NULL if the property wasn't found.
    */
    void* GetProperty(const std::string &name) const;
    /*
    *Removes the property from the property table.
    *Param: [in] the name of the property to remove.
    *Return: true on success, false otherwise.
    *Note: this will not call delete on the object associated with that property.
    */
    BOOL RemoveProperty(const std::string &name);
    /*
    *Loads files into memory such as the MOTD so we won't have to read them every time we need them.
    */
    BOOL InitializeFiles(void);
    /*
    *The following functions return pointers to memory where the loaded files can be found.
    */
    const char* GetBanner(void) const;
    const char* GetMotd(void) const;
    /*
    *Updates the world.
    *Polls sockets, updates events, and keeps the world clean with tasks that should be ran every game loop.
    *Keep this function as minimal as possible; everything that occurs here runs every game loop iteration.
    */
    void Update(void) const;
    /*
    *Will perform a command for the specified player.
    *Params: [in] a pointer to the player object.
    *[in] A string with the arguments passed to the command.
    *Returns: true on success, false on failure.
    */
    BOOL DoCommand(Player* mobile, std::string args);
    /*
    *Finds an object from the input.
    *Param: [in] the name of the object.
    *Return: A pointer to the object, NULL if it wasn't found.
    */
    Entity* MatchObject(const std::string &name,Player* caller);
    /*
    *Adds a zone to the list of zones.
    *Param: [in] a pointer to the zone to add.
    *Return: True on success, false on failure.
    */
    BOOL AddZone(Zone* zone);
    /*
    *Removes the zone from the list.
    *Param: [in] a pointer to the object to remove.
    *Return: True on success, false on failure.
    */
    BOOL RemoveZone(Zone* zone);
    /*
    *Retrieves a pointer to the specified zone.
    *Param: [in] The name of the zone object.
    *Return: A pointer to the object, NULLif it wasn't found.
    */
    Zone* GetZone(const std::string &name);
    void GetZones(std::list <Zone*> *zones);
    /*
    *Assigns a vnum to the object, and adds it to the registry.
    *Param: [in] a pointer to the object.
    *Return: True on success, false on failure.
    */
    BOOL CreateObject(Entity* obj);
    BOOL  CreateRoom(Room* room);
    void InitializeNums(void);
    void WriteLog(const std::string &data,LOG_LEVEL l=INFORM);
};

#endif