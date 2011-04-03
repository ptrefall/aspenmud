#ifndef SCR_ENTITY_H
#define SCR_ENTITY_H
#include "../mud.h"
#include "../conf.h"
#include "../player.h"
#include "../entity.h"
#include "../world.h"
#include "../variant.h"
#include "../event.h"
#include "scripts.h"
#include <string>
#include <cstring>

#ifdef MODULE_SCRIPTING
EVENT(EVENT_INIT_ENTITY_SCRIPT);
BOOL InitEntityScript(void);
#endif
#endif
