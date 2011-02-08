/*
*The main living class
*Holds functions needed for both players and mobs.
*/

#ifndef LIVING_H
#define LIVING_H
#include <tinyxml.h>
#include "mud.h"
#include "conf.h"
#include "event.h"
#include "entity.h"

class Living:public Entity
{
public:
    Living();
    virtual ~Living();
    /*
    *This is called when an object enters or leaves the game environment
    */
    virtual void EnterGame(void);
    virtual void LeaveGame(void);
    /*
    *This is updated on a set interval to keep the objects up-to-date.
    *This includes heartbeat, removing uneeded objects, etc.
    */
    virtual void Update();

    /*
    *Find out what type of living this is
    *Return: False
    *Note: Should overwrite this for player and NPC objects to return true for each object.
    */
    virtual BOOL IsNpc(void);
    virtual BOOL IsPlayer(void);

    virtual void Serialize(TiXmlElement* root);
    virtual void Deserialize(TiXmlElement* root);
};
#endif
