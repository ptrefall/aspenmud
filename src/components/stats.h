#ifndef STATS_H
#define STATS_H
#include "../mud.h"
#include "../conf.h"
#include "../component.h"
#include "../event.h"

class Stats:public Component
{
public:
  Stats(void);
};

//prototypes
COMCREATE(STATS_CREATE);
COMINIT(STATS_INIT);
EVENT(STATS_ATTACHED);
EVENT(STATS_ADD);


#endif
