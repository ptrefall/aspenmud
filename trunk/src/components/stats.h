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
  EVENT(attached);
  static EVENT(Added);
  static COMCREATE(Create);
  static COMINIT(Initialize);
};
#endif
