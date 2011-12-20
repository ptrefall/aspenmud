#ifndef NPC_H
#define NPC_H
#include "mud.h"
#include "conf.h"
#include "living.h"

class Npc:public Living
{
public:
  BOOL IsNpc() const;
};
#endif
