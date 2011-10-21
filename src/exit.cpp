#include <string>
#include <tinyxml.h>
#include "exit.h"
#include "living.h"

Exit::Exit(VNUM to)
{
  _to=to;
}
Exit::Exit(void)
{
  _to=EXIT_NOWHERE;
}
Exit::~Exit(void)
{
}

VNUM Exit::GetTo(void) const
{
  return _to;
}
void Exit::SetTo(VNUM to)
{
  _to=to;
}

ExitDirection Exit::GetDirection(void) const
{
  return _direction;
}
void Exit::SetDirection(ExitDirection dir)
{
  _direction = dir;
}

std::string Exit::GetName() const
{
  switch(_direction)
    {
    case north:
      return "north";
    case south:
      return "south";
    case east:
      return "east";
    case west:
      return "west";
    case northwest:
      return "northwest";
    case northeast:
      return "northeast";
    case southwest:
      return "southwest";
    case southeast:
      return "southeast";
    default:
      return "unknown";
    }
}

BOOL Exit::CanEnter(Living* mobile)
{
  return true;
}

void Exit::Serialize(TiXmlElement* root)
{
  TiXmlElement* node = new TiXmlElement("exit");

  int tmp = (int)_direction;
  node->SetAttribute("direction", tmp);
  node->SetAttribute("to", _to);
  root->LinkEndChild(node);
}
void Exit::Deserialize(TiXmlElement* node)
{
  int tmp = 0;

  node->Attribute("to", &_to);
  node->Attribute("direction", &tmp);
  _direction = (ExitDirection)tmp;
}
