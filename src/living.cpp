#include "living.h"
#include "event.h"
#include "delayedEvent.h"
#include <tinyxml.h>

Living::Living()
{
  events.RegisterEvent("HeartBeat", new DelayedEvent(LIVING_PULSE,0));
}
Living::~Living()
{
}

void Living::EnterGame()
{
}
void Living::LeaveGame()
{
}

void Living::Update()
{
  events.CallEvent("HeartBeat", NULL, (void*)this);
}

BOOL Living::IsNpc(void)
{
  return false;
}
BOOL Living::IsPlayer(void)
{
  return false;
}

void Living::Serialize(TiXmlElement* root)
{
  TiXmlElement* node = new TiXmlElement("living");
  Entity::Serialize(node);
  root->LinkEndChild(node);
}
void Living::Deserialize(TiXmlElement* root)
{
  Entity::Deserialize(root->FirstChild("entity")->ToElement());
}
