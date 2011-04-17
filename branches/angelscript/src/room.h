/*
*The basic room object
*/
#ifndef ROOM_H
#define ROOM_H
#include <list>
#include <string>
#include <tinyxml.h>
#include "mud.h"
#include "conf.h"
#include "entity.h"
#include "living.h"
#include "exit.h"
#include "player.h"
#include "event.h"
#include "zone.h"

EVENT(ROOM_POST_LOOK);

class Zone;

class Room:public Entity
{
  std::list <Living*> _mobiles;
  std::vector<Exit*> *_exits;
  Zone* _zone;
public:
  Room(void);
  virtual ~Room(void);
  /*
  *Adds the specified exit.
  *Param: [in] a pointer to the Exit object to be added.
  *Return: True if the exit could be added, false otherwise.
  */
  virtual BOOL AddExit(Exit* exit);
  /*
  *Checks for the existance of an exit
  *Param: [in] the direction of the exit to check for.
  *Return: True if the exit exists, false otherwise.
  */
  virtual BOOL ExitExists(ExitDirection dir);
  /*
  *Finds the specified exit.
  *Param: [in] the name of the exit.
  *Return: a pointer to the exit object found, or NULL if none was found.
  */
  virtual Exit* GetExit(ExitDirection dir);
  /*
  *Returns a pointer to the list of exits.
  *Return: a pointer to a vector of pointers to Exit objects
  */
  std::vector<Exit*>* GetExits(void);
  /*
  *Sets the zone object on the room.
  *Param: [in] a pointer to a zone.
  */
  void SetZone(Zone* s);
  /*
  *Gets the zone associated with the room object.
  *Return: The zone for the object, or NULL.
  */
  Zone* GetZone(void) const;
  /*
  *Sends a message to all players in the room.
  *Param: [in] the message to send.
  */
  virtual void TellAll(const std::string &message);
  virtual void TellAllBut(const std::string & message, std::list <Player*>* players);
//serialization
  virtual void Serialize(TiXmlElement* root);
  virtual void Deserialize(TiXmlElement* node);
};
#endif
